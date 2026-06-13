#ifndef PLANT_ACTION_HPP
#define PLANT_ACTION_HPP

#include <glm/vec2.hpp>

#include <memory>
#include <unordered_set>
#include <vector>

class Zombie;

struct PlantUpdateContext {
    PlantUpdateContext(const std::vector<std::shared_ptr<Zombie>>& zombies,
                       float cameraRightX);

    const std::vector<std::shared_ptr<Zombie>>& zombies;
    float cameraRightX;

    bool IsVisibleToCamera(const std::shared_ptr<Zombie>& zombie) const;
    bool IsZombieReserved(const std::shared_ptr<Zombie>& zombie) const;
    bool TryReserveZombie(const std::shared_ptr<Zombie>& zombie) const;

private:
    mutable std::unordered_set<const Zombie*> m_ReservedZombies;
};

struct PlantAction {
    enum class Type {
        SpawnBullet,
        SpawnSun,
        DamageZombie,
        EatZombie,
        RemovePlant,
        PlaySound,
    };

    enum class Sound {
        None,
        CherryBoom,
        ZombieBoom,
    };

    Type type;
    Sound sound = Sound::None;
    std::shared_ptr<Zombie> zombie = nullptr;
    glm::vec2 position = {0.0f, 0.0f};
    int row = -1;
    int col = -1;
    int sourcePlantType = -1;
    int damage = 0;
    bool ice = false;
    bool ashDeath = false;

    static PlantAction SpawnBullet(glm::vec2 position, int row,
                                   int sourcePlantType,
                                   int damage, bool ice) {
        PlantAction action{Type::SpawnBullet};
        action.position = position;
        action.row = row;
        action.sourcePlantType = sourcePlantType;
        action.damage = damage;
        action.ice = ice;
        return action;
    }

    static PlantAction SpawnSun(glm::vec2 position, int row, int col) {
        PlantAction action{Type::SpawnSun};
        action.position = position;
        action.row = row;
        action.col = col;
        return action;
    }

    static PlantAction DamageZombie(std::shared_ptr<Zombie> zombie,
                                    int damage, bool ashDeath = false) {
        PlantAction action{Type::DamageZombie};
        action.zombie = std::move(zombie);
        action.damage = damage;
        action.ashDeath = ashDeath;
        return action;
    }

    static PlantAction EatZombie(std::shared_ptr<Zombie> zombie) {
        PlantAction action{Type::EatZombie};
        action.zombie = std::move(zombie);
        return action;
    }

    static PlantAction RemovePlant(int row, int col) {
        PlantAction action{Type::RemovePlant};
        action.row = row;
        action.col = col;
        return action;
    }

    static PlantAction PlaySound(Sound sound) {
        PlantAction action{Type::PlaySound};
        action.sound = sound;
        return action;
    }
};

#endif // PLANT_ACTION_HPP
