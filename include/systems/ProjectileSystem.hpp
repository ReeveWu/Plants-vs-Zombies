#ifndef PROJECTILE_SYSTEM_HPP
#define PROJECTILE_SYSTEM_HPP

#include "entities/Bullet.hpp"
#include "Util/Renderer.hpp"

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

class Zombie;

class ProjectileSystem {
public:
    void SpawnBullet(const glm::vec2& position, int row, int damage,
                     bool ice, Util::Renderer& root);
    void Update(std::vector<std::shared_ptr<Zombie>>& zombies,
                Util::Renderer& root);
    void Clear(Util::Renderer& root);

private:
    std::vector<std::shared_ptr<Bullet>> m_Bullets;
};

#endif // PROJECTILE_SYSTEM_HPP
