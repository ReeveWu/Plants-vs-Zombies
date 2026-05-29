#ifndef WALLNUT_HPP
#define WALLNUT_HPP

#include "entities/Plant.hpp"

class Wallnut : public Plant {
public:
    static constexpr int WALLNUT_HP = 7200;

    Wallnut(int row, int col)
        : Plant(MakeWallnutFrames(0), 4, row, col, WALLNUT_HP, 100) {}

protected:
    void OnHurt() override {
        int newState = HPToState();
        if (newState != m_DamageState) {
            m_DamageState = newState;
            SwitchAnimation(MakeWallnutFrames(m_DamageState));
        }
    }

private:
    struct DamageThreshold {
        int minHP;   // HP >= minHP means this state
        int state;
        int frames;
    };

    static constexpr DamageThreshold THRESHOLDS[] = {
        {4800, 0, 32},  // full HP
        {2400, 1, 11},  // medium damage
        {0,    2, 32},  // severe damage
    };

    int HPToState() const {
        for (const auto& t : THRESHOLDS) {
            if (m_HP >= t.minHP) return t.state;
        }
        return 2;
    }

    static std::vector<std::string> MakeWallnutFrames(int state) {
        int count = 32;
        for (const auto& t : THRESHOLDS) {
            if (t.state == state) { count = t.frames; break; }
        }
        std::vector<std::string> paths;
        paths.reserve(count);
        for (int i = 0; i < count; ++i) {
            paths.push_back(
                std::string(RESOURCE_DIR "/Plant/wallnut/idle/")
                + std::to_string(state) + "/"
                + std::to_string(i) + ".png");
        }
        return paths;
    }

    int m_DamageState = 0;
};

#endif // WALLNUT_HPP
