#include "ZombieSpawner.hpp"

#include "GridSystem.hpp"
#include "Zombie.hpp"
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

void ZombieSpawner::Update(const std::vector<int>& activeLanes,
                           float cameraOffset,
                           std::vector<std::shared_ptr<Zombie>>& zombies,
                           Util::Renderer& root) {
    if (m_InitialDelayTimer > 0) {
        --m_InitialDelayTimer;
        return;
    }

    if (m_WaveIndex < static_cast<int>(m_Waves.size()) &&
        m_TotalToSpawn > 0) {
        float progress = static_cast<float>(m_Spawned) / m_TotalToSpawn;
        if (progress >= m_Waves[m_WaveIndex].progressThreshold) {
            int waveCount = m_Waves[m_WaveIndex].count;
            if (TakeFlagZombie()) {
                SpawnZombie(ZombieType::Flag, activeLanes, cameraOffset,
                            zombies, root, 100);
                --waveCount;
            }

            for (int i = 0; i < waveCount; ++i) {
                if (GetRegularRemaining() <= 0) break;
                SpawnRegularZombie(activeLanes, cameraOffset, zombies, root);
            }
            ++m_WaveIndex;
        }
    }

    ++m_ZombieSpawnTimer;
    if (m_ZombieSpawnTimer >= m_SpawnInterval &&
        GetRegularRemaining() > 0) {
        SpawnRegularZombie(activeLanes, cameraOffset, zombies, root);
        m_ZombieSpawnTimer = 0;
    }
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

void ZombieSpawner::SpawnZombie(
    ZombieType type, const std::vector<int>& activeLanes, float cameraOffset,
    std::vector<std::shared_ptr<Zombie>>& zombies, Util::Renderer& root,
    int randomOffset) {
    if (activeLanes.empty()) return;

    int row = activeLanes[rand() % activeLanes.size()];
    float yPos = GridSystem::CellToPosition(row, GridSystem::COLS - 1).y;
    auto zombie = ZombieFactory::Create(type, row);
    zombie->SetZIndex(15);
    zombie->m_Transform.translation = {
        620.0f + cameraOffset + static_cast<float>(rand() % randomOffset),
        yPos};
    zombies.push_back(zombie);
    root.AddChild(zombie);
    ++m_Spawned;
    LOG_DEBUG("Zombie spawned in row {} (total: {}/{})", row, m_Spawned,
              m_TotalToSpawn);
}

void ZombieSpawner::SpawnRegularZombie(
    const std::vector<int>& activeLanes, float cameraOffset,
    std::vector<std::shared_ptr<Zombie>>& zombies, Util::Renderer& root) {
    int remaining = GetRegularRemaining();
    if (remaining <= 0) return;
    int roll = rand() % remaining;
    ZombieType type = TakeRegularZombieByRoll(roll);
    SpawnZombie(type, activeLanes, cameraOffset, zombies, root, 60);
}
