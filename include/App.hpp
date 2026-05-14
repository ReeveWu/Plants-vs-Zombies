#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"

#include "AnimatedCharacter.hpp"
#include "Background.hpp"
#include "LevelConfig.hpp"
#include "Bullet.hpp"
#include "CardSlot.hpp"
#include "GridSystem.hpp"
#include "LawnMower.hpp"
#include "BucketZombie.hpp"
#include "ConeheadZombie.hpp"
#include "FlagZombie.hpp"
#include "NormalZombie.hpp"
#include "Plant.hpp"
#include "Peashooter.hpp"
#include "IceShooter.hpp"
#include "FastShooter.hpp"
#include "Sunflower.hpp"
#include "Wallnut.hpp"
#include "PotatoMine.hpp"
#include "CherryBomb.hpp"
#include "Chomper.hpp"
#include "Sun.hpp"
#include "SunCounter.hpp"
#include "ZombieCorpse.hpp"

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

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
        MAIN_MENU,
        MAIN_MENU_FLASH,
        INTRO_CAMERA,
        READY_ANIM,
        GAMEPLAY,
        LEVEL_COMPLETE,
        REWARD_SCREEN,
        LEVEL_FADE_IN,
        GAME_OVER,
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
    void InitLevel();
    void PlacePlant(int row, int col);
    void CancelHolding();
    void SpawnZombie();
    void UpdateZombies();
    void UpdateBullets();
    void UpdatePlantShooting();
    void UpdatePlantSunProduction();
    void UpdateZombieEating();
    void UpdateLawnMowers();
    void InitLawnMowers();
    void UpdateSpecialPlants();
    void CheckWinLose();
    void ClearLevel();
    bool IsActiveLane(int row) const;
    void UpdateDeathAnims();
    void SpawnDeathAnims(const std::shared_ptr<Zombie>& zombie);
    std::shared_ptr<Plant> CreatePlant(int typeIndex, int row, int col);

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

    // Shovel system
    std::shared_ptr<Util::GameObject> m_ShovelSlot;
    std::shared_ptr<Util::GameObject> m_ShovelIcon;
    bool m_IsHoldingShovel = false;

    // Level system
    int m_CurrentLevel = 0;
    std::vector<int> m_ActiveLanes;
    int m_TotalZombiesToSpawn = 0;
    int m_SpawnInterval = 600;
    int m_InitialDelayTimer = 0;
    int m_NormalRemaining = 0;
    int m_ConeheadRemaining = 0;
    int m_BucketRemaining = 0;
    int m_FlagRemaining = 0;
    int m_WaveIndex = 0;
    int m_WaveBurstRemaining = 0;

    // Zombie system
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    int m_ZombieSpawnTimer = 0;
    int m_ZombiesSpawned = 0;
    static constexpr float ZOMBIE_LOSE_X = -600.0f;

    // Bullet system
    std::vector<std::shared_ptr<Bullet>> m_Bullets;

    // Death animation system
    std::vector<std::shared_ptr<ZombieCorpse>> m_DeathAnims;

    // LawnMower system
    std::array<std::shared_ptr<LawnMower>, GridSystem::ROWS> m_LawnMowers{};
    static constexpr float LAWNMOWER_X = -560.0f;

    // End-game UI
    std::shared_ptr<Util::GameObject> m_EndScreen;
    std::shared_ptr<AnimatedCharacter> m_TransitionFadeOut;
    std::shared_ptr<AnimatedCharacter> m_TransitionFadeIn;
    std::shared_ptr<Util::GameObject> m_RewardBackground;
    std::shared_ptr<Util::GameObject> m_RewardPlant;
    std::shared_ptr<Util::GameObject> m_RewardButton;
    std::shared_ptr<Util::GameObject> m_RewardTextTitle;
    std::shared_ptr<Util::GameObject> m_RewardTextDesc;
    int m_EndTimer = 0;
    static constexpr int END_SCREEN_DURATION = 180;

    float m_CameraOffset = 0.0f;
    int m_CameraTimer = 0;

    static constexpr float CAMERA_SPEED = 3.0f;
    static constexpr float CAMERA_IDLE_OFFSET = 180.0f;  // play position (house visible)
    static constexpr float CAMERA_PAN_TARGET = -150.0f;  // road-preview position
    static constexpr int CAMERA_PAUSE_FRAMES = 60;
    static constexpr int SUN_SPAWN_INTERVAL = 300;
    // Start screen objects
    std::shared_ptr<Util::GameObject> m_StartMenuBackground;
    std::shared_ptr<Util::GameObject> m_StartMenuButton;    
    int m_MainMenuFlashTimer = 0;
    int m_MainMenuFlashCount = 0;
};

#endif
