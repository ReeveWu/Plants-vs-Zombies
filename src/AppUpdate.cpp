#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Update() {
    switch (m_Phase) {
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
        // TODO: level completion logic
        break;
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}
