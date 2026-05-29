#include "app/App.hpp"

#include "Util/Logger.hpp"

void App::UpdateCamera() {
    switch (m_CameraState) {
    case CameraState::PAN_RIGHT:
        m_CameraOffset -= CAMERA_SPEED;
        if (m_CameraOffset <= CAMERA_PAN_TARGET) {
            m_CameraOffset = CAMERA_PAN_TARGET;
            m_CameraState = CameraState::PAN_PAUSE;
            m_CameraTimer = 0;
        }
        break;

    case CameraState::PAN_PAUSE:
        m_CameraTimer++;
        if (m_CameraTimer >= CAMERA_PAUSE_FRAMES) {
            m_CameraState = CameraState::PAN_LEFT;
        }
        break;

    case CameraState::PAN_LEFT:
        m_CameraOffset += CAMERA_SPEED;
        if (m_CameraOffset >= CAMERA_IDLE_OFFSET) {
            m_CameraOffset = CAMERA_IDLE_OFFSET;
            m_CameraState = CameraState::IDLE;

            m_ReadyAnim = std::make_shared<AnimatedCharacter>(
                std::vector<std::string>{
                    RESOURCE_DIR "/Opening/ready.png",
                    RESOURCE_DIR "/Opening/okay.png",
                    RESOURCE_DIR "/Opening/start.png",
                },
                true, 500, false, 0);
            m_ReadyAnim->SetZIndex(100);
            m_ReadyAnim->m_Transform.scale = {0.5f, 0.5f};
            m_Root.AddChild(m_ReadyAnim);
            m_Phase = Phase::READY_ANIM;
            LOG_DEBUG("Camera pan complete, playing Ready animation");
        }
        break;

    case CameraState::IDLE:
        break;
    }

    m_Background->m_Transform.translation.x = m_CameraOffset;
    
    // 同步更新除草機的 X 座標以符合相機偏移
    m_LawnMowerSystem.SyncCamera(m_CameraOffset);
}
