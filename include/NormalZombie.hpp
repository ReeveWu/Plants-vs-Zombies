#ifndef NORMAL_ZOMBIE_HPP
#define NORMAL_ZOMBIE_HPP

#include "Zombie.hpp"

#include <string>
#include <vector>

class NormalZombie : public Zombie {
public:
    explicit NormalZombie(int row)
        : Zombie(GetWalkFrames(), row, SPEED, HP) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 8;

private:
    static std::vector<std::string> GetWalkFrames() {
        constexpr int FRAME_COUNT = 46;
        std::vector<std::string> paths;
        paths.reserve(FRAME_COUNT);
        for (int i = 0; i < FRAME_COUNT; ++i) {
            paths.push_back(RESOURCE_DIR "/Zombie/normal/walk/"
                            + std::to_string(i) + ".png");
        }
        return paths;
    }
};

#endif // NORMAL_ZOMBIE_HPP
