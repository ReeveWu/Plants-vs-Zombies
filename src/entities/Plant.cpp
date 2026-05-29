#include "entities/Plant.hpp"

#include "entities/Zombie.hpp"

void Plant::UpdateBehavior(const PlantUpdateContext& context,
                           std::vector<PlantAction>& actions) {
    if (CanShoot()) {
        bool zombieAhead = false;
        for (const auto& zombie : context.zombies) {
            if (zombie->GetRow() == m_Row && zombie->IsAlive() &&
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
                bulletPos, m_Row, GetDamage(), IsIce()));
        }
    }

    if (TryProduceSun()) {
        actions.push_back(
            PlantAction::SpawnSun(m_Transform.translation, m_Row, m_Col));
    }
}
