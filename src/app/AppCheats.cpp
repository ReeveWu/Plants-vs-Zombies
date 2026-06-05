#include "app/App.hpp"

#include "entities/Plant.hpp"
#include "entities/Zombie.hpp"
#include "ui/CardSlot.hpp"
#include "Util/Input.hpp"
#include "Util/Image.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include <algorithm>
#include <array>
#include <sstream>

namespace {

struct LevelHotkey {
    Util::Keycode key;
    int normalLevelIndex;
    int shiftedLevelIndex;
};

bool IsShiftPressed() {
    return Util::Input::IsKeyPressed(Util::Keycode::LSHIFT) ||
           Util::Input::IsKeyPressed(Util::Keycode::RSHIFT);
}

} // namespace

void App::HandleCheatKeys() {
    static constexpr int NO_SHIFT_LEVEL = -1;
    static constexpr std::array<LevelHotkey, 10> LEVEL_KEYS = {{
        {Util::Keycode::NUM_1, 0, 10},
        {Util::Keycode::NUM_2, 1, 11},
        {Util::Keycode::NUM_3, 2, 12},
        {Util::Keycode::NUM_4, 3, 13},
        {Util::Keycode::NUM_5, 4, 14},
        {Util::Keycode::NUM_6, 5, NO_SHIFT_LEVEL},
        {Util::Keycode::NUM_7, 6, NO_SHIFT_LEVEL},
        {Util::Keycode::NUM_8, 7, NO_SHIFT_LEVEL},
        {Util::Keycode::NUM_9, 8, NO_SHIFT_LEVEL},
        {Util::Keycode::NUM_0, 9, NO_SHIFT_LEVEL},
    }};

    if (Util::Input::IsKeyDown(Util::Keycode::H)) {
        ToggleCheatMode();
        return;
    }

    if (!m_CheatModeEnabled) {
        return;
    }

    const bool shiftPressed = IsShiftPressed();
    for (const auto& hotkey : LEVEL_KEYS) {
        if (!Util::Input::IsKeyDown(hotkey.key)) continue;

        int targetLevel = hotkey.normalLevelIndex;
        if (shiftPressed && hotkey.shiftedLevelIndex != NO_SHIFT_LEVEL) {
            targetLevel = hotkey.shiftedLevelIndex;
        }
        JumpToLevel(targetLevel);
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::F)) {
        CompleteCurrentLevel();
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
        LoseCurrentLevel();
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::R)) {
        RestartCurrentLevel();
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::C)) {
        m_NoCardCooldown = !m_NoCardCooldown;
        ApplyNoCardCooldown();
        UpdateCheatOverlay();
        LOG_DEBUG("No card cooldown {}", m_NoCardCooldown ? "enabled" : "disabled");
        return;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::U)) {
        m_SunSystem.SetUnlimited(!m_SunSystem.IsUnlimited());
        LOG_DEBUG("Unlimited sun {}", m_SunSystem.IsUnlimited() ? "enabled" : "disabled");
    }

    UpdateCheatOverlay();
}

void App::JumpToLevel(int levelIndex) {
    const int maxLevelIndex = static_cast<int>(GetAllLevels().size()) - 1;
    levelIndex = std::clamp(levelIndex, 0, maxLevelIndex);

    ClearCompletionUi();
    ClearLevel();
    m_CurrentLevel = levelIndex;
    m_CurrentState = State::UPDATE;
    InitLevel();
    ApplyNoCardCooldown();
    UpdateCheatOverlay();

    LOG_DEBUG("Cheat: jumped to level {}", m_CurrentLevel + 1);
}

void App::RestartCurrentLevel() {
    JumpToLevel(m_CurrentLevel);
    LOG_DEBUG("Cheat: restarted level {}", m_CurrentLevel + 1);
}

void App::CompleteCurrentLevel() {
    if (m_Phase == Phase::MAIN_MENU ||
        m_Phase == Phase::MAIN_MENU_FLASH ||
        m_Phase == Phase::LEVEL_COMPLETE ||
        m_Phase == Phase::REWARD_SCREEN ||
        m_Phase == Phase::GAME_OVER) {
        return;
    }

    ClearCompletionUi();

    for (auto& zombie : m_Zombies) {
        m_Root.RemoveChild(zombie);
    }
    m_Zombies.clear();

    for (auto& corpse : m_DeathAnims) {
        m_Root.RemoveChild(corpse);
    }
    m_DeathAnims.clear();

    if (m_HoldingPlant) {
        m_Root.RemoveChild(m_HoldingPlant);
        m_HoldingPlant = nullptr;
    }
    m_SelectedCard = nullptr;
    m_HoldingCardIndex = -1;

    m_ProjectileSystem.Clear(m_Root);
    m_ZombieSpawner.CompleteAll();
    m_EndTimer = 0;
    m_Phase = Phase::LEVEL_COMPLETE;
    UpdateCheatOverlay();

    LOG_DEBUG("Cheat: completed level {}", m_CurrentLevel + 1);
}

void App::LoseCurrentLevel() {
    if (m_Phase == Phase::MAIN_MENU ||
        m_Phase == Phase::MAIN_MENU_FLASH ||
        m_Phase == Phase::LEVEL_COMPLETE ||
        m_Phase == Phase::REWARD_SCREEN ||
        m_Phase == Phase::GAME_OVER) {
        return;
    }

    ClearCompletionUi();

    if (m_HoldingPlant) {
        m_Root.RemoveChild(m_HoldingPlant);
        m_HoldingPlant = nullptr;
    }
    m_SelectedCard = nullptr;
    m_HoldingCardIndex = -1;

    m_EndScreen = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(
            RESOURCE_DIR "/LevelCompleted/lose/0.png"),
        100.0f);
    m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
    m_Root.AddChild(m_EndScreen);
    m_EndTimer = 0;
    m_Phase = Phase::GAME_OVER;
    UpdateCheatOverlay();

    LOG_DEBUG("Cheat: lost level {}", m_CurrentLevel + 1);
}

void App::ApplyNoCardCooldown() {
    for (auto& card : m_Cards) {
        card->SetNoCooldown(m_NoCardCooldown);
    }
}

void App::ToggleCheatMode() {
    m_CheatModeEnabled = !m_CheatModeEnabled;

    if (!m_CheatModeEnabled) {
        m_SunSystem.SetUnlimited(false);
        m_NoCardCooldown = false;
        ApplyNoCardCooldown();
    }

    EnsureCheatOverlay();
    m_CheatOverlay->SetVisible(m_CheatModeEnabled);
    m_CheatStatusOverlay->SetVisible(m_CheatModeEnabled);
    UpdateCheatOverlay();

    LOG_DEBUG("Cheat mode {}", m_CheatModeEnabled ? "enabled" : "disabled");
}

void App::EnsureCheatOverlay() {
    if (!m_CheatOverlay) {
        m_CheatOverlayText = std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/Geo-Regular.ttf", 18, "CHEAT MODE",
            Util::Color(0, 0, 0));
        m_CheatOverlay = std::make_shared<Util::GameObject>(
            m_CheatOverlayText, 99.0f);
        m_CheatOverlay->SetVisible(false);
        m_Root.AddChild(m_CheatOverlay);
    }

    if (!m_CheatStatusOverlay) {
        m_CheatStatusText = std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/Geo-Regular.ttf", 18, "CHEAT STATUS",
            Util::Color(0, 0, 0));
        m_CheatStatusOverlay = std::make_shared<Util::GameObject>(
            m_CheatStatusText, 99.0f);
        m_CheatStatusOverlay->SetVisible(false);
        m_Root.AddChild(m_CheatStatusOverlay);
    }
}

void App::UpdateCheatOverlay() {
    if (!m_CheatModeEnabled) return;
    EnsureCheatOverlay();

    std::ostringstream statusText;
    statusText << "Level: " << (m_CurrentLevel + 1) << "/"
               << GetAllLevels().size() << "\n"
               << "Unlimited Sun: "
               << (m_SunSystem.IsUnlimited() ? "ON" : "OFF") << "\n"
               << "No CD: " << (m_NoCardCooldown ? "ON" : "OFF");

    const std::string statusContent = statusText.str();
    if (statusContent != m_CheatStatusContent) {
        m_CheatStatusContent = statusContent;
        m_CheatStatusText->SetText(m_CheatStatusContent);
    }

    const glm::vec2 statusSize = m_CheatStatusOverlay->GetScaledSize();
    constexpr float margin = 24.0f;
    m_CheatStatusOverlay->SetPivot({0.0f, 0.0f});
    m_CheatStatusOverlay->m_Transform.translation = {
        640.0f - margin - statusSize.x / 2.0f,
        360.0f - margin - statusSize.y / 2.0f,
    };

    std::ostringstream helpText;
    helpText << "CHEAT MODE (H: hide)\n"
             << "1-9/0: Jump 1-10\n"
             << "Shift+1-5: Jump 11-15\n"
             << "F: Win  L: Lose  R: Replay\n"
             << "U: Sun  C: No CD";

    const std::string helpContent = helpText.str();
    if (helpContent != m_CheatOverlayContent) {
        m_CheatOverlayContent = helpContent;
        m_CheatOverlayText->SetText(m_CheatOverlayContent);
    }

    const glm::vec2 size = m_CheatOverlay->GetScaledSize();
    m_CheatOverlay->SetPivot({0.0f, 0.0f});
    m_CheatOverlay->m_Transform.translation = {
        640.0f - margin - size.x / 2.0f,
        -360.0f + margin + size.y / 2.0f,
    };
}

void App::ClearCompletionUi() {
    if (m_EndScreen) {
        m_Root.RemoveChild(m_EndScreen);
        m_EndScreen = nullptr;
    }
    if (m_RewardBackground) {
        m_Root.RemoveChild(m_RewardBackground);
        m_RewardBackground = nullptr;
    }
    if (m_RewardPlant) {
        m_Root.RemoveChild(m_RewardPlant);
        m_RewardPlant = nullptr;
    }
    if (m_RewardButton) {
        m_Root.RemoveChild(m_RewardButton);
        m_RewardButton = nullptr;
    }
    if (m_RewardTextTitle) {
        m_Root.RemoveChild(m_RewardTextTitle);
        m_RewardTextTitle = nullptr;
    }
    if (m_RewardTextDesc) {
        m_Root.RemoveChild(m_RewardTextDesc);
        m_RewardTextDesc = nullptr;
    }
    m_EndTimer = 0;
}
