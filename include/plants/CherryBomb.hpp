#ifndef CHERRY_BOMB_HPP
#define CHERRY_BOMB_HPP

#include "entities/Plant.hpp"
#include "entities/Zombie.hpp"

#include <string>
#include <vector>

class CherryBomb : public Plant {
public:
    enum class BombState { IDLE, BOOM, DONE };

    CherryBomb(int row, int col)
        : Plant(MakeFrames("cherrybomb", 14), 3, row, col, 300, 100) {}

    BombState GetBombState() const { return m_BombState; }
    bool IsDone() const { return m_BombState == BombState::DONE; }

    void UpdateBehavior(const PlantUpdateContext& context,
                        std::vector<PlantAction>& actions) override {
        Update();

        if (ShouldApplyDamage()) {
            SetZIndex(20);
            float centerX = m_Transform.translation.x;
            float halfRange = 1.5f * GridSystem::CELL_WIDTH;
            for (const auto& zombie : context.zombies) {
                if (!context.IsVisibleToCamera(zombie)) continue;
                if (!zombie->IsAlive()) continue;
                int zombieRow = zombie->GetRow();
                if (zombieRow < GetMinRow() || zombieRow > GetMaxRow()) {
                    continue;
                }
                if (zombie->GetX() >= centerX - halfRange &&
                    zombie->GetX() <= centerX + halfRange) {
                    actions.push_back(PlantAction::DamageZombie(
                        zombie, EXPLODE_DAMAGE, true));
                }
            }
        }

        if (IsDone()) {
            actions.push_back(PlantAction::RemovePlant(m_Row, m_Col));
        }
    }

    void Update() {
        switch (m_BombState) {
        case BombState::IDLE:
            ++m_FuseTimer;
            if (m_FuseTimer >= FUSE_DELAY) {
                m_BombState = BombState::BOOM;
                m_BoomTimer = 0;
                m_DamageApplied = false;
                SwitchBoomAnimation();
            }
            break;
        case BombState::BOOM:
            ++m_BoomTimer;
            if (m_BoomTimer >= BOOM_DURATION) {
                m_BombState = BombState::DONE;
                SetVisible(false);
            }
            break;
        case BombState::DONE:
            break;
        }
    }

    // Returns true exactly once when the bomb first enters BOOM
    bool ShouldApplyDamage() {
        if (m_BombState == BombState::BOOM && !m_DamageApplied) {
            m_DamageApplied = true;
            return true;
        }
        return false;
    }

    static constexpr int EXPLODE_DAMAGE = 9999;

    // 3×3 AOE range in grid cells around (m_Row, m_Col)
    int GetMinRow() const { return (m_Row > 0) ? m_Row - 1 : 0; }
    int GetMaxRow() const {
        return (m_Row < GridSystem::ROWS - 1) ? m_Row + 1 : GridSystem::ROWS - 1;
    }
    int GetMinCol() const { return (m_Col > 0) ? m_Col - 1 : 0; }
    int GetMaxCol() const {
        return (m_Col < GridSystem::COLS - 1) ? m_Col + 1 : GridSystem::COLS - 1;
    }

private:
    void SwitchBoomAnimation() {
        auto boomFrames = MakeActionFrames("cherrybomb/boom", 13);
        auto anim = std::make_shared<Util::Animation>(
            boomFrames, false, 60, false, 0);
        m_Drawable = anim;
        Play();
    }

    BombState m_BombState = BombState::IDLE;
    int m_FuseTimer = 0;
    int m_BoomTimer = 0;
    bool m_DamageApplied = false;
    static constexpr int FUSE_DELAY = 20;
    static constexpr int BOOM_DURATION = 50;
};

#endif // CHERRY_BOMB_HPP
