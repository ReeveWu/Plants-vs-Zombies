#include "entities/Plant.hpp"

#include "entities/Zombie.hpp"

PlantUpdateContext::PlantUpdateContext(
    const std::vector<std::shared_ptr<Zombie>>& zombies, float cameraRightX)
    : zombies(zombies), cameraRightX(cameraRightX) {}

bool PlantUpdateContext::IsVisibleToCamera(
    const std::shared_ptr<Zombie>& zombie) const {
    return zombie && zombie->GetX() <= cameraRightX;
}

bool PlantUpdateContext::IsZombieReserved(
    const std::shared_ptr<Zombie>& zombie) const {
    return zombie && m_ReservedZombies.count(zombie.get()) > 0;
}

bool PlantUpdateContext::TryReserveZombie(
    const std::shared_ptr<Zombie>& zombie) const {
    return zombie && m_ReservedZombies.insert(zombie.get()).second;
}

void Plant::UpdateBehavior(const PlantUpdateContext& context,
                           std::vector<PlantAction>& actions) {
    if (CanShoot()) {
        bool zombieAhead = false;
        for (const auto& zombie : context.zombies) {
            if (context.IsVisibleToCamera(zombie) &&
                zombie->GetRow() == m_Row && zombie->IsAlive() &&
                zombie->GetX() > m_Transform.translation.x) {
                zombieAhead = true;
                break;
            }
        }

        if (zombieAhead && TryShoot()) {
            glm::vec2 bulletPos = m_Transform.translation;
            bulletPos.x += 30.0f;
            bulletPos.y += 20.0f;
            actions.push_back(PlantAction::SpawnBullet(
                bulletPos, m_Row, m_TypeIndex, GetDamage(), IsIce()));
        }
    }

    if (TryProduceSun()) {
        actions.push_back(
            PlantAction::SpawnSun(m_Transform.translation, m_Row, m_Col));
    }
}
