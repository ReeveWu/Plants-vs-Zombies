#include "app/App.hpp"

#include "ui/CardSlot.hpp"
#include "entities/Plant.hpp"
#include "ui/SunCounter.hpp"
#include "entities/Zombie.hpp"
#include "Util/Logger.hpp"

#include "Util/Image.hpp"

void App::InitCards() {
    constexpr float startX = -500.0f;
    constexpr float spacing = 64.0f;
    constexpr float y = 310.0f;

    m_Cards.clear();
    const auto& level = GetAllLevels()[m_CurrentLevel];
    int slot = 0;
    for (int plantType : level.availablePlants) {
        PlantType type = ToPlantType(plantType);
        glm::vec2 pos = {startX + spacing * slot, y};
        auto card = std::make_shared<CardSlot>(
            type, PlantCatalog::Get(type), pos);
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
    m_SunSystem.Reset(50, m_SunCounter);

    // Card bar
    InitCards();
    ApplyNoCardCooldown();

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
    m_SelectedCard = nullptr;
    m_HoldingPlant = nullptr;
    m_HoldingCardIndex = -1;
    m_PlantGrid.Clear();
    m_Zombies.clear();
    m_ProjectileSystem.Clear(m_Root);
    m_DeathAnims.clear();

    // Level-specific spawn setup
    m_ZombieSpawner.Reset(level);
    m_LawnMowerSystem.Init(m_ActiveLanes, m_CameraOffset, m_Root);
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
    m_SunSystem.Clear(m_Root);
    for (const auto& plant : m_PlantGrid.Clear()) {
        m_Root.RemoveChild(plant);
    }
    if (m_HoldingPlant) {
        m_Root.RemoveChild(m_HoldingPlant);
        m_HoldingPlant = nullptr;
    }
    for (auto& z : m_Zombies) m_Root.RemoveChild(z);
    m_Zombies.clear();
    m_ProjectileSystem.Clear(m_Root);
    for (auto& d : m_DeathAnims) m_Root.RemoveChild(d);
    m_DeathAnims.clear();
    m_LawnMowerSystem.Clear(m_Root);
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
