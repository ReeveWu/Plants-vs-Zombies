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

                    m_HoldingPlant = CreatePlant(
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
    UpdatePlantSunProduction();
    UpdateBullets();
    UpdateZombieEating();
    UpdateZombies();
    UpdateLawnMowers();
    UpdateDeathAnims();
    CheckWinLose();
}

void App::PlacePlant(int row, int col) {
    glm::vec2 screenPos = GridSystem::CellToPosition(row, col)
                          + glm::vec2{m_CameraOffset, 0.0f};

    auto plant = CreatePlant(m_HoldingCardIndex, row, col);
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

    // Weighted random: Normal 40%, Flag 20%, Conehead 25%, Bucket 15%
    std::shared_ptr<Zombie> zombie;
    int roll = rand() % 100;
    if (roll < 40) {
        zombie = std::make_shared<NormalZombie>(row);
    } else if (roll < 60) {
        zombie = std::make_shared<FlagZombie>(row);
    } else if (roll < 85) {
        zombie = std::make_shared<ConeheadZombie>(row);
    } else {
        zombie = std::make_shared<BucketZombie>(row);
    }

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
                if (z->IsDead()) {
                    SpawnDeathAnims(z);
                    m_Root.RemoveChild(z);
                    return true;
                }
                if (z->IsPastLine(ZOMBIE_LOSE_X)) {
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
                auto bullet = std::make_shared<Bullet>(
                    bulletPos, plant->GetDamage(), plant->IsIce());
                bullet->SetRow(row);
                m_Bullets.push_back(bullet);
                m_Root.AddChild(bullet);
            }
        }
    }
}

void App::UpdatePlantSunProduction() {
    for (int row = 0; row < GridSystem::ROWS; ++row) {
        for (int col = 0; col < GridSystem::COLS; ++col) {
            auto& plant = m_PlantGrid[row][col];
            if (!plant) continue;

            if (plant->TryProduceSun()) {
                glm::vec2 plantPos = plant->m_Transform.translation;
                auto sun = std::make_shared<Sun>(plantPos, true);
                m_Suns.push_back(sun);
                m_Root.AddChild(sun);
                LOG_DEBUG("Sunflower at ({}, {}) produced a sun", row, col);
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
                    plant->Hurt(zombie->GetEatDamage());
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

void App::CheckWinLose() {
    // Lose: any zombie past the line with no lawnmower left
    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) continue;
        int row = zombie->GetRow();
        bool hasMower = m_LawnMowers[row] != nullptr;
        if (!hasMower && zombie->IsPastLine(ZOMBIE_LOSE_X+m_CameraOffset)) {
            LOG_DEBUG("Game Over! Zombie reached the house in row {}", row);
            m_EndScreen = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(
                    RESOURCE_DIR "/LevelCompleted/lose/0.png"),
                100.0f);
            m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
            m_Root.AddChild(m_EndScreen);
            m_EndTimer = 0;
            m_Phase = Phase::GAME_OVER;
            return;
        }
    }

    // Win: all zombies spawned and none alive
    if (m_ZombiesSpawned >= ZOMBIES_PER_LEVEL && m_Zombies.empty()) {
        LOG_DEBUG("Level Complete! All zombies defeated.");
        m_EndScreen = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(
                RESOURCE_DIR "/LevelCompleted/lose/0.png"),
            100.0f);
        m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
        m_Root.AddChild(m_EndScreen);
        m_EndTimer = 0;
        m_Phase = Phase::LEVEL_COMPLETE;
    }
}

void App::SpawnDeathAnims(const std::shared_ptr<Zombie>& zombie) {
    glm::vec2 pos = zombie->m_Transform.translation;

    auto body = std::make_shared<ZombieCorpse>(
        zombie->GetDieBodyFrames(), pos);
    body->SetZIndex(14);
    m_DeathAnims.push_back(body);
    m_Root.AddChild(body);

    auto head = std::make_shared<ZombieCorpse>(
        zombie->GetDieHeadFrames(),
        glm::vec2{pos.x + 20.0f, pos.y + 40.0f});
    head->SetZIndex(16);
    m_DeathAnims.push_back(head);
    m_Root.AddChild(head);
}

void App::UpdateDeathAnims() {
    m_DeathAnims.erase(
        std::remove_if(m_DeathAnims.begin(), m_DeathAnims.end(),
            [&](const std::shared_ptr<ZombieCorpse>& anim) {
                if (anim->IsFinished()) {
                    m_Root.RemoveChild(anim);
                    return true;
                }
                return false;
            }),
        m_DeathAnims.end());
}

std::shared_ptr<Plant> App::CreatePlant(int typeIndex, int row, int col) {
    switch (typeIndex) {
        case 1: return std::make_shared<Peashooter>(row, col);
        case 2: return std::make_shared<Sunflower>(row, col);
        case 4: return std::make_shared<Wallnut>(row, col);
        case 6: return std::make_shared<IceShooter>(row, col);
        case 8: return std::make_shared<FastShooter>(row, col);
        default: return std::make_shared<Plant>(typeIndex, row, col);
    }
}
