#ifndef BUCKET_ZOMBIE_HPP
#define BUCKET_ZOMBIE_HPP

#include "entities/Zombie.hpp"

class BucketZombie : public Zombie {
public:
    explicit BucketZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 1290;      // 1100 (bucket) + 190 (base)
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/bucket/idle/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/bucket/idle/die/head/", 12);
    }

    std::vector<std::string> GetAshFrames() const override {
        return MakeFrames("/Zombie/bucket/idle/ash/", 20);
    }

private:
    static std::vector<ArmorPhase> BuildPhases() {
        return {
            { 920, // Phase 0: full bucket (HP > 920)
              {{"idle", {
                  MakeFrames("/Zombie/bucket/idle/walk_0/normal/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_0/normal/", 39),
                  MakeFrames("/Zombie/bucket/idle/walk_0/hit/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_0/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/bucket/frozen/walk_0/normal/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_0/normal/", 39),
                  MakeFrames("/Zombie/bucket/frozen/walk_0/hit/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_0/hit/", 39),
              }}} },
            { 555, // Phase 1: dented bucket (HP > 555)
              {{"idle", {
                  MakeFrames("/Zombie/bucket/idle/walk_1/normal/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_1/normal/", 39),
                  MakeFrames("/Zombie/bucket/idle/walk_1/hit/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_1/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/bucket/frozen/walk_1/normal/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_1/normal/", 39),
                  MakeFrames("/Zombie/bucket/frozen/walk_1/hit/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_1/hit/", 39),
              }}} },
            { 190, // Phase 2: cracked bucket (HP > 190)
              {{"idle", {
                  MakeFrames("/Zombie/bucket/idle/walk_2/normal/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_2/normal/", 39),
                  MakeFrames("/Zombie/bucket/idle/walk_2/hit/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_2/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/bucket/frozen/walk_2/normal/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_2/normal/", 39),
                  MakeFrames("/Zombie/bucket/frozen/walk_2/hit/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_2/hit/", 39),
              }}} },
            { 0,   // Phase 3: no bucket (HP > 0)
              {{"idle", {
                  MakeFrames("/Zombie/bucket/idle/walk_3/normal/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_3/normal/", 39),
                  MakeFrames("/Zombie/bucket/idle/walk_3/hit/", 46),
                  MakeFrames("/Zombie/bucket/idle/eat_3/hit/", 39),
              }},
              {"frozen", {
                  MakeFrames("/Zombie/bucket/frozen/walk_3/normal/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_3/normal/", 39),
                  MakeFrames("/Zombie/bucket/frozen/walk_3/hit/", 46),
                  MakeFrames("/Zombie/bucket/frozen/eat_3/hit/", 39),
              }}} },
        };
    }
};

#endif // BUCKET_ZOMBIE_HPP
