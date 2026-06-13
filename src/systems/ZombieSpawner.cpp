#include "systems/ZombieSpawner.hpp"

#include "core/GridSystem.hpp"
#include "entities/Zombie.hpp"
#include "Util/Logger.hpp"

#include <cstdlib>

void ZombieSpawner::Reset(const LevelConfig& level) {
    m_TotalToSpawn = level.TotalZombies();
    m_SpawnInterval = level.spawnIntervalFrames;
    m_InitialDelayTimer = level.initialDelayFrames;
    m_NormalRemaining = level.normalCount;
    m_ConeheadRemaining = level.coneheadCount;
    m_BucketRemaining = level.bucketCount;
    m_FlagRemaining = level.flagCount;
    m_WaveIndex = 0;
    m_ZombieSpawnTimer = 0;
    m_Spawned = 0;
    m_Waves = level.waves;
}

void ZombieSpawner::CompleteAll() {
    m_NormalRemaining = 0;
    m_ConeheadRemaining = 0;
    m_BucketRemaining = 0;
    m_FlagRemaining = 0;
    m_WaveIndex = static_cast<int>(m_Waves.size());
    m_InitialDelayTimer = 0;
    m_ZombieSpawnTimer = 0;
    m_Spawned = m_TotalToSpawn;
}

ZombieSpawnUpdateResult ZombieSpawner::Update(
    const std::vector<int>& activeLanes,
    float cameraOffset,
    std::vector<std::shared_ptr<Zombie>>& zombies,
    Util::Renderer& root) {
    ZombieSpawnUpdateResult result;
    const int spawnedBeforeUpdate = m_Spawned;

    if (activeLanes.empty()) return result;

    if (m_InitialDelayTimer > 0) {
        --m_InitialDelayTimer;
        return result;
    }

    if (m_WaveIndex < static_cast<int>(m_Waves.size()) &&
        m_TotalToSpawn > 0) {
        float progress = static_cast<float>(m_Spawned) / m_TotalToSpawn;
        if (progress >= m_Waves[m_WaveIndex].progressThreshold) {
            bool waveSpawned = false;
            int waveCount = m_Waves[m_WaveIndex].count;
            if (TakeFlagZombie()) {
                waveSpawned = SpawnZombie(ZombieType::Flag, activeLanes,
                                          cameraOffset, zombies, root, 100);
                --waveCount;
            }

            for (int i = 0; i < waveCount; ++i) {
                if (GetRegularRemaining() <= 0) break;
                waveSpawned = SpawnRegularZombie(activeLanes, cameraOffset,
                                                 zombies, root) ||
                              waveSpawned;
            }
            result.shouldAnnounce = waveSpawned;
            ++m_WaveIndex;
        }
    }

    ++m_ZombieSpawnTimer;
    if (m_ZombieSpawnTimer >= m_SpawnInterval &&
        GetRegularRemaining() > 0) {
        SpawnRegularZombie(activeLanes, cameraOffset, zombies, root);
        m_ZombieSpawnTimer = 0;
    }

    result.spawnedCount = m_Spawned - spawnedBeforeUpdate;

    if (spawnedBeforeUpdate == 0 && result.spawnedCount > 0) {
        result.shouldAnnounce = true;
    }

    return result;
}

ZombieType ZombieSpawner::TakeRegularZombieByRoll(int roll) {
    if (roll < m_NormalRemaining) {
        --m_NormalRemaining;
        return ZombieType::Normal;
    }
    if (roll < m_NormalRemaining + m_ConeheadRemaining) {
        --m_ConeheadRemaining;
        return ZombieType::Conehead;
    }
    --m_BucketRemaining;
    return ZombieType::Bucket;
}

bool ZombieSpawner::TakeFlagZombie() {
    if (m_FlagRemaining <= 0) return false;
    --m_FlagRemaining;
    return true;
}

bool ZombieSpawner::SpawnZombie(
    ZombieType type, const std::vector<int>& activeLanes, float cameraOffset,
    std::vector<std::shared_ptr<Zombie>>& zombies, Util::Renderer& root,
    int randomOffset) {
    if (activeLanes.empty()) return false;

    int row = activeLanes[rand() % activeLanes.size()];
    float yPos = GridSystem::CellToPosition(row, GridSystem::COLS - 1).y;
    auto zombie = ZombieFactory::Create(type, row);

    static float zOffset = 0.0f;
    zombie->SetZIndex(15.0f + zOffset);
    zOffset += 0.001f;
    if (zOffset > 0.1f) zOffset = 0.0f;

    zombie->m_Transform.translation = {
        620.0f + cameraOffset + static_cast<float>(rand() % randomOffset),
        yPos};
    zombies.push_back(zombie);
    root.AddChild(zombie);
    ++m_Spawned;
    LOG_DEBUG("Zombie spawned in row {} (total: {}/{})", row, m_Spawned,
              m_TotalToSpawn);
    return true;
}

bool ZombieSpawner::SpawnRegularZombie(
    const std::vector<int>& activeLanes, float cameraOffset,
    std::vector<std::shared_ptr<Zombie>>& zombies, Util::Renderer& root) {
    int remaining = GetRegularRemaining();
    if (remaining <= 0) return false;
    int roll = rand() % remaining;
    ZombieType type = TakeRegularZombieByRoll(roll);
    return SpawnZombie(type, activeLanes, cameraOffset, zombies, root, 60);
}
