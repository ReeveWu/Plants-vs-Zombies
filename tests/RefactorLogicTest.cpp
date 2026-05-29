#include "config/LevelConfig.hpp"
#include "catalog/PlantCatalog.hpp"
#include "systems/PlantGrid.hpp"
#include "systems/ZombieSpawner.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace {

std::shared_ptr<Plant> MakeFakePlant() {
    auto owner = std::make_shared<int>(1);
    return std::shared_ptr<Plant>(owner,
                                  reinterpret_cast<Plant*>(owner.get()));
}

void TestPlantCatalog() {
    const auto& all = PlantCatalog::All();
    assert(all.size() == 9);

    const auto& peashooter = PlantCatalog::Get(PlantType::Peashooter);
    assert(peashooter.cost == 100);
    assert(peashooter.cooldownFrames == 450);
    assert(peashooter.cardImage.find("/Card/bright/peashooter.png") !=
           std::string::npos);
    assert(peashooter.cardCooldownImage.find("/Card/dark/peashooter.png") !=
           std::string::npos);
    assert(peashooter.idleDirectory == "peashooter");
    assert(peashooter.idleFrameCount == 24);
    assert(peashooter.isPlant);

    const auto& cherryBomb = PlantCatalog::Get(PlantType::CherryBomb);
    assert(cherryBomb.cost == 150);
    assert(cherryBomb.cooldownFrames == 2000);
    assert(cherryBomb.rewardTitle == "CHERRY BOMB");

    const auto& shovel = PlantCatalog::Get(PlantType::Shovel);
    assert(!shovel.isPlant);
    assert(shovel.rewardImage.find("/Background/shovel.png") !=
           std::string::npos);
}

void TestPlantGrid() {
    PlantGrid grid;
    std::vector<int> activeLanes = {1, 2, 3};

    assert(grid.CanPlace(2, 4, activeLanes));
    assert(!grid.CanPlace(0, 4, activeLanes));
    assert(!grid.CanPlace(2, GridSystem::COLS, activeLanes));

    auto plant = MakeFakePlant();
    grid.Place(2, 4, plant);
    assert(grid.At(2, 4) == plant);
    assert(!grid.CanPlace(2, 4, activeLanes));

    auto removed = grid.Remove(2, 4);
    assert(removed == plant);
    assert(grid.At(2, 4) == nullptr);

    grid.Place(1, 1, MakeFakePlant());
    grid.Place(3, 3, MakeFakePlant());
    auto removedPlants = grid.Clear();
    assert(removedPlants.size() == 2);
    assert(grid.At(1, 1) == nullptr);
    assert(grid.At(3, 3) == nullptr);
}

void TestZombieSpawnerCounters() {
    LevelConfig level{
        5,
        {1, 2},
        1,
        1,
        1,
        1,
        600,
        0,
        {{0.5f, 2}},
        0,
    };

    ZombieSpawner spawner;
    spawner.Reset(level);
    assert(spawner.GetTotalZombies() == 4);
    assert(spawner.GetSpawnedCount() == 0);
    assert(spawner.GetRegularRemaining() == 3);
    assert(spawner.GetFlagRemaining() == 1);

    assert(spawner.TakeFlagZombie());
    assert(!spawner.TakeFlagZombie());
    assert(spawner.GetFlagRemaining() == 0);

    assert(spawner.TakeRegularZombieByRoll(0) == ZombieType::Normal);
    assert(spawner.GetRegularRemaining() == 2);
    assert(spawner.TakeRegularZombieByRoll(0) == ZombieType::Conehead);
    assert(spawner.GetRegularRemaining() == 1);
    assert(spawner.TakeRegularZombieByRoll(0) == ZombieType::Bucket);
    assert(spawner.GetRegularRemaining() == 0);
}

} // namespace

int main() {
    TestPlantCatalog();
    TestPlantGrid();
    TestZombieSpawnerCounters();

    std::cout << "Refactor logic tests passed\n";
    return 0;
}
