#ifndef NORMAL_ZOMBIE_HPP
#define NORMAL_ZOMBIE_HPP

#include "entities/Zombie.hpp"

class NormalZombie : public Zombie {
public:
    explicit NormalZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 190;
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/normal/idle/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/normal/idle/die/head/", 12);
    }

    std::vector<std::string> GetAshFrames() const override {
        return MakeFrames("/Zombie/normal/idle/ash/", 20);
    }

private:
    static std::vector<ArmorPhase> BuildPhases() {
        return {{
            0,
            {{"idle", {
                MakeFrames("/Zombie/normal/idle/walk/normal/", 46),
                MakeFrames("/Zombie/normal/idle/eat/normal/", 39),
                MakeFrames("/Zombie/normal/idle/walk/hit/", 46),
                MakeFrames("/Zombie/normal/idle/eat/hit/", 39),
            }},
            {"frozen", {
                MakeFrames("/Zombie/normal/frozen/walk/normal/", 46),
                MakeFrames("/Zombie/normal/frozen/eat/normal/", 39),
                MakeFrames("/Zombie/normal/frozen/walk/hit/", 46),
                MakeFrames("/Zombie/normal/frozen/eat/hit/", 39),
            }}},
        }};
    }
};

#endif // NORMAL_ZOMBIE_HPP
