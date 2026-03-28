#ifndef SUNFLOWER_HPP
#define SUNFLOWER_HPP

#include "Plant.hpp"

class Sunflower : public Plant {
public:
    static constexpr int SUN_PRODUCE_INTERVAL = 350; // ~7.5s at 60fps

    Sunflower(int row, int col)
        : Plant(MakeFrames("sunflower", 24), 2, row, col, 300, 100) {}

    bool TryProduceSun() override {
        ++m_SunTimer;
        if (m_SunTimer >= SUN_PRODUCE_INTERVAL) {
            m_SunTimer = 0;
            return true;
        }
        return false;
    }

private:
    int m_SunTimer = 0;
};

#endif // SUNFLOWER_HPP
