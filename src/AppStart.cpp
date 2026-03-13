#include "App.hpp"

#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");

    m_Background = std::make_shared<Background>();
    m_Root.AddChild(m_Background);

    m_CameraOffset = CAMERA_IDLE_OFFSET;
    m_CameraState = CameraState::PAN_RIGHT;
    m_CameraTimer = 0;
    m_Phase = Phase::INTRO_CAMERA;
    m_ReadyAnim = nullptr;

    m_CurrentState = State::UPDATE;
}
