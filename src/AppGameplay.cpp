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

    // Handle mouse click
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        auto click = Util::Input::GetCursorPosition();

        for (auto& sun : m_Suns) {
            if (sun->TryCollect(click)) break;
        }

        for (auto& card : m_Cards) {
            if (card->ContainsPoint(click)) {
                if (card->IsAvailable(m_SunAmount)) {
                    m_SelectedCard = (m_SelectedCard == card) ? nullptr : card;
                    LOG_DEBUG("Card {} selected (cost: {})",
                              card->GetIndex(), card->GetCost());
                } else {
                    LOG_DEBUG("Card {} not available (cost: {}, sun: {})",
                              card->GetIndex(), card->GetCost(), m_SunAmount);
                }
                break;
            }
        }
    }

    // Deselect with right click
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB)) {
        m_SelectedCard = nullptr;
    }
}

void App::SpawnSkySun() {
    float x = static_cast<float>(rand() % 650 - 200);
    float y = static_cast<float>(rand() % 250 - 100);

    auto sun = std::make_shared<Sun>(glm::vec2{x, y});
    m_Suns.push_back(sun);
    m_Root.AddChild(sun);
    LOG_DEBUG("Sky sun spawned at target ({}, {})", x, y);
}
