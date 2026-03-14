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
        if (m_State == State::EAT) {
            ++m_EatTimer;
        }
    }

    void TakeDamage(int damage) {
        m_HP -= damage;
        if (m_HP <= 0) {
            m_State = State::DIE;
            SetVisible(false);
        }
    }

    void StartEat() {
        if (m_State != State::EAT) {
            m_State = State::EAT;
            m_EatTimer = 0;
        }
    }

    void StartWalk() {
        if (m_State != State::WALK) {
            m_State = State::WALK;
        }
    }

    bool ShouldDealEatDamage() {
        if (m_State != State::EAT) return false;
        if (m_EatTimer >= EAT_INTERVAL) {
            m_EatTimer = 0;
            return true;
        }
        return false;
    }

    bool IsAlive() const { return m_State != State::DIE; }
    bool IsDead() const { return m_State == State::DIE; }
    bool IsEating() const { return m_State == State::EAT; }
    bool IsPastLine(float x) const { return m_Transform.translation.x < x; }

    int GetRow() const { return m_Row; }
    float GetX() const { return m_Transform.translation.x; }
    State GetState() const { return m_State; }

protected:
    int m_Row;
    float m_Speed;
    int m_HP;
    State m_State = State::WALK;
    int m_EatTimer = 0;
    static constexpr int EAT_INTERVAL = 30;
};

#endif // ZOMBIE_HPP
