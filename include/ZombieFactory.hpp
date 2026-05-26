#ifndef ZOMBIE_FACTORY_HPP
#define ZOMBIE_FACTORY_HPP

#include <memory>

class Zombie;

enum class ZombieType {
    Normal,
    Conehead,
    Bucket,
    Flag,
};

class ZombieFactory {
public:
    static std::shared_ptr<Zombie> Create(ZombieType type, int row);
};

#endif // ZOMBIE_FACTORY_HPP
