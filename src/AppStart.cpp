#include "App.hpp"

#include "Util/Logger.hpp"

#include "Util/Image.hpp"

void App::InitCards() {
    // Plant data: cost, cooldown frames, card image, CD image
    const PlantInfo plantData[] = {
        {100, 450, RESOURCE_DIR "/Card/bright/peashooter.png",
                    RESOURCE_DIR "/Card/dark/peashooter.png"},
        { 50, 450, RESOURCE_DIR "/Card/bright/sunflower.png",
                    RESOURCE_DIR "/Card/dark/sunflower.png"},
        {150, 2000, RESOURCE_DIR "/Card/bright/cherrybomb.png",
                    RESOURCE_DIR "/Card/dark/cherrybomb.png"},
        { 50, 1800, RESOURCE_DIR "/Card/bright/wallnut.png",
                    RESOURCE_DIR "/Card/dark/wallnut.png"},
        { 25, 1800, RESOURCE_DIR "/Card/bright/mine.png",
                    RESOURCE_DIR "/Card/dark/mine.png"},
        {175, 450, RESOURCE_DIR "/Card/bright/iceshooter.png",
                    RESOURCE_DIR "/Card/dark/iceshooter.png"},
        {150, 450, RESOURCE_DIR "/Card/bright/chomper.png",
                    RESOURCE_DIR "/Card/dark/chomper.png"},
        {200, 450, RESOURCE_DIR "/Card/bright/fastshooter.png",
                    RESOURCE_DIR "/Card/dark/fastshooter.png"},
    };

    constexpr float startX = -500.0f;
    constexpr float spacing = 64.0f;
    constexpr float y = 310.0f;

    m_Cards.clear();
    const auto& level = GetAllLevels()[m_CurrentLevel];
    int slot = 0;
    for (int plantType : level.availablePlants) {
        glm::vec2 pos = {startX + spacing * slot, y};
        auto card = std::make_shared<CardSlot>(
            plantType, plantData[plantType - 1], pos);
        m_Cards.push_back(card);
        m_Root.AddChild(card);
        ++slot;
    }
}

void App::InitLevel() {
    LOG_TRACE("InitLevel (level {})", m_CurrentLevel + 1);

    if (m_StartMenuBackground) {
        m_Root.RemoveChild(m_StartMenuBackground);
        m_StartMenuBackground = nullptr;
    }
    if (m_StartMenuButton) {
        m_Root.RemoveChild(m_StartMenuButton);
        m_StartMenuButton = nullptr;
    }

    const auto& level = GetAllLevels()[m_CurrentLevel];

    m_ActiveLanes = GetActiveLanes(level.lanes);

    m_Background = std::make_shared<Background>(level.lanes);
    // 預設與 m_CameraOffset (= CAMERA_IDLE_OFFSET) 保持一致，避免第一幀畫面閃爍跳動
    m_Background->m_Transform.translation.x = App::CAMERA_IDLE_OFFSET;
    m_Root.AddChild(m_Background);

    m_CardBarBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/Background/list.png"),
        55.0f);
    m_CardBarBackground->m_Transform.translation = {-315.0f, 309.0f};
    m_Root.AddChild(m_CardBarBackground);

    // Sun counter
    m_SunCounter = std::make_shared<SunCounter>();
    m_Root.AddChild(m_SunCounter);

    // Card bar
    InitCards();

    // Shovel
    if (m_CurrentLevel >= 4) {
        m_ShovelSlot = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/Background/shovelslot.png"), 55.0f);
        m_ShovelSlot->m_Transform.scale = {1.41f, 1.41f};
        m_ShovelSlot->m_Transform.translation = {39.0f, 315.6f};
        m_Root.AddChild(m_ShovelSlot);

        m_ShovelIcon = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/Background/shovel.png"), 56.0f);
        m_ShovelIcon->m_Transform.scale = {1.15f, 1.15f};
        m_ShovelIcon->m_Transform.translation = {39.0f, 315.6f};
        m_Root.AddChild(m_ShovelIcon);
    }
    m_IsHoldingShovel = false;

    // Camera intro
    m_CameraOffset = CAMERA_IDLE_OFFSET;
    m_CameraState = CameraState::PAN_RIGHT;
    m_CameraTimer = 0;
    m_Phase = Phase::INTRO_CAMERA;
    m_ReadyAnim = nullptr;
    m_SunAmount = 50;
    m_SunSpawnTimer = 0;
    m_SelectedCard = nullptr;
    m_HoldingPlant = nullptr;
    m_HoldingCardIndex = -1;
    m_PlantGrid = {};
    m_Zombies.clear();
    m_ZombieSpawnTimer = 0;
    m_ZombiesSpawned = 0;
    m_Bullets.clear();
    m_DeathAnims.clear();

    // Level-specific spawn setup
    m_TotalZombiesToSpawn = level.TotalZombies();
    m_SpawnInterval = level.spawnIntervalFrames;
    m_InitialDelayTimer = level.initialDelayFrames;
    m_NormalRemaining = level.normalCount;
    m_ConeheadRemaining = level.coneheadCount;
    m_BucketRemaining = level.bucketCount;
    m_FlagRemaining = level.flagCount;
    m_WaveIndex = 0;
    m_WaveBurstRemaining = 0;

    InitLawnMowers();
}

void App::ClearLevel() {
    if (m_Background) {
        m_Root.RemoveChild(m_Background);
        m_Background = nullptr;
    }
    if (m_CardBarBackground) {
        m_Root.RemoveChild(m_CardBarBackground);
        m_CardBarBackground = nullptr;
    }
    if (m_SunCounter) {
        m_Root.RemoveChild(m_SunCounter);
        m_SunCounter = nullptr;
    }
    if (m_ShovelSlot) {
        m_Root.RemoveChild(m_ShovelSlot);
        m_ShovelSlot = nullptr;
    }
    if (m_ShovelIcon) {
        m_Root.RemoveChild(m_ShovelIcon);
        m_ShovelIcon = nullptr;
    }
    for (auto& card : m_Cards) m_Root.RemoveChild(card);
    m_Cards.clear();
    for (auto& sun : m_Suns) m_Root.RemoveChild(sun);
    m_Suns.clear();
    for (int r = 0; r < GridSystem::ROWS; ++r) {
        for (int c = 0; c < GridSystem::COLS; ++c) {
            if (m_PlantGrid[r][c]) {
                m_Root.RemoveChild(m_PlantGrid[r][c]);
                m_PlantGrid[r][c] = nullptr;
            }
        }
    }
    if (m_HoldingPlant) {
        m_Root.RemoveChild(m_HoldingPlant);
        m_HoldingPlant = nullptr;
    }
    for (auto& z : m_Zombies) m_Root.RemoveChild(z);
    m_Zombies.clear();
    for (auto& b : m_Bullets) m_Root.RemoveChild(b);
    m_Bullets.clear();
    for (auto& d : m_DeathAnims) m_Root.RemoveChild(d);
    m_DeathAnims.clear();
    for (auto& m : m_LawnMowers) {
        if (m) { m_Root.RemoveChild(m); m = nullptr; }
    }
    if (m_ReadyAnim) {
        m_Root.RemoveChild(m_ReadyAnim);
        m_ReadyAnim = nullptr;
    }
}

bool App::IsActiveLane(int row) const {
    for (int lane : m_ActiveLanes) {
        if (lane == row) return true;
    }
    return false;
}

void App::Start() {
    LOG_TRACE("Start");

    m_StartMenuBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/Menu/main.png"),
        100.0f); // Make sure it's visible, adjust zIndex later if needed
    m_StartMenuBackground->SetZIndex(10);
    m_Root.AddChild(m_StartMenuBackground);

    m_StartMenuButton = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR "/Menu/options/0.png"),
        100.0f);
    m_StartMenuButton->SetZIndex(11);
    // Approximate position of the "Start" or "Adventure" button
    m_StartMenuButton->m_Transform.translation = {270.0f, 180.0f};
    m_StartMenuButton->m_Transform.scale = {1.4f, 1.4f};
    
    m_Root.AddChild(m_StartMenuButton);

    // 預先載入轉場動畫以避免切換遊戲階段時卡頓
    std::vector<std::string> fadeOutPaths;
    std::vector<std::string> fadeInPaths;
    for (int i = 0; i <= 99; ++i) {
        fadeOutPaths.push_back(RESOURCE_DIR "/LevelCompleted/win/transition/" + std::to_string(i) + ".png");
    }
    for (int i = 99; i >= 0; --i) {
        fadeInPaths.push_back(RESOURCE_DIR "/LevelCompleted/win/transition/" + std::to_string(i) + ".png");
    }
    
    m_TransitionFadeOut = std::make_shared<AnimatedCharacter>(
        fadeOutPaths,
        false, 10, false);
    m_TransitionFadeOut->SetZIndex(100.0f);
    m_TransitionFadeOut->m_Transform.translation = {0.0f, 0.0f};

    m_TransitionFadeIn = std::make_shared<AnimatedCharacter>(
        fadeInPaths,
        false, 10, false);
    m_TransitionFadeIn->SetZIndex(100.0f);
    m_TransitionFadeIn->m_Transform.translation = {0.0f, 0.0f};

    m_Phase = Phase::MAIN_MENU;
    m_CurrentState = State::UPDATE;
}