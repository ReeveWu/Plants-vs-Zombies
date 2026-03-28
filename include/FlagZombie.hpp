#ifndef FLAG_ZOMBIE_HPP
#define FLAG_ZOMBIE_HPP

#include "Zombie.hpp"

class FlagZombie : public Zombie {
public:
    explicit FlagZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 300;
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/flag/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/flag/die/head/", 12);
    }

private:
    static std::vector<ArmorPhase> BuildPhases() {
        return {{
            0,
            MakeFrames("/Zombie/flag/walk/normal/", 11),
            MakeFrames("/Zombie/flag/eat/normal/", 11),
            MakeFrames("/Zombie/flag/walk/hit/", 11),
            MakeFrames("/Zombie/flag/eat/hit/", 11),
        }};
    }
};

#endif // FLAG_ZOMBIE_HPP
