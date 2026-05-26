#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/Color.hpp"

void App::Update() {
    switch (m_Phase) {
    case Phase::MAIN_MENU:
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            auto click = Util::Input::GetCursorPosition();
            auto pos = m_StartMenuButton->m_Transform.translation;
            auto size = m_StartMenuButton->GetScaledSize();
            if (click.x >= pos.x - size.x / 2 && click.x <= pos.x + size.x / 2 &&
                click.y >= pos.y - size.y / 2 && click.y <= pos.y + size.y / 2) {
                m_Phase = Phase::MAIN_MENU_FLASH;
                m_MainMenuFlashTimer = 0;
                m_MainMenuFlashCount = 0;
            }
        }
        break;

    case Phase::MAIN_MENU_FLASH:
        ++m_MainMenuFlashTimer;
        if (m_MainMenuFlashTimer % 8 == 0) { 
            ++m_MainMenuFlashCount;
            if (m_MainMenuFlashCount % 2 == 0) {
                m_StartMenuButton->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Menu/options/0.png"));
            } else {
                m_StartMenuButton->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR "/Menu/options/1.png"));
            }
        }
        if (m_MainMenuFlashCount > 6) { // flash 6 times
            InitLevel();
        }
        break;

    case Phase::INTRO_CAMERA:
        UpdateCamera();
        break;

    case Phase::READY_ANIM:
        if (m_ReadyAnim && m_ReadyAnim->IfAnimationEnds()) {
            m_Root.RemoveChild(m_ReadyAnim);
            m_ReadyAnim = nullptr;
            m_Phase = Phase::GAMEPLAY;
            LOG_DEBUG("Entering gameplay");
        }
        break;

    case Phase::GAMEPLAY:
        UpdateGameplay();
        break;

    case Phase::LEVEL_COMPLETE:
        if (!m_EndScreen) {
            m_EndScreen = m_TransitionFadeOut;
            std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen)->Play();
            m_Root.AddChild(m_EndScreen);
            m_EndTimer = 0;
            
            // Note: Since AnimatedCharacter might maintain its previous state, we MUST pause and let it replay upon Play()
        } else {
            m_EndTimer++;
            auto anim = std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen);
            
            // Delay has been removed since images are preloaded
            if (anim) {
                if (anim->IfAnimationEnds()) {
                    m_Root.RemoveChild(m_EndScreen);
                    m_EndScreen = nullptr;

                    if (GetAllLevels()[m_CurrentLevel].rewardPlant != 0) {
                        // 切換到獎勵畫面，清空前一個階段（遊戲階段）的所有物件
                        ClearLevel();

                        // Show reward screen instead of immediate next level
                        m_RewardBackground = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Image>(
                                RESOURCE_DIR "/LevelCompleted/win/win_card.png"),
                            90.0f); // Behind the fade-in overlay
                        m_RewardBackground->m_Transform.translation = {0.0f, 0.0f};
                        m_Root.AddChild(m_RewardBackground);

                        PlantType rewardType =
                            ToPlantType(GetAllLevels()[m_CurrentLevel].rewardPlant);
                        const auto& reward = PlantCatalog::Get(rewardType);
                        
                        m_RewardPlant = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Image>(reward.rewardImage),
                            91.0f);
                        // Position slightly adjusted to visually fit into the green slot of win_card.png
                        m_RewardPlant->m_Transform.translation = {0.0f, 125.0f};
                        
                        if (rewardType == PlantType::Shovel) {
                            m_RewardPlant->m_Transform.scale = {1.5f, 1.5f}; // Adjust the shovel scale here if needed
                        } else {
                            m_RewardPlant->m_Transform.scale = {0.9f, 0.9f};
                        }
                        
                        m_Root.AddChild(m_RewardPlant);

                        m_RewardTextTitle = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Text>(RESOURCE_DIR "/Font/Rye-Regular.ttf", 26, reward.rewardTitle, Util::Color(209, 150, 57)),
                            92.0f);
                        m_RewardTextTitle->m_Transform.translation = {0.0f, -14.0f};
                        m_Root.AddChild(m_RewardTextTitle);

                        m_RewardTextDesc = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Text>(RESOURCE_DIR "/Font/CaveatBrush-Regular.ttf", 26, reward.rewardDescription, Util::Color::FromName(Util::Colors::BLACK)),
                            92.0f);
                        m_RewardTextDesc->m_Transform.translation = {0.0f, -140.0f};
                        m_Root.AddChild(m_RewardTextDesc);

                        m_RewardButton = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Image>(
                                RESOURCE_DIR "/LevelCompleted/win/button.png"),
                            92.0f);
                        m_RewardButton->m_Transform.translation = {0.0f, -285.0f};
                        m_Root.AddChild(m_RewardButton);

                        m_EndScreen = m_TransitionFadeIn;
                        std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen)->Play();
                        m_Root.AddChild(m_EndScreen);
                        m_EndTimer = 0;
                        m_Phase = Phase::REWARD_SCREEN;
                    } else {
                        // Advance to next level directly
                        ClearLevel();
                        m_CurrentLevel++;
                        if (static_cast<size_t>(m_CurrentLevel) >= GetAllLevels().size()) {
                            m_CurrentState = State::END;
                        } else {
                            InitLevel();

                            m_EndScreen = m_TransitionFadeIn;
                            std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen)->Play();
                            m_Root.AddChild(m_EndScreen);
                            m_EndTimer = 0;
                            m_Phase = Phase::LEVEL_FADE_IN;
                        }
                    }
                }
            }
        }
        break;

    case Phase::REWARD_SCREEN: {
        m_EndTimer++;
        auto anim = std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen);

        if (anim) {
            if (anim->IfAnimationEnds()) {
                // Fade-in finished. Wait for user to click to proceed.
                m_Root.RemoveChild(m_EndScreen);
                m_EndScreen = nullptr;
            }
        } else {
            // Once animation is done, we wait for a click on the button to continue
            if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) && m_RewardButton) {
                auto click = Util::Input::GetCursorPosition();
                auto pos = m_RewardButton->m_Transform.translation;
                auto size = m_RewardButton->GetScaledSize();
                if (click.x >= pos.x - size.x / 2 && click.x <= pos.x + size.x / 2 &&
                    click.y >= pos.y - size.y / 2 && click.y <= pos.y + size.y / 2) {
                    // Advance to next level
                    m_Root.RemoveChild(m_RewardBackground);
                    m_RewardBackground = nullptr;
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

                ClearLevel();
                m_CurrentLevel++;
                if (static_cast<size_t>(m_CurrentLevel) >= GetAllLevels().size()) {
                    m_CurrentState = State::END;
                } else {
                    InitLevel();

                    // To make it smooth, let's just fade-in again for the next level
                    m_EndScreen = m_TransitionFadeIn;
                    std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen)->Play();
                    m_Root.AddChild(m_EndScreen);
                    m_EndTimer = 0;
                    m_Phase = Phase::LEVEL_FADE_IN;
                }
                } // Close click check
            }
        }
        break;
    }

    case Phase::LEVEL_FADE_IN: {
        m_EndTimer++;
        auto anim = std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen);

        // Delay has been removed since images are preloaded

        if (anim) {
            if (anim->IfAnimationEnds()) {
                m_Root.RemoveChild(m_EndScreen);
                m_EndScreen = nullptr;
                m_Phase = Phase::INTRO_CAMERA;
            }
        }
        break;
    }

    case Phase::GAME_OVER:
        ++m_EndTimer;
        if (m_EndTimer >= END_SCREEN_DURATION) {
            m_CurrentState = State::END;
        }
        break;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}
