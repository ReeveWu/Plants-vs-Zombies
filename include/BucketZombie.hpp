#ifndef BUCKET_ZOMBIE_HPP
#define BUCKET_ZOMBIE_HPP

#include "Zombie.hpp"

class BucketZombie : public Zombie {
public:
    explicit BucketZombie(int row)
        : Zombie(BuildPhases(), row, SPEED, HP, EAT_DAMAGE) {}

    static constexpr float SPEED = 0.5f;
    static constexpr int HP = 1290;      // 1100 (bucket) + 190 (base)
    static constexpr int EAT_DAMAGE = 100;

    std::vector<std::string> GetDieBodyFrames() const override {
        return MakeFrames("/Zombie/bucket/die/body/", 10);
    }

    std::vector<std::string> GetDieHeadFrames() const override {
        return MakeFrames("/Zombie/bucket/die/head/", 12);
    }

private:
    // 4 armor phases: full bucket → dented → cracked → no bucket
    // Bucket armor = 1100, spread across first 3 phases
    static std::vector<ArmorPhase> BuildPhases() {
        return {
            { 920, // Phase 0: full bucket (HP > 920)
              MakeFrames("/Zombie/bucket/walk_0/normal/", 46),
              MakeFrames("/Zombie/bucket/eat_0/normal/", 39),
              MakeFrames("/Zombie/bucket/walk_0/hit/", 46),
              MakeFrames("/Zombie/bucket/eat_0/hit/", 39) },
            { 555, // Phase 1: dented bucket (HP > 555)
              MakeFrames("/Zombie/bucket/walk_1/normal/", 46),
              MakeFrames("/Zombie/bucket/eat_1/normal/", 39),
              MakeFrames("/Zombie/bucket/walk_1/hit/", 46),
              MakeFrames("/Zombie/bucket/eat_1/hit/", 39) },
            { 190, // Phase 2: cracked bucket (HP > 190)
              MakeFrames("/Zombie/bucket/walk_2/normal/", 46),
              MakeFrames("/Zombie/bucket/eat_2/normal/", 39),
              MakeFrames("/Zombie/bucket/walk_2/hit/", 46),
              MakeFrames("/Zombie/bucket/eat_2/hit/", 39) },
            { 0,   // Phase 3: no bucket (HP > 0)
              MakeFrames("/Zombie/bucket/walk_3/normal/", 46),
              MakeFrames("/Zombie/bucket/eat_3/normal/", 39),
              MakeFrames("/Zombie/bucket/walk_3/hit/", 46),
              MakeFrames("/Zombie/bucket/eat_3/hit/", 39) },
        };
    }
};

#endif // BUCKET_ZOMBIE_HPP
