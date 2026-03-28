#ifndef NORMAL_ZOMBIE_HPP
#define NORMAL_ZOMBIE_HPP

#include "Zombie.hpp"

class NormalZombie : public Zombie {
public:
    explicit NormalZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 190;
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/normal/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/normal/die/head/", 12);
    }

private:
    static std::vector<ArmorPhase> BuildPhases() {
        return {{
            0,
            MakeFrames("/Zombie/normal/walk/normal/", 46),
            MakeFrames("/Zombie/normal/eat/normal/", 39),
            MakeFrames("/Zombie/normal/walk/hit/", 46),
            MakeFrames("/Zombie/normal/eat/hit/", 39),
        }};
    }
};

#endif // NORMAL_ZOMBIE_HPP
