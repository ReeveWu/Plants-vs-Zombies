#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Update() {
    switch (m_Phase) {
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
        // TODO: level completion logic
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
