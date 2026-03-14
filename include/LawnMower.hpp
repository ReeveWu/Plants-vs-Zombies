#ifndef LAWN_MOWER_HPP
#define LAWN_MOWER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class LawnMower : public Util::GameObject {
public:
    enum class State { IDLE, MOVING };

    explicit LawnMower(const glm::vec2& pos)
        : GameObject(
              std::make_unique<Util::Image>(RESOURCE_DIR "/Background/car.png"),
              5.0f) {
        m_Transform.translation = pos;
    }

    void Trigger() { m_State = State::MOVING; }

    void Update() {
        if (m_State == State::MOVING) {
            m_Transform.translation.x += SPEED;
        }
    }

    bool IsOffScreen() const { return m_Transform.translation.x > 700.0f; }
    State GetState() const { return m_State; }

    bool HitCheck(const glm::vec2& zombiePos) const {
        if (m_State != State::MOVING) return false;
        float dx = std::abs(m_Transform.translation.x - zombiePos.x);
        float dy = std::abs(m_Transform.translation.y - zombiePos.y);
        return dx < 30.0f && dy < 30.0f;
    }

private:
    static constexpr float SPEED = 4.0f;
    State m_State = State::IDLE;
};

#endif // LAWN_MOWER_HPP
