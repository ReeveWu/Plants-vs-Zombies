#include "SunSystem.hpp"

#include "Util/Logger.hpp"

#include <algorithm>
#include <cstdlib>

void SunSystem::Reset(int initialAmount, std::shared_ptr<SunCounter> counter) {
    m_Suns.clear();
    m_Counter = std::move(counter);
    m_Amount = initialAmount;
    m_SpawnTimer = 0;
    SyncCounter();
}

void SunSystem::Update(Util::Renderer& root) {
    ++m_SpawnTimer;
    if (m_SpawnTimer >= SKY_SPAWN_INTERVAL) {
        SpawnSkySun(root);
        m_SpawnTimer = 0;
    }

    for (auto& sun : m_Suns) {
        sun->Update();
    }

    m_Suns.erase(
        std::remove_if(m_Suns.begin(), m_Suns.end(),
            [&](const std::shared_ptr<Sun>& sun) {
                if (sun->GetState() == Sun::State::DONE) {
                    root.RemoveChild(sun);
                    m_Amount += Sun::VALUE;
                    SyncCounter();
                    LOG_DEBUG("Sun collected! Total: {}", m_Amount);
                    return true;
                }
                return false;
            }),
        m_Suns.end());
}

void SunSystem::TryCollect(const glm::vec2& click) {
    for (auto& sun : m_Suns) {
        if (sun->TryCollect(click)) break;
    }
}

void SunSystem::SpawnPlantSun(const glm::vec2& plantPosition,
                              Util::Renderer& root) {
    auto sun = std::make_shared<Sun>(plantPosition, true);
    m_Suns.push_back(sun);
    root.AddChild(sun);
}

void SunSystem::Clear(Util::Renderer& root) {
    for (auto& sun : m_Suns) {
        root.RemoveChild(sun);
    }
    m_Suns.clear();
    m_Counter = nullptr;
    m_SpawnTimer = 0;
}

void SunSystem::Spend(int amount) {
    m_Amount -= amount;
    SyncCounter();
}

void SunSystem::SpawnSkySun(Util::Renderer& root) {
    float x = static_cast<float>(rand() % 650 - 200);
    float y = static_cast<float>(rand() % 250 - 100);

    auto sun = std::make_shared<Sun>(glm::vec2{x, y});
    m_Suns.push_back(sun);
    root.AddChild(sun);
    LOG_DEBUG("Sky sun spawned at target ({}, {})", x, y);
}

void SunSystem::SyncCounter() {
    if (m_Counter) {
        m_Counter->SetAmount(m_Amount);
    }
}
