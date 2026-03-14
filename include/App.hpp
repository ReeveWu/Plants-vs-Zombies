#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"

#include "AnimatedCharacter.hpp"
#include "Background.hpp"
#include "CardSlot.hpp"
#include "GridSystem.hpp"
#include "NormalZombie.hpp"
#include "Plant.hpp"
#include "Sun.hpp"
#include "SunCounter.hpp"

#include "Util/GameObject.hpp"

#include <array>

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    enum class Phase {
        INTRO_CAMERA,
        READY_ANIM,
        GAMEPLAY,
        LEVEL_COMPLETE,
    };

    enum class CameraState {
        IDLE,
        PAN_RIGHT,
        PAN_PAUSE,
        PAN_LEFT,
    };

    void UpdateCamera();
    void UpdateGameplay();
    void SpawnSkySun();
    void InitCards();
    void PlacePlant(int row, int col);
    void CancelHolding();
    void SpawnZombie();
    void UpdateZombies();

private:
    State m_CurrentState = State::START;
    Phase m_Phase = Phase::INTRO_CAMERA;
    CameraState m_CameraState = CameraState::PAN_RIGHT;

    Util::Renderer m_Root;

    // Game objects
    std::shared_ptr<Background> m_Background;
    std::shared_ptr<Util::GameObject> m_CardBarBackground;
    std::shared_ptr<AnimatedCharacter> m_ReadyAnim;

    // Sun system
    std::vector<std::shared_ptr<Sun>> m_Suns;
    std::shared_ptr<SunCounter> m_SunCounter;
    int m_SunAmount = 50;
    int m_SunSpawnTimer = 0;

    // Card system
    std::vector<std::shared_ptr<CardSlot>> m_Cards;
    std::shared_ptr<CardSlot> m_SelectedCard;

    // Plant grid & drag state
    std::array<std::array<std::shared_ptr<Plant>, GridSystem::COLS>, GridSystem::ROWS> m_PlantGrid{};
    std::shared_ptr<Plant> m_HoldingPlant;
    int m_HoldingCardIndex = -1;

    // Zombie system
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    int m_ZombieSpawnTimer = 0;
    int m_ZombiesSpawned = 0;
    static constexpr int ZOMBIE_SPAWN_INTERVAL = 600;
    static constexpr int ZOMBIES_PER_LEVEL = 10;
    static constexpr float ZOMBIE_LOSE_X = -600.0f;

    float m_CameraOffset = 0.0f;
    int m_CameraTimer = 0;

    static constexpr float CAMERA_SPEED = 3.0f;
    static constexpr float CAMERA_IDLE_OFFSET = 180.0f;  // play position (house visible)
    static constexpr float CAMERA_PAN_TARGET = -150.0f;  // road-preview position
    static constexpr int CAMERA_PAUSE_FRAMES = 60;
    static constexpr int SUN_SPAWN_INTERVAL = 350;
};

#endif
