#include "Sun.hpp"

#include <cstdlib>
#include <glm/gtx/compatibility.hpp>

std::vector<std::string> Sun::GetFramePaths() {
    std::vector<std::string> paths;
    paths.reserve(12);
    for (int i = 0; i < 12; ++i) {
        paths.push_back(std::string(RESOURCE_DIR) +
                        "/Plant/sun/sun_" + std::to_string(i) + ".png");
    }
    return paths;
}

Sun::Sun(const glm::vec2& target)
    : m_FallTarget(target),
      m_CollectTarget({-560.0f, 310.0f}) {
    auto anim = std::make_shared<Util::Animation>(
        GetFramePaths(), true, 100, true, 0);
    SetDrawable(anim);
    SetZIndex(100);
    m_Transform.scale = {0.8f, 0.8f};
    m_Transform.translation = {target.x, 400.0f};
    m_State = State::FALLING;
}

Sun::Sun(const glm::vec2& plantPos, bool)
    : m_FallTarget({plantPos.x + static_cast<float>(rand() % 31 - 15),
                    plantPos.y - 30.0f}),
      m_CollectTarget({-560.0f, 310.0f}) {
    auto anim = std::make_shared<Util::Animation>(
        GetFramePaths(), true, 100, true, 0);
    SetDrawable(anim);
    SetZIndex(100);
    m_Transform.scale = {0.8f, 0.8f};
    m_Transform.translation = {plantPos.x, plantPos.y + 20.0f};
    m_State = State::FALLING;
}

void Sun::Update() {
    glm::vec2 pos = m_Transform.translation;

    switch (m_State) {
    case State::FALLING: {
        glm::vec2 dir = m_FallTarget - pos;
        float dist = glm::length(dir);
        if (dist < 1.0f) {
            m_Transform.translation = m_FallTarget;
            m_State = State::IDLE;
        } else {
            m_Transform.translation += glm::normalize(dir) * FALL_SPEED;
        }
        break;
    }
    case State::IDLE:
        break;
    case State::COLLECTING:
        m_Transform.translation = glm::lerp(pos, m_CollectTarget, COLLECT_EASE);
        if (glm::distance(m_Transform.translation, m_CollectTarget) < 3.0f) {
            m_State = State::DONE;
        }
        break;
    case State::DONE:
        break;
    }
}

bool Sun::TryCollect(const glm::vec2& clickPos) {
    if (m_State != State::FALLING && m_State != State::IDLE) return false;
    if (glm::distance(m_Transform.translation, clickPos) <= CLICK_RADIUS) {
        m_State = State::COLLECTING;
        return true;
    }
    return false;
}
