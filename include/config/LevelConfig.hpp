#ifndef LEVEL_CONFIG_HPP
#define LEVEL_CONFIG_HPP

#include <array>
#include <vector>

struct WaveEvent {
    float progressThreshold;
    int count;
};

struct LevelConfig {
    int lanes;
    std::vector<int> availablePlants;
    int normalCount;
    int coneheadCount;
    int bucketCount;
    int flagCount;
    int spawnIntervalFrames;
    int initialDelayFrames;
    std::vector<WaveEvent> waves;
    int rewardPlant; // plant type unlocked after this level (0 = none)

    int TotalZombies() const {
        return normalCount + coneheadCount + bucketCount + flagCount;
    }
};

inline const std::array<LevelConfig, 15>& GetAllLevels() {
    static const std::array<LevelConfig, 15> levels = {{
        // Level  1: 1 lane, peashooter only
        {1, {1}, 5, 0, 0, 0, 900, 0, {}, 2},
        // Level  2: 3 lanes, +sunflower, 8s delay
        {3, {1, 2}, 10, 0, 0, 0, 720, 480, {}, 3},
        // Level  3: 3 lanes, +cherrybomb
        {3, {1, 2, 3}, 12, 2, 0, 0, 660, 0, {}, 4},
        // Level  4: 5 lanes, +wallnut, 1 wave
        {5, {1, 2, 3, 4}, 15, 4, 0, 1, 600, 0, {{0.75f, 7}}, 9},
        // Level  5: 5 lanes, +shovel
        {5, {1, 2, 3, 4}, 15, 5, 1, 0, 600, 0, {}, 5},
        // Level  6: 5 lanes, +mine
        {5, {1, 2, 3, 4, 5}, 15, 8, 2, 0, 540, 0, {}, 6},
        // Level  7: 5 lanes, +iceshooter, 2 waves
        {5, {1, 2, 3, 4, 5, 6}, 20, 10, 3, 2, 540, 0,
         {{0.50f, 8}, {0.85f, 8}}, 7},
        // Level  8: +chomper, all plants, fast spawn
        {5, {1, 2, 3, 4, 5, 6, 7}, 20, 10, 5, 0, 480, 0, {}, 8},
        // Level  9: 2 waves
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 25, 15, 6, 2, 480, 0,
         {{0.55f, 10}, {0.85f, 10}}, 0},
        // Level 10: 3 waves
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 30, 15, 10, 3, 450, 0,
         {{0.30f, 10}, {0.60f, 10}, {0.90f, 10}}, 0},
        // Level 11: defense check
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 10, 20, 5, 0, 480, 0, {}, 0},
        // Level 12: heavy armor, 1 massive wave
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 15, 10, 15, 1, 720, 0,
         {{0.85f, 15}}, 0},
        // Level 13: quantity pressure, 2 waves
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 40, 10, 5, 2, 300, 0,
         {{0.45f, 12}, {0.85f, 12}}, 0},
        // Level 14: mixed assault, 2 waves
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 30, 20, 15, 2, 420, 0,
         {{0.45f, 12}, {0.85f, 12}}, 0},
        // Level 15: final stand, 4 waves
        {5, {1, 2, 3, 4, 5, 6, 7, 8}, 50, 30, 20, 4, 360, 0,
         {{0.20f, 14}, {0.45f, 14}, {0.70f, 14}, {0.90f, 14}}, 0},
    }};
    return levels;
}

inline std::vector<int> GetActiveLanes(int lanes) {
    switch (lanes) {
    case 1:
        return {2};
    case 3:
        return {1, 2, 3};
    default:
        return {0, 1, 2, 3, 4};
    }
}

#endif // LEVEL_CONFIG_HPP
