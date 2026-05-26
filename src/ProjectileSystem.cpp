#include "ProjectileSystem.hpp"

#include "StatusEffect.hpp"
#include "Zombie.hpp"

#include <algorithm>
#include <memory>

void ProjectileSystem::SpawnBullet(const glm::vec2& position, int row,
                                   int damage, bool ice,
                                   Util::Renderer& root) {
    auto bullet = std::make_shared<Bullet>(position, damage, ice);
    bullet->SetRow(row);
    m_Bullets.push_back(bullet);
    root.AddChild(bullet);
}

void ProjectileSystem::Update(std::vector<std::shared_ptr<Zombie>>& zombies,
                              Util::Renderer& root) {
    for (auto& bullet : m_Bullets) {
        bullet->Update();
    }

    for (auto& bullet : m_Bullets) {
        if (bullet->IsHit()) continue;
        for (auto& zombie : zombies) {
            if (!zombie->IsAlive()) continue;
            if (zombie->GetRow() != bullet->GetRow()) continue;
            if (bullet->HitCheck(zombie->m_Transform.translation)) {
                zombie->TakeDamage(bullet->GetDamage());
                if (bullet->IsIce()) {
                    zombie->ApplyEffect(std::make_unique<FrostEffect>());
                }
                bullet->MarkHit();
                break;
            }
        }
    }

    m_Bullets.erase(
        std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [&](const std::shared_ptr<Bullet>& bullet) {
                if (bullet->IsHit() || bullet->IsOffScreen()) {
                    root.RemoveChild(bullet);
                    return true;
                }
                return false;
            }),
        m_Bullets.end());
}

void ProjectileSystem::Clear(Util::Renderer& root) {
    for (auto& bullet : m_Bullets) {
        root.RemoveChild(bullet);
    }
    m_Bullets.clear();
}
