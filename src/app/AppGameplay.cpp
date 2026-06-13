#include "app/App.hpp"

#include "ui/CardSlot.hpp"
#include "entities/Plant.hpp"
#include "factory/PlantFactory.hpp"
#include "Util/Color.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "config.hpp"
#include "entities/Zombie.hpp"

#include <algorithm>

void App::UpdateGameplay() {
    m_SunSystem.Update(m_Root);

    for (auto& card : m_Cards) {
        card->UpdateState(m_SunSystem.GetAmount());
    }

    if (m_HoldingPlant) {
        auto cursor = Util::Input::GetCursorPosition();
        m_HoldingPlant->m_Transform.translation = cursor;
    }

    if (m_IsHoldingShovel && m_ShovelIcon) {
        auto cursor = Util::Input::GetCursorPosition();
        m_ShovelIcon->m_Transform.translation = cursor;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        auto click = Util::Input::GetCursorPosition();

        if (m_HoldingPlant) {
            glm::vec2 worldPos = click - glm::vec2{m_CameraOffset, 0.0f};
            auto [row, col] = GridSystem::PositionToCell(worldPos);

            if (m_PlantGrid.CanPlace(row, col, m_ActiveLanes)) {
                PlacePlant(row, col);
            }
        } else if (m_IsHoldingShovel) {
            glm::vec2 worldPos = click - glm::vec2{m_CameraOffset, 0.0f};
            auto [row, col] = GridSystem::PositionToCell(worldPos);
            auto plant = m_PlantGrid.Remove(row, col);

            if (plant) {
                m_Root.RemoveChild(plant);
                LOG_DEBUG("Dug up plant at ({}, {})", row, col);

                m_IsHoldingShovel = false;
                if (m_ShovelIcon) {
                    m_ShovelIcon->m_Transform.translation = {39.0f, 315.6f};
                }
            }
        } else {
            if (m_SunSystem.TryCollect(click)) {
                m_Audio.PlaySunCollect();
            }

            if (m_ShovelIcon && !m_IsHoldingShovel) {
                glm::vec2 shovelPos = {39.0f, 315.6f};
                float dist = glm::length(click - shovelPos);
                if (dist < 45.0f) {
                    m_IsHoldingShovel = true;
                    m_Audio.PlayGameClick();
                    LOG_DEBUG("Picked up shovel");
                }
            }

            if (!m_IsHoldingShovel) {
                for (auto& card : m_Cards) {
                    if (card->ContainsPoint(click) &&
                        card->IsAvailable(m_SunSystem.GetAmount())) {
                        m_SelectedCard = card;
                        m_HoldingCardIndex = card->GetIndex();

                        m_HoldingPlant =
                            PlantFactory::Create(card->GetType(), -1, -1);
                        m_HoldingPlant->SetZIndex(90);
                        m_HoldingPlant->m_Transform.translation = click;
                        m_Root.AddChild(m_HoldingPlant);
                        m_Audio.PlayGameClick();

                        LOG_DEBUG("Picked up plant {} (cost: {})",
                                  card->GetIndex(), card->GetCost());
                        break;
                    }
                }
            }
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
        if (m_HoldingPlant) {
            CancelHolding();
        } else if (m_IsHoldingShovel) {
            m_IsHoldingShovel = false;
            if (m_ShovelIcon) {
                m_ShovelIcon->m_Transform.translation = {39.0f, 309.6f};
            }
        }
    }

    const auto spawnResult = m_ZombieSpawner.Update(
        m_ActiveLanes, m_CameraOffset, m_Zombies, m_Root);
    if (spawnResult.shouldAnnounce) {
        m_Audio.PlayZombiesAreComing();
    }
    UpdatePlantBehaviors();
    const auto hitPlantTypes = m_ProjectileSystem.Update(m_Zombies, m_Root);
    for (int sourcePlantType : hitPlantTypes) {
        m_Audio.PlayZombieHit(sourcePlantType);
    }
    UpdateZombieEating();
    UpdateZombies();
    if (m_LawnMowerSystem.Update(m_Zombies, m_Root)) {
        m_Audio.PlayLawnMower();
    }
    UpdateDeathAnims();
    CheckWinLose();
}

void App::PlacePlant(int row, int col) {
    glm::vec2 screenPos = GridSystem::CellToPosition(row, col)
                          + glm::vec2{m_CameraOffset, 0.0f};

    auto plant = PlantFactory::Create(ToPlantType(m_HoldingCardIndex), row, col);
    plant->SetZIndex(10);
    plant->m_Transform.translation = screenPos;
    m_PlantGrid.Place(row, col, plant);
    m_Root.AddChild(plant);

    m_Root.RemoveChild(m_HoldingPlant);
    m_HoldingPlant = nullptr;

    m_SunSystem.Spend(m_SelectedCard->GetCost());
    m_SelectedCard->Use();
    m_Audio.PlayPlacePlant();

    LOG_DEBUG("Placed plant {} at ({}, {}), sun remaining: {}",
              m_HoldingCardIndex, row, col, m_SunSystem.GetAmount());

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

void App::UpdatePlantBehaviors() {
    constexpr float halfWindowWidth = static_cast<float>(WINDOW_WIDTH) / 2.0f;
    constexpr float attackLookahead = 80.0f;
    PlantUpdateContext context{
        m_Zombies,
        halfWindowWidth + attackLookahead,
    };
    std::vector<PlantAction> actions;

    m_PlantGrid.ForEachPlant(
        [&](int, int, const std::shared_ptr<Plant>& plant) {
            plant->UpdateBehavior(context, actions);
        });

    ApplyPlantActions(actions);
}

void App::ApplyPlantActions(const std::vector<PlantAction>& actions) {
    for (const auto& action : actions) {
        switch (action.type) {
        case PlantAction::Type::SpawnBullet:
            m_ProjectileSystem.SpawnBullet(action.position, action.row,
                                           action.damage, action.ice,
                                           action.sourcePlantType, m_Root);
            break;
        case PlantAction::Type::SpawnSun:
            m_SunSystem.SpawnPlantSun(action.position, m_Root);
            LOG_DEBUG("Sunflower at ({}, {}) produced a sun", action.row,
                      action.col);
            break;
        case PlantAction::Type::DamageZombie:
            if (action.zombie && action.zombie->IsAlive()) {
                if (action.ashDeath) {
                    action.zombie->MarkAshDeath();
                }
                action.zombie->TakeDamage(action.damage);
            }
            break;
        case PlantAction::Type::EatZombie:
            if (action.zombie && action.zombie->IsAlive()) {
                action.zombie->MarkEaten();
                m_Audio.PlayZombieDead();
            }
            break;
        case PlantAction::Type::RemovePlant: {
            auto plant = m_PlantGrid.Remove(action.row, action.col);
            if (plant) {
                m_Root.RemoveChild(plant);
            }
            break;
        }
        case PlantAction::Type::PlaySound:
            if (action.sound == PlantAction::Sound::CherryBoom) {
                m_Audio.PlayCherryBoom();
            } else if (action.sound == PlantAction::Sound::ZombieBoom) {
                m_Audio.PlayZombieBoom();
            }
            break;
        }
    }
}

void App::UpdateZombies() {
    for (auto& zombie : m_Zombies) {
        zombie->Update();
    }

    bool playedDeadThisFrame = false;
    m_Zombies.erase(
        std::remove_if(m_Zombies.begin(), m_Zombies.end(),
            [&](const std::shared_ptr<Zombie>& zombie) {
                if (zombie->IsDead()) {
                    if (!zombie->IsEaten()) {
                        if (!playedDeadThisFrame) {
                            m_Audio.PlayZombieDead();
                            playedDeadThisFrame = true;
                        }
                        SpawnDeathAnims(zombie);
                    }
                    m_Root.RemoveChild(zombie);
                    return true;
                }
                if (zombie->IsPastLine(ZOMBIE_LOSE_X)) {
                    m_Root.RemoveChild(zombie);
                    return true;
                }
                return false;
            }),
        m_Zombies.end());
}

void App::UpdateZombieEating() {
    bool playedChewThisFrame = false;

    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) continue;

        int row = zombie->GetRow();
        bool foundPlant = false;

        for (int col = 0; col < GridSystem::COLS; ++col) {
            auto plant = m_PlantGrid.At(row, col);
            if (!plant) continue;

            float dx = zombie->GetX() - plant->m_Transform.translation.x;
            if (dx >= -5.0f && dx <= 40.0f) {
                foundPlant = true;
                zombie->StartEat();

                if (zombie->ShouldDealEatDamage()) {
                    if (!playedChewThisFrame) {
                        m_Audio.PlayZombieChew();
                        playedChewThisFrame = true;
                    }
                    plant->Hurt(zombie->GetEatDamage());
                    if (plant->IsDead()) {
                        auto removed = m_PlantGrid.Remove(row, col);
                        if (removed) {
                            m_Root.RemoveChild(removed);
                        }
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

void App::CheckWinLose() {
    for (auto& zombie : m_Zombies) {
        if (!zombie->IsAlive()) continue;
        int row = zombie->GetRow();
        if (!m_LawnMowerSystem.HasMower(row) &&
            zombie->IsPastLine(ZOMBIE_LOSE_X + m_CameraOffset)) {
            LOG_DEBUG("Game Over! Zombie reached the house in row {}", row);
            ShowGameOverUi();
            return;
        }
    }

    if (m_ZombieSpawner.GetSpawnedCount() >=
            m_ZombieSpawner.GetTotalZombies() &&
        m_Zombies.empty()) {
        LOG_DEBUG("Level {} Complete! All zombies defeated.",
                  m_CurrentLevel + 1);

        m_EndTimer = 0;
        m_Audio.PlayWin();
        m_Phase = Phase::LEVEL_COMPLETE;
    }
}

void App::ShowGameOverUi() {
    ClearCompletionUi();
    m_Audio.PlayLose();

    m_EndScreen = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR "/LevelCompleted/lose/0.png"),
        98.0f);
    m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
    m_Root.AddChild(m_EndScreen);

    m_GameOverPrompt = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/Geo-Regular.ttf", 28,
            "CLICK ANYWHERE TO RETRY",
            Util::Color(255, 255, 255)),
        99.0f);
    m_GameOverPrompt->m_Transform.translation = {0.0f, -285.0f};
    m_Root.AddChild(m_GameOverPrompt);

    m_EndTimer = 0;
    m_Phase = Phase::GAME_OVER;
}

void App::SpawnDeathAnims(const std::shared_ptr<Zombie>& zombie) {
    glm::vec2 pos = zombie->m_Transform.translation;

    if (zombie->IsAshDeath()) {
        auto ashFrames = zombie->GetAshFrames();
        if (!ashFrames.empty()) {
            auto ash = std::make_shared<ZombieCorpse>(ashFrames, pos);
            ash->SetZIndex(14);
            m_DeathAnims.push_back(ash);
            m_Root.AddChild(ash);
        }
        return;
    }

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
