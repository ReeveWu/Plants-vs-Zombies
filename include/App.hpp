#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"

#include "AnimatedCharacter.hpp"
#include "Background.hpp"

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
        INTRO_CAMERA,
        READY_ANIM,
        GAMEPLAY,
        LEVEL_COMPLETE,
    };

    enum class CameraState {
        IDLE,
        PAN_RIGHT,
        PAN_PAUSE,
        PAN_LEFT,
    };

    void UpdateCamera();

private:
    State m_CurrentState = State::START;
    Phase m_Phase = Phase::INTRO_CAMERA;
    CameraState m_CameraState = CameraState::PAN_RIGHT;

    Util::Renderer m_Root;

    // Game objects
    std::shared_ptr<Background> m_Background;
    std::shared_ptr<AnimatedCharacter> m_ReadyAnim;

    // Camera system
    // bg.x > 0 → background shifts right → shows more house (left side)
    // bg.x < 0 → background shifts left  → shows more road  (right side)
    // Safe range for 1280x720 window with 1680px bg: [-200, +200]
    float m_CameraOffset = 0.0f;
    int m_CameraTimer = 0;

    static constexpr float CAMERA_SPEED = 3.0f;
    static constexpr float CAMERA_IDLE_OFFSET = 180.0f;  // play position (house visible)
    static constexpr float CAMERA_PAN_TARGET = -150.0f;  // road-preview position
    static constexpr int CAMERA_PAUSE_FRAMES = 60;
};

#endif
