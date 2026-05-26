#include "PlantCatalog.hpp"

#include <stdexcept>

PlantType ToPlantType(int id) {
    if (id < ToPlantTypeId(PlantType::Peashooter) ||
        id > ToPlantTypeId(PlantType::Shovel)) {
        throw std::out_of_range("Unknown plant type id");
    }
    return static_cast<PlantType>(id);
}

const std::array<PlantDefinition, 9>& PlantCatalog::All() {
    static const std::array<PlantDefinition, 9> definitions = {{
        {PlantType::Peashooter, 100, 450,
         RESOURCE_DIR "/Card/bright/peashooter.png",
         RESOURCE_DIR "/Card/dark/peashooter.png",
         RESOURCE_DIR "/Card/bright/peashooter.png",
         "PEASHOOTER", "Shoots peas at the enemy",
         "peashooter", 24, true},
        {PlantType::Sunflower, 50, 450,
         RESOURCE_DIR "/Card/bright/sunflower.png",
         RESOURCE_DIR "/Card/dark/sunflower.png",
         RESOURCE_DIR "/Card/bright/sunflower.png",
         "SUNFLOWER", "Gives you additional sun",
         "sunflower", 24, true},
        {PlantType::CherryBomb, 150, 2000,
         RESOURCE_DIR "/Card/bright/cherrybomb.png",
         RESOURCE_DIR "/Card/dark/cherrybomb.png",
         RESOURCE_DIR "/Card/bright/cherrybomb.png",
         "CHERRY BOMB", "Blows up all zombies in an\narea",
         "cherrybomb", 14, true},
        {PlantType::Wallnut, 50, 1800,
         RESOURCE_DIR "/Card/bright/wallnut.png",
         RESOURCE_DIR "/Card/dark/wallnut.png",
         RESOURCE_DIR "/Card/bright/wallnut.png",
         "WALL-NUT", "Blocks off zombies and\nprotects your other plants",
         "wallnut", 32, true},
        {PlantType::PotatoMine, 25, 1800,
         RESOURCE_DIR "/Card/bright/mine.png",
         RESOURCE_DIR "/Card/dark/mine.png",
         RESOURCE_DIR "/Card/bright/mine.png",
         "POTATO MINE", "Explodes on contact, but\ntakes time to arm itself",
         "mine", 8, true},
        {PlantType::IceShooter, 175, 450,
         RESOURCE_DIR "/Card/bright/iceshooter.png",
         RESOURCE_DIR "/Card/dark/iceshooter.png",
         RESOURCE_DIR "/Card/bright/iceshooter.png",
         "SNOW PEA", "Shoots frozen peas that\ndamage and slow the enemy",
         "iceshooter", 15, true},
        {PlantType::Chomper, 150, 450,
         RESOURCE_DIR "/Card/bright/chomper.png",
         RESOURCE_DIR "/Card/dark/chomper.png",
         RESOURCE_DIR "/Card/bright/chomper.png",
         "CHOMPER", "Devours a zombie whole, but\nis vulnerable while chewing",
         "chomper", 13, true},
        {PlantType::FastShooter, 200, 450,
         RESOURCE_DIR "/Card/bright/fastshooter.png",
         RESOURCE_DIR "/Card/dark/fastshooter.png",
         RESOURCE_DIR "/Card/bright/fastshooter.png",
         "REPEATER", "Fires two peas at a time",
         "fastshooter", 15, true},
        {PlantType::Shovel, 0, 0,
         RESOURCE_DIR "/Background/shovel.png",
         RESOURCE_DIR "/Background/shovel.png",
         RESOURCE_DIR "/Background/shovel.png",
         "SHOVEL", "Digs up plants to make\nroom for new ones",
         "", 0, false},
    }};
    return definitions;
}

const PlantDefinition& PlantCatalog::Get(PlantType type) {
    const auto id = ToPlantTypeId(type);
    if (id < ToPlantTypeId(PlantType::Peashooter) ||
        id > ToPlantTypeId(PlantType::Shovel)) {
        throw std::out_of_range("Unknown plant type");
    }
    return All()[static_cast<std::size_t>(id - 1)];
}

bool PlantCatalog::IsPlant(PlantType type) {
    return Get(type).isPlant;
}
