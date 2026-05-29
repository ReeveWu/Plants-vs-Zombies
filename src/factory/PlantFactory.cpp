#include "factory/PlantFactory.hpp"

#include "plants/CherryBomb.hpp"
#include "plants/Chomper.hpp"
#include "plants/FastShooter.hpp"
#include "plants/IceShooter.hpp"
#include "plants/Peashooter.hpp"
#include "entities/Plant.hpp"
#include "plants/PotatoMine.hpp"
#include "plants/Sunflower.hpp"
#include "plants/Wallnut.hpp"

std::shared_ptr<Plant> PlantFactory::Create(PlantType type, int row, int col) {
    switch (type) {
    case PlantType::Peashooter:
        return std::make_shared<Peashooter>(row, col);
    case PlantType::Sunflower:
        return std::make_shared<Sunflower>(row, col);
    case PlantType::CherryBomb:
        return std::make_shared<CherryBomb>(row, col);
    case PlantType::Wallnut:
        return std::make_shared<Wallnut>(row, col);
    case PlantType::PotatoMine:
        return std::make_shared<PotatoMine>(row, col);
    case PlantType::IceShooter:
        return std::make_shared<IceShooter>(row, col);
    case PlantType::Chomper:
        return std::make_shared<Chomper>(row, col);
    case PlantType::FastShooter:
        return std::make_shared<FastShooter>(row, col);
    case PlantType::Shovel:
        break;
    }
    return std::make_shared<Plant>(ToPlantTypeId(type), row, col);
}
