#ifndef NORMAL_ZOMBIE_HPP
#define NORMAL_ZOMBIE_HPP

#include "Zombie.hpp"

#include <string>
#include <vector>

class NormalZombie : public Zombie {
public:
    explicit NormalZombie(int row)
        : Zombie(GetWalkFrames(), GetEatFrames(),
                 GetWalkHitFrames(), GetEatHitFrames(),
                 row, SPEED, HP) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 8;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/die/die/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/die/head/", 12);
    }

private:
    static std::vector<std::string> MakeFrames(const std::string& dir, int count) {
        std::vector<std::string> paths;
        paths.reserve(count);
        for (int i = 0; i < count; ++i) {
            paths.push_back(RESOURCE_DIR + dir + std::to_string(i) + ".png");
        }
        return paths;
    }

    static std::vector<std::string> GetWalkFrames() {
        return MakeFrames("/Zombie/normal/walk/normal/", 46);
    }

    static std::vector<std::string> GetEatFrames() {
        return MakeFrames("/Zombie/normal/eat/normal/", 39);
    }

    static std::vector<std::string> GetWalkHitFrames() {
        return MakeFrames("/Zombie/normal/walk/hit/", 46);
    }

    static std::vector<std::string> GetEatHitFrames() {
        return MakeFrames("/Zombie/normal/eat/hit/", 39);
    }
};

#endif // NORMAL_ZOMBIE_HPP
