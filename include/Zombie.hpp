#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "AnimatedCharacter.hpp"
#include "GridSystem.hpp"

#include <string>
#include <vector>

class Zombie : public AnimatedCharacter {
public:
    enum class State { WALK, EAT, DIE };

    Zombie(const std::vector<std::string>& walkFrames,
           int row, float speed, int hp)
        : AnimatedCharacter(walkFrames, true, 100, true, 100),
          m_Row(row), m_Speed(speed), m_HP(hp) {
        m_Transform.scale = {0.9f, 0.9f};
    }

    virtual ~Zombie() = default;

    void Update() {
        if (m_State == State::WALK) {
            m_Transform.translation.x -= m_Speed;
        }
    }

    bool IsAlive() const { return m_State != State::DIE; }
    bool IsPastLine(float x) const { return m_Transform.translation.x < x; }

    int GetRow() const { return m_Row; }
    float GetX() const { return m_Transform.translation.x; }
    State GetState() const { return m_State; }

protected:
    int m_Row;
    float m_Speed;
    int m_HP;
    State m_State = State::WALK;
};

#endif // ZOMBIE_HPP
