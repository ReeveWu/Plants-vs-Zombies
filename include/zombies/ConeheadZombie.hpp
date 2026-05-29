#ifndef CONEHEAD_ZOMBIE_HPP
#define CONEHEAD_ZOMBIE_HPP

#include "entities/Zombie.hpp"

class ConeheadZombie : public Zombie {
public:
    explicit ConeheadZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 560; 
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/cone/idle/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/cone/idle/die/head/", 12);
    }

    std::vector<std::string> GetAshFrames() const override {
        return MakeFrames("/Zombie/cone/idle/ash/", 20);
    }

private:
    // 4 armor phases: full cone → damaged → cracked → no cone
    // Cone armor = 370, spread across first 3 phases
    static std::vector<ArmorPhase> BuildPhases() {
        return {
            { 420, // Phase 0: full cone (HP > 420)
              {{"idle", {
                  MakeFrames("/Zombie/cone/idle/walk_0/normal/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_0/normal/", 39),
                  MakeFrames("/Zombie/cone/idle/walk_0/hit/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_0/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/cone/frozen/walk_0/normal/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_0/normal/", 39),
                  MakeFrames("/Zombie/cone/frozen/walk_0/hit/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_0/hit/", 39),
              }}} },
            { 280, // Phase 1: damaged cone (HP > 280)
              {{"idle", {
                  MakeFrames("/Zombie/cone/idle/walk_1/normal/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_1/normal/", 39),
                  MakeFrames("/Zombie/cone/idle/walk_1/hit/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_1/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/cone/frozen/walk_1/normal/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_1/normal/", 39),
                  MakeFrames("/Zombie/cone/frozen/walk_1/hit/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_1/hit/", 39),
              }}} },
            { 190, // Phase 2: cracked cone (HP > 190)
              {{"idle", {
                  MakeFrames("/Zombie/cone/idle/walk_2/normal/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_2/normal/", 39),
                  MakeFrames("/Zombie/cone/idle/walk_2/hit/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_2/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/cone/frozen/walk_2/normal/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_2/normal/", 39),
                  MakeFrames("/Zombie/cone/frozen/walk_2/hit/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_2/hit/", 39),
              }}} },
            { 0,   // Phase 3: no cone (HP > 0)
              {{"idle", {
                  MakeFrames("/Zombie/cone/idle/walk_3/normal/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_3/normal/", 39),
                  MakeFrames("/Zombie/cone/idle/walk_3/hit/", 46),
                  MakeFrames("/Zombie/cone/idle/eat_3/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/cone/frozen/walk_3/normal/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_3/normal/", 39),
                  MakeFrames("/Zombie/cone/frozen/walk_3/hit/", 46),
                  MakeFrames("/Zombie/cone/frozen/eat_3/hit/", 39),
              }}} },
        };
    }
};

#endif // CONEHEAD_ZOMBIE_HPP
