#include "factory/ZombieFactory.hpp"

#include "zombies/BucketZombie.hpp"
#include "zombies/ConeheadZombie.hpp"
#include "zombies/FlagZombie.hpp"
#include "zombies/NormalZombie.hpp"

std::shared_ptr<Zombie> ZombieFactory::Create(ZombieType type, int row) {
    switch (type) {
    case ZombieType::Normal:
        return std::make_shared<NormalZombie>(row);
    case ZombieType::Conehead:
        return std::make_shared<ConeheadZombie>(row);
    case ZombieType::Bucket:
        return std::make_shared<BucketZombie>(row);
    case ZombieType::Flag:
        return std::make_shared<FlagZombie>(row);
    }
    return std::make_shared<NormalZombie>(row);
}
