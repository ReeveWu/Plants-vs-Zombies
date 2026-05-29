#ifndef PLANT_FACTORY_HPP
#define PLANT_FACTORY_HPP

#include "catalog/PlantCatalog.hpp"

#include <memory>

class Plant;

class PlantFactory {
public:
    static std::shared_ptr<Plant> Create(PlantType type, int row, int col);
};

#endif // PLANT_FACTORY_HPP
