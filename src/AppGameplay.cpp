#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include <algorithm>
#include <cstdlib>

void App::UpdateGameplay() {
    // Spawn sky suns periodically
    m_SunSpawnTimer++;
    if (m_SunSpawnTimer >= SUN_SPAWN_INTERVAL) {
        SpawnSkySun();
        m_SunSpawnTimer = 0;
    }

    // Update all sun objects
    for (auto& sun : m_Suns) {
        sun->Update();
    }

    // Remove collected suns and add value
    m_Suns.erase(
        std::remove_if(m_Suns.begin(), m_Suns.end(),
            [&](const std::shared_ptr<Sun>& sun) {
                if (sun->GetState() == Sun::State::DONE) {
                    m_Root.RemoveChild(sun);
                    m_SunAmount += Sun::VALUE;
                    m_SunCounter->SetAmount(m_SunAmount);
                    LOG_DEBUG("Sun collected! Total: {}", m_SunAmount);
                    return true;
                }
                return false;
            }),
        m_Suns.end());

    // Update card cooldowns
    for (auto& card : m_Cards) {
        card->UpdateCooldown();
    }

    // Update holding plant position to follow cursor
    if (m_HoldingPlant) {
        auto cursor = Util::Input::GetCursorPosition();
        m_HoldingPlant->m_Transform.translation = cursor;
    }

    // Handle left click
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        auto click = Util::Input::GetCursorPosition();

        if (m_HoldingPlant) {
            glm::vec2 worldPos = click - glm::vec2{m_CameraOffset, 0.0f};
            auto [row, col] = GridSystem::PositionToCell(worldPos);

            if (GridSystem::IsValidCell(row, col) && !m_PlantGrid[row][col]) {
                PlacePlant(row, col);
            }
        } else {
            for (auto& sun : m_Suns) {
                if (sun->TryCollect(click)) break;
            }
            for (auto& card : m_Cards) {
                if (card->ContainsPoint(click) && card->IsAvailable(m_SunAmount)) {
                    m_SelectedCard = card;
                    m_HoldingCardIndex = card->GetIndex();

                    m_HoldingPlant = std::make_shared<Plant>(
                        card->GetIndex(), -1, -1);
                    m_HoldingPlant->SetZIndex(90);
                    m_HoldingPlant->m_Transform.translation = click;
                    m_Root.AddChild(m_HoldingPlant);

                    LOG_DEBUG("Picked up plant {} (cost: {})",
                              card->GetIndex(), card->GetCost());
                    break;
                }
            }
        }
    }

    // Cancel with right click
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
        if (m_HoldingPlant) {
            CancelHolding();
        }
    }

    // Zombie spawning
    ++m_ZombieSpawnTimer;
    if (m_ZombieSpawnTimer >= ZOMBIE_SPAWN_INTERVAL
        && m_ZombiesSpawned < ZOMBIES_PER_LEVEL) {
        SpawnZombie();
        m_ZombieSpawnTimer = 0;
    }
    
    UpdatePlantShooting();
    UpdateBullets();
    UpdateZombieEating();
    UpdateZombies();
    UpdateLawnMowers();
}

void App::PlacePlant(int row, int col) {
    glm::vec2 screenPos = GridSystem::CellToPosition(row, col)
                          + glm::vec2{m_CameraOffset, 0.0f};

    auto plant = std::make_shared<Plant>(m_HoldingCardIndex, row, col);
    plant->SetZIndex(10);
    plant->m_Transform.translation = screenPos;
    m_PlantGrid[row][col] = plant;
    m_Root.AddChild(plant);

    m_Root.RemoveChild(m_HoldingPlant);
    m_HoldingPlant = nullptr;

    m_SunAmount -= m_SelectedCard->GetCost();
    m_SunCounter->SetAmount(m_SunAmount);
    m_SelectedCard->Use();

    LOG_DEBUG("Placed plant {} at ({}, {}), sun remaining: {}",
              m_HoldingCardIndex, row, col, m_SunAmount);

    m_SelectedCard = nullptr;
    m_HoldingCardIndex = -1;
}

void App::CancelHolding() {
    m_Root.RemoveChild(m_HoldingPlant);
    m_HoldingPlant = nullptr;
    m_SelectedCard = nullptr;
    m_HoldingCardIndex = -1;
    LOG_DEBUG("Plant placement cancelled");
}

void App::SpawnSkySun() {
    float x = static_cast<float>(rand() % 650 - 200);
    float y = static_cast<float>(rand() % 250 - 100);

    auto sun = std::make_shared<Sun>(glm::vec2{x, y});
    m_Suns.push_back(sun);
    m_Root.AddChild(sun);
    LOG_DEBUG("Sky sun spawned at target ({}, {})", x, y);
}

void App::SpawnZombie() {
    int row = rand() % GridSystem::ROWS;
    float yPos = GridSystem::CellToPosition(row, GridSystem::COLS - 1).y;

    auto zombie = std::make_shared<NormalZombie>(row);
    zombie->SetZIndex(15);
    zombie->m_Transform.translation = {620.0f + m_CameraOffset, yPos};
    m_Zombies.push_back(zombie);
    m_Root.AddChild(zombie);
    ++m_ZombiesSpawned;
    LOG_DEBUG("Zombie spawned in row {} (total: {})", row, m_ZombiesSpawned);
}

void App::UpdateZombies() {
    for (auto& zombie : m_Zombies) {
        zombie->Update();
    }

    m_Zombies.erase(
        std::remove_if(m_Zombies.begin(), m_Zombies.end(),
            [&](const std::shared_ptr<Zombie>& z) {
                if (z->IsDead() || z->IsPastLine(ZOMBIE_LOSE_X)) {
                    m_Root.RemoveChild(z);
                    return true;
                }
                return false;
            }),
        m_Zombies.end());
}

void App::UpdatePlantShooting() {
    for (int row = 0; row < GridSystem::ROWS; ++row) {
        for (int col = 0; col < GridSystem::COLS; ++col) {
            auto& plant = m_PlantGrid[row][col];
            if (!plant || !plant->CanShoot()) continue;

            // Only shoot if there's a zombie ahead in the same row
            bool zombieAhead = false;
            for (const auto& z : m_Zombies) {
                if (z->GetRow() == row && z->IsAlive()
                    && z->GetX() > plant->m_Transform.translation.x) {
                    zombieAhead = true;
                    break;
                }
            }
            if (!zombieAhead) continue;

            if (plant->TryShoot()) {
                glm::vec2 bulletPos = plant->m_Transform.translation;
                bulletPos.x += 30.0f;
                bulletPos.y += 20.0f;
                auto bullet = std::make_shared<Bullet>(bulletPos);
                bullet->SetRow(row);
                m_Bullets.push_back(bullet);
                m_Root.AddChild(bullet);
            }
        }
    }
}

void App::UpdateBullets() {
    for (auto& bullet : m_Bullets) {
        bullet->Update();
    }

    // Check bullet-zombie collisions
    for (auto& bullet : m_Bullets) {
        if (bullet->IsHit()) continue;
        for (auto& zombie : m_Zombies) {
            if (!zombie->IsAlive()) continue;
            if (zombie->GetRow() != bullet->GetRow()) continue;
            if (bullet->HitCheck(zombie->m_Transform.translation)) {
                zombie->TakeDamage(bullet->GetDamage());
                bullet->MarkHit();
                break;
            }
        }
    }

    // Remove dead bullets
    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [&](const std::shared_ptr<Bullet>& b) {
                if (b->IsHit() || b->IsOffScreen()) {
                    m_Root.RemoveChild(b);
                    return true;
                }
                return false;
            }),
        m_Bullets.end());
}

void App::UpdateZombieEating() {
    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) continue;

        int row = zombie->GetRow();
        bool foundPlant = false;

        for (int col = 0; col < GridSystem::COLS; ++col) {
            auto& plant = m_PlantGrid[row][col];
            if (!plant) continue;

            float dx = zombie->GetX()
                       - plant->m_Transform.translation.x;
            if (dx >= -5.0f && dx <= 40.0f) {
                foundPlant = true;
                zombie->StartEat();

                if (zombie->ShouldDealEatDamage()) {
                    plant->Hurt();
                    if (plant->IsDead()) {
                        m_Root.RemoveChild(plant);
                        plant = nullptr;
                        zombie->StartWalk();
                        LOG_DEBUG("Plant eaten at row {} col {}", row, col);
                    }
                }
                break;
            }
        }

        if (!foundPlant && zombie->IsEating()) {
            zombie->StartWalk();
        }
    }
}

void App::UpdateLawnMowers() {
    for (int row = 0; row < GridSystem::ROWS; ++row) {
        auto& mower = m_LawnMowers[row];
        if (!mower) continue;

        // Trigger mower if idle zombie reaches it
        if (mower->GetState() == LawnMower::State::IDLE) {
            for (auto& zombie : m_Zombies) {
                if (!zombie->IsAlive() || zombie->GetRow() != row) continue;
                if (zombie->GetX() <= mower->m_Transform.translation.x + 30.0f) {
                    mower->Trigger();
                    LOG_DEBUG("LawnMower triggered in row {}", row);
                    break;
                }
            }
        }

        mower->Update();

        // Kill zombies in path
        if (mower->GetState() == LawnMower::State::MOVING) {
            for (auto& zombie : m_Zombies) {
                if (!zombie->IsAlive() || zombie->GetRow() != row) continue;
                if (mower->HitCheck(zombie->m_Transform.translation)) {
                    zombie->TakeDamage(9999);
                }
            }
        }

        // Remove off-screen mower
        if (mower->IsOffScreen()) {
            m_Root.RemoveChild(mower);
            mower = nullptr;
        }
    }
}
