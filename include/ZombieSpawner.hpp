#ifndef ZOMBIE_SPAWNER_HPP
#define ZOMBIE_SPAWNER_HPP

#include "LevelConfig.hpp"
#include "Util/Renderer.hpp"
#include "ZombieFactory.hpp"

#include <memory>
#include <vector>

class Zombie;

class ZombieSpawner {
public:
    void Reset(const LevelConfig& level);
    void Update(const std::vector<int>& activeLanes, float cameraOffset,
                std::vector<std::shared_ptr<Zombie>>& zombies,
                Util::Renderer& root);

    int GetTotalZombies() const { return m_TotalToSpawn; }
    int GetSpawnedCount() const { return m_Spawned; }
    int GetRegularRemaining() const {
        return m_NormalRemaining + m_ConeheadRemaining + m_BucketRemaining;
    }
    int GetFlagRemaining() const { return m_FlagRemaining; }

    ZombieType TakeRegularZombieByRoll(int roll);
    bool TakeFlagZombie();

private:
    void SpawnZombie(ZombieType type, const std::vector<int>& activeLanes,
                     float cameraOffset,
                     std::vector<std::shared_ptr<Zombie>>& zombies,
                     Util::Renderer& root, int randomOffset);
    void SpawnRegularZombie(const std::vector<int>& activeLanes,
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
