#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "AnimatedCharacter.hpp"
#include "GridSystem.hpp"
#include "Util/Animation.hpp"

#include <string>
#include <vector>

class Zombie : public AnimatedCharacter {
public:
    enum class State { WALK, EAT, DIE };

    Zombie(const std::vector<std::string>& walkFrames,
           const std::vector<std::string>& eatFrames,
           const std::vector<std::string>& walkHitFrames,
           const std::vector<std::string>& eatHitFrames,
           int row, float speed, int hp)
        : AnimatedCharacter(walkFrames, true, 100, true, 100),
          m_WalkFrames(walkFrames), m_EatFrames(eatFrames),
          m_WalkHitFrames(walkHitFrames), m_EatHitFrames(eatHitFrames),
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
        // Hit flash
        if (m_HitFlashTimer > 0) {
            --m_HitFlashTimer;
            if (m_HitFlashTimer == 0) {
                if (m_State == State::WALK) {
                    SwitchAnimation(m_WalkFrames);
                } else if (m_State == State::EAT) {
                    SwitchAnimation(m_EatFrames);
                }
            }
        }
    }

    void TakeDamage(int damage) {
        if (m_State == State::DIE) return;
        m_HP -= damage;
        if (m_HP <= 0) {
            m_State = State::DIE;
            SetVisible(false);
        } else {
            m_HitFlashTimer = HIT_FLASH_DURATION;
            if (m_State == State::WALK) {
                SwitchAnimation(m_WalkHitFrames);
            } else if (m_State == State::EAT) {
                SwitchAnimation(m_EatHitFrames);
            }
        }
    }

    void StartEat() {
        if (m_State != State::EAT) {
            m_State = State::EAT;
            m_EatTimer = 0;
            m_HitFlashTimer = 0;
            SwitchAnimation(m_EatFrames);
        }
    }

    void StartWalk() {
        if (m_State != State::WALK) {
            m_State = State::WALK;
            m_HitFlashTimer = 0;
            SwitchAnimation(m_WalkFrames);
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

    virtual std::vector<std::string> GetDieBodyFrames() const = 0;
    virtual std::vector<std::string> GetDieHeadFrames() const = 0;

protected:
    void SwitchAnimation(const std::vector<std::string>& frames) {
        std::size_t idx = 0;
        if (auto anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable)) {
            idx = anim->GetCurrentFrameIndex();
        }
        auto newAnim = std::make_shared<Util::Animation>(
            frames, true, 100, true, 100);
        if (idx < frames.size()) {
            newAnim->SetCurrentFrame(idx);
        }
        m_Drawable = newAnim;
    }

    std::vector<std::string> m_WalkFrames;
    std::vector<std::string> m_EatFrames;
    std::vector<std::string> m_WalkHitFrames;
    std::vector<std::string> m_EatHitFrames;
    int m_Row;
    float m_Speed;
    int m_HP;
    State m_State = State::WALK;
    int m_EatTimer = 0;
    int m_HitFlashTimer = 0;
    static constexpr int EAT_INTERVAL = 30;
    static constexpr int HIT_FLASH_DURATION = 8;
};

#endif // ZOMBIE_HPP
