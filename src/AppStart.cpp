#include "App.hpp"

#include "Util/Logger.hpp"

#include "Util/Image.hpp"

void App::InitCards() {
    // Plant data: cost, cooldown frames, card image, CD image
    const PlantInfo plantData[] = {
        {100, 450, RESOURCE_DIR "/Background/Card/bright/peashooter.png",
                    RESOURCE_DIR "/Background/Card/dark/peashooter.png"},
        { 50, 450, RESOURCE_DIR "/Background/Card/bright/sunflower.png",
                    RESOURCE_DIR "/Background/Card/dark/sunflower.png"},
        {150, 2000, RESOURCE_DIR "/Background/Card/bright/cherrybomb.png",
                    RESOURCE_DIR "/Background/Card/dark/cherrybomb.png"},
        { 50, 1800, RESOURCE_DIR "/Background/Card/bright/wallnut.png",
                    RESOURCE_DIR "/Background/Card/dark/wallnut.png"},
        { 25, 1800, RESOURCE_DIR "/Background/Card/bright/mine.png",
                    RESOURCE_DIR "/Background/Card/dark/mine.png"},
        {175, 450, RESOURCE_DIR "/Background/Card/bright/iceshooter.png",
                    RESOURCE_DIR "/Background/Card/dark/iceshooter.png"},
        {150, 450, RESOURCE_DIR "/Background/Card/bright/chomper.png",
                    RESOURCE_DIR "/Background/Card/dark/chomper.png"},
        {200, 450, RESOURCE_DIR "/Background/Card/bright/fastshooter.png",
                    RESOURCE_DIR "/Background/Card/dark/fastshooter.png"},
    };

    constexpr float startX = -500.0f;
    constexpr float spacing = 64.0f;
    constexpr float y = 310.0f;

    m_Cards.clear();
    for (int i = 0; i < 8; ++i) {
        glm::vec2 pos = {startX + spacing * i, y};
        auto card = std::make_shared<CardSlot>(i + 1, plantData[i], pos);
        m_Cards.push_back(card);
        m_Root.AddChild(card);
    }
}

void App::Start() {
    LOG_TRACE("Start");

    m_Background = std::make_shared<Background>();
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

    // Camera intro
    m_CameraOffset = CAMERA_IDLE_OFFSET;
    m_CameraState = CameraState::PAN_RIGHT;
    m_CameraTimer = 0;
    m_Phase = Phase::INTRO_CAMERA;
    m_ReadyAnim = nullptr;
    m_SunAmount = 50;
    m_SunSpawnTimer = 0;
    m_SelectedCard = nullptr;

    m_CurrentState = State::UPDATE;
}
