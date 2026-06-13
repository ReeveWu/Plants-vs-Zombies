#ifndef ZOMBIE_SPAWNER_HPP
#define ZOMBIE_SPAWNER_HPP

#include "config/LevelConfig.hpp"
#include "Util/Renderer.hpp"
#include "factory/ZombieFactory.hpp"

#include <memory>
#include <vector>

class Zombie;

struct ZombieSpawnUpdateResult {
    int spawnedCount = 0;
    bool shouldAnnounce = false;
};

class ZombieSpawner {
public:
    void Reset(const LevelConfig& level);
    ZombieSpawnUpdateResult Update(
        const std::vector<int>& activeLanes, float cameraOffset,
        std::vector<std::shared_ptr<Zombie>>& zombies,
        Util::Renderer& root);

    int GetTotalZombies() const { return m_TotalToSpawn; }
    int GetSpawnedCount() const { return m_Spawned; }
    void CompleteAll();
    int GetRegularRemaining() const {
        return m_NormalRemaining + m_ConeheadRemaining + m_BucketRemaining;
    }
    int GetFlagRemaining() const { return m_FlagRemaining; }

    ZombieType TakeRegularZombieByRoll(int roll);
    bool TakeFlagZombie();

private:
    bool SpawnZombie(ZombieType type, const std::vector<int>& activeLanes,
                     float cameraOffset,
                     std::vector<std::shared_ptr<Zombie>>& zombies,
                     Util::Renderer& root, int randomOffset);
    bool SpawnRegularZombie(const std::vector<int>& activeLanes,
                            float cameraOffset,
                            std::vector<std::shared_ptr<Zombie>>& zombies,
                            Util::Renderer& root);

    int m_TotalToSpawn = 0;
    int m_SpawnInterval = 600;
    int m_InitialDelayTimer = 0;
    int m_NormalRemaining = 0;
    int m_ConeheadRemaining = 0;
    int m_BucketRemaining = 0;
    int m_FlagRemaining = 0;
    int m_WaveIndex = 0;
    int m_ZombieSpawnTimer = 0;
    int m_Spawned = 0;
    std::vector<WaveEvent> m_Waves;
};

#endif // ZOMBIE_SPAWNER_HPP
