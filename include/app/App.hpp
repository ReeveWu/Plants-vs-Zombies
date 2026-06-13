#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"

#include "core/AnimatedCharacter.hpp"
#include "ui/Background.hpp"
#include "config/LevelConfig.hpp"
#include "systems/LawnMowerSystem.hpp"
#include "catalog/PlantAction.hpp"
#include "catalog/PlantCatalog.hpp"
#include "systems/AudioSystem.hpp"
#include "systems/PlantGrid.hpp"
#include "systems/ProjectileSystem.hpp"
#include "systems/SunSystem.hpp"
#include "systems/ZombieSpawner.hpp"
#include "entities/ZombieCorpse.hpp"

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"

class CardSlot;
class Plant;
class SunCounter;
class Zombie;

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
    void InitCards();
    void InitLevel();
    void PlacePlant(int row, int col);
    void CancelHolding();
    void UpdateZombies();
    void UpdatePlantBehaviors();
    void ApplyPlantActions(const std::vector<PlantAction>& actions);
    void UpdateZombieEating();
    void CheckWinLose();
    void ClearLevel();
    bool IsActiveLane(int row) const;
    void UpdateDeathAnims();
    void SpawnDeathAnims(const std::shared_ptr<Zombie>& zombie);
    void ShowGameOverUi();
    void HandleCheatKeys();
    void JumpToLevel(int levelIndex);
    void RestartCurrentLevel();
    void CompleteCurrentLevel();
    void LoseCurrentLevel();
    void ApplyNoCardCooldown();
    void ToggleCheatMode();
    void ToggleCheatPause();
    void EnsureCheatOverlay();
    void UpdateCheatOverlay();
    void ClearCompletionUi();

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
    SunSystem m_SunSystem;
    std::shared_ptr<SunCounter> m_SunCounter;

    // Card system
    std::vector<std::shared_ptr<CardSlot>> m_Cards;
    std::shared_ptr<CardSlot> m_SelectedCard;
    bool m_NoCardCooldown = false;

    // Cheat mode
    bool m_CheatModeEnabled = false;
    bool m_GamePausedByCheat = false;
    std::shared_ptr<Util::GameObject> m_CheatOverlay;
    std::shared_ptr<Util::Text> m_CheatOverlayText;
    std::string m_CheatOverlayContent;
    std::shared_ptr<Util::GameObject> m_CheatStatusOverlay;
    std::shared_ptr<Util::Text> m_CheatStatusText;
    std::string m_CheatStatusContent;

    AudioSystem m_Audio;

    // Plant grid & drag state
    PlantGrid m_PlantGrid;
    std::shared_ptr<Plant> m_HoldingPlant;
    int m_HoldingCardIndex = -1;

    // Shovel system
    std::shared_ptr<Util::GameObject> m_ShovelSlot;
    std::shared_ptr<Util::GameObject> m_ShovelIcon;
    bool m_IsHoldingShovel = false;

    // Level system
    int m_CurrentLevel = 0;
    std::vector<int> m_ActiveLanes;

    // Zombie system
    ZombieSpawner m_ZombieSpawner;
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    static constexpr float ZOMBIE_LOSE_X = -600.0f;

    // Bullet system
    ProjectileSystem m_ProjectileSystem;

    // Death animation system
    std::vector<std::shared_ptr<ZombieCorpse>> m_DeathAnims;

    // LawnMower system
    LawnMowerSystem m_LawnMowerSystem;

    // End-game UI
    std::shared_ptr<Util::GameObject> m_EndScreen;
    std::shared_ptr<AnimatedCharacter> m_TransitionFadeOut;
    std::shared_ptr<AnimatedCharacter> m_TransitionFadeIn;
    std::shared_ptr<Util::GameObject> m_RewardBackground;
    std::shared_ptr<Util::GameObject> m_RewardPlant;
    std::shared_ptr<Util::GameObject> m_RewardButton;
    std::shared_ptr<Util::GameObject> m_RewardTextTitle;
    std::shared_ptr<Util::GameObject> m_RewardTextDesc;
    std::shared_ptr<Util::GameObject> m_GameOverPrompt;
    int m_EndTimer = 0;
    static constexpr int END_SCREEN_DURATION = 180;

    float m_CameraOffset = 0.0f;
    int m_CameraTimer = 0;

    static constexpr float CAMERA_SPEED = 3.0f;
    static constexpr float CAMERA_IDLE_OFFSET = 180.0f;  // play position (house visible)
    static constexpr float CAMERA_PAN_TARGET = -150.0f;  // road-preview position
    static constexpr int CAMERA_PAUSE_FRAMES = 60;
    // Start screen objects
    std::shared_ptr<Util::GameObject> m_StartMenuBackground;
    std::shared_ptr<Util::GameObject> m_StartMenuButton;    
    int m_MainMenuFlashTimer = 0;
    int m_MainMenuFlashCount = 0;
};

#endif
