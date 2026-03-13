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
