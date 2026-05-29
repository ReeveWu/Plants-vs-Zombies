#include "systems/LawnMowerSystem.hpp"

#include "entities/Zombie.hpp"
#include "Util/Logger.hpp"

#include <algorithm>

void LawnMowerSystem::Init(const std::vector<int>& activeLanes,
                           float cameraOffset, Util::Renderer& root) {
    Clear(root);

    for (int row = 0; row < GridSystem::ROWS; ++row) {
        if (std::find(activeLanes.begin(), activeLanes.end(), row) ==
            activeLanes.end()) {
            continue;
        }
        float y = GridSystem::CellToPosition(row, 0).y - 20.0f;
        auto mower =
            std::make_shared<LawnMower>(glm::vec2{LAWNMOWER_X + cameraOffset,
                                                  y});
        mower->SetZIndex(5);
        m_Mowers[row] = mower;
        root.AddChild(mower);
    }
}

void LawnMowerSystem::Update(std::vector<std::shared_ptr<Zombie>>& zombies,
                             Util::Renderer& root) {
    for (int row = 0; row < GridSystem::ROWS; ++row) {
        auto& mower = m_Mowers[row];
        if (!mower) continue;

        if (mower->GetState() == LawnMower::State::IDLE) {
            for (auto& zombie : zombies) {
                if (!zombie->IsAlive() || zombie->GetRow() != row) continue;
                if (zombie->GetX() <= mower->m_Transform.translation.x + 30.0f) {
                    mower->Trigger();
                    LOG_DEBUG("LawnMower triggered in row {}", row);
                    break;
                }
            }
        }

        mower->Update();

        if (mower->GetState() == LawnMower::State::MOVING) {
            for (auto& zombie : zombies) {
                if (!zombie->IsAlive() || zombie->GetRow() != row) continue;
                if (mower->HitCheck(zombie->m_Transform.translation)) {
                    zombie->TakeDamage(HIT_DAMAGE);
                }
            }
        }

        if (mower->IsOffScreen()) {
            root.RemoveChild(mower);
            mower = nullptr;
        }
    }
}

void LawnMowerSystem::SyncCamera(float cameraOffset) {
    for (auto& mower : m_Mowers) {
        if (mower) {
            mower->m_Transform.translation.x = LAWNMOWER_X + cameraOffset;
        }
    }
}

void LawnMowerSystem::Clear(Util::Renderer& root) {
    for (auto& mower : m_Mowers) {
        if (mower) {
            root.RemoveChild(mower);
            mower = nullptr;
        }
    }
}

bool LawnMowerSystem::HasMower(int row) const {
    return row >= 0 && row < GridSystem::ROWS && m_Mowers[row] != nullptr;
}
