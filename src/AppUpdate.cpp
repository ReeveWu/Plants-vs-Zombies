#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "Util/Image.hpp"

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
        InitLawnMowers();
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
            std::vector<std::string> transitionPaths;
            for (int i = 0; i <= 99; ++i) {
                transitionPaths.push_back(RESOURCE_DIR "/LevelCompleted/win/transition/" + std::to_string(i) + ".png");
            }
            
            m_EndScreen = std::make_shared<AnimatedCharacter>(
                transitionPaths,
                false, // play = false (pause initially to wait out the loading spike)
                10,   // interval = 10ms
                false // looping = false
            );
            m_EndScreen->SetZIndex(100.0f);
            m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
            m_Root.AddChild(m_EndScreen);
            m_EndTimer = 0;
        } else {
            m_EndTimer++;
            auto anim = std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen);
            
            // Wait 2 frames for DeltaTime to stabilize after heavy image loading
            if (m_EndTimer <= 2) {
                break;
            }
            
            if (anim) {
                if (!anim->IsPlaying() && !anim->IfAnimationEnds()) {
                    anim->Play();
                } else if (anim->IfAnimationEnds()) {
                    m_Root.RemoveChild(m_EndScreen);
                    m_EndScreen = nullptr;

                    if (m_CurrentLevel >= 14) {
                        // Final level beaten — show victory
                        m_EndScreen = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Image>(
                                RESOURCE_DIR "/LevelCompleted/win/win_card.png"),
                            100.0f);
                        m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
                        m_Root.AddChild(m_EndScreen);
                        m_EndTimer = 0;
                        m_Phase = Phase::GAME_OVER; // reuse for victory display
                    } else {
                        // Advance to next level
                        ClearLevel();
                        m_CurrentLevel++;
                        InitLevel();

                        // Create fade-in overlay (reverse transition: white → transparent)
                        std::vector<std::string> fadeInPaths;
                        for (int i = 99; i >= 0; --i) {
                            fadeInPaths.push_back(
                                RESOURCE_DIR "/LevelCompleted/win/transition/"
                                + std::to_string(i) + ".png");
                        }
                        m_EndScreen = std::make_shared<AnimatedCharacter>(
                            fadeInPaths, false, 10, false);
                        m_EndScreen->SetZIndex(100.0f);
                        m_EndScreen->m_Transform.translation = {0.0f, 0.0f};
                        m_Root.AddChild(m_EndScreen);
                        m_EndTimer = 0;
                        m_Phase = Phase::LEVEL_FADE_IN;
                    }
                }
            }
        }
        break;

    case Phase::LEVEL_FADE_IN: {
        m_EndTimer++;
        auto anim = std::dynamic_pointer_cast<AnimatedCharacter>(m_EndScreen);

        if (m_EndTimer <= 2) break;

        if (anim) {
            if (!anim->IsPlaying() && !anim->IfAnimationEnds()) {
                anim->Play();
            } else if (anim->IfAnimationEnds()) {
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

void App::InitLawnMowers() {
    for (int row = 0; row < GridSystem::ROWS; ++row) {
        if (m_LawnMowers[row]) {
            m_Root.RemoveChild(m_LawnMowers[row]);
            m_LawnMowers[row] = nullptr;
        }
        if (!IsActiveLane(row)) continue;
        float y = GridSystem::CellToPosition(row, 0).y - 20.0f;
        auto mower = std::make_shared<LawnMower>(
            glm::vec2{LAWNMOWER_X + m_CameraOffset, y});
        mower->SetZIndex(5);
        m_LawnMowers[row] = mower;
        m_Root.AddChild(mower);
    }
}
