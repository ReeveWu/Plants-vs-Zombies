#ifndef FASTSHOOTER_HPP
#define FASTSHOOTER_HPP

#include "Plant.hpp"

class FastShooter : public Plant {
public:
    FastShooter(int row, int col)
        : Plant(MakeFrames("fastshooter", 15), 8, row, col, 300, 100) {}

    bool CanShoot() const override { return true; }
    int GetDamage() const override { return 20; }

    bool TryShoot() override {
        if (!CanShoot()) return false;
        ++m_ShootTimer;
        // First bullet at cooldown 90, second at 110 (burst of 2)
        if (m_BurstCount == 0 && m_ShootTimer >= FIRST_SHOT_CD) {
            ++m_BurstCount;
            return true;
        }
        if (m_BurstCount == 1 && m_ShootTimer >= SECOND_SHOT_CD) {
            m_BurstCount = 0;
            m_ShootTimer = 0;
            return true;
        }
        return false;
    }

private:
    int m_BurstCount = 0;
    static constexpr int FIRST_SHOT_CD = 90;
    static constexpr int SECOND_SHOT_CD = 110;
};

#endif // FASTSHOOTER_HPP
