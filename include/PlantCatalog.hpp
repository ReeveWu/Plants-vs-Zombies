#ifndef PLANT_CATALOG_HPP
#define PLANT_CATALOG_HPP

#include <array>
#include <string>

enum class PlantType {
    Peashooter = 1,
    Sunflower = 2,
    CherryBomb = 3,
    Wallnut = 4,
    PotatoMine = 5,
    IceShooter = 6,
    Chomper = 7,
    FastShooter = 8,
    Shovel = 9,
};

constexpr int ToPlantTypeId(PlantType type) {
    return static_cast<int>(type);
}

PlantType ToPlantType(int id);

struct PlantDefinition {
    PlantType type;
    int cost;
    int cooldownFrames;
    std::string cardImage;
    std::string cardCooldownImage;
    std::string rewardImage;
    std::string rewardTitle;
    std::string rewardDescription;
    std::string idleDirectory;
    int idleFrameCount;
    bool isPlant;
};

class PlantCatalog {
public:
    static const PlantDefinition& Get(PlantType type);
    static const std::array<PlantDefinition, 9>& All();
    static bool IsPlant(PlantType type);
};

#endif // PLANT_CATALOG_HPP
