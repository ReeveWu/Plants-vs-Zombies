#ifndef LAWN_MOWER_SYSTEM_HPP
#define LAWN_MOWER_SYSTEM_HPP

#include "core/GridSystem.hpp"
#include "entities/LawnMower.hpp"
#include "Util/Renderer.hpp"

#include <array>
#include <memory>
#include <vector>

class Zombie;

class LawnMowerSystem {
public:
    void Init(const std::vector<int>& activeLanes, float cameraOffset,
              Util::Renderer& root);
    bool Update(std::vector<std::shared_ptr<Zombie>>& zombies,
                Util::Renderer& root);
    void SyncCamera(float cameraOffset);
    void Clear(Util::Renderer& root);
    bool HasMower(int row) const;

private:
    std::array<std::shared_ptr<LawnMower>, GridSystem::ROWS> m_Mowers{};

    static constexpr float LAWNMOWER_X = -560.0f;
    static constexpr int HIT_DAMAGE = 9999;
};

#endif // LAWN_MOWER_SYSTEM_HPP
