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
        ++m_EndTimer;
        if (m_EndTimer >= END_SCREEN_DURATION) {
            m_CurrentState = State::END;
        }
        break;

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
        }
        float y = GridSystem::CellToPosition(row, 0).y - 20.0f;
        auto mower = std::make_shared<LawnMower>(
            glm::vec2{LAWNMOWER_X + m_CameraOffset, y});
        mower->SetZIndex(5);
        m_LawnMowers[row] = mower;
        m_Root.AddChild(mower);
    }
}
