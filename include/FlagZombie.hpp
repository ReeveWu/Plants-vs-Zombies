#ifndef FLAG_ZOMBIE_HPP
#define FLAG_ZOMBIE_HPP

#include "Zombie.hpp"

class FlagZombie : public Zombie {
public:
    explicit FlagZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE, 140) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 300;
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/flag/idle/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/flag/idle/die/head/", 12);
    }

    std::vector<std::string> GetAshFrames() const override {
        return MakeFrames("/Zombie/flag/idle/ash/", 20);
    }

private:
    static std::vector<ArmorPhase> BuildPhases() {
        return {{
            0,
            {{"idle", {
                MakeFrames("/Zombie/flag/idle/walk/normal/", 12),
                MakeFrames("/Zombie/flag/idle/eat/normal/", 11),
                MakeFrames("/Zombie/flag/idle/walk/hit/", 12),
                MakeFrames("/Zombie/flag/idle/eat/hit/", 11),
            }},
            {"frozen", {
                MakeFrames("/Zombie/flag/frozen/walk/normal/", 12),
                MakeFrames("/Zombie/flag/frozen/eat/normal/", 11),
                MakeFrames("/Zombie/flag/frozen/walk/hit/", 12),
                MakeFrames("/Zombie/flag/frozen/eat/hit/", 11),
            }}},
        }};
    }
};

#endif // FLAG_ZOMBIE_HPP
