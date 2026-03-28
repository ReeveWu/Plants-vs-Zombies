#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "AnimatedCharacter.hpp"
#include "GridSystem.hpp"
#include "Util/Animation.hpp"

#include <string>
#include <vector>

// Represents one visual stage of a zombie's armor/appearance.
// Phases are ordered strongest → weakest; the last phase (hpThreshold=0)
// is the "naked" zombie look.
struct ArmorPhase {
    int hpThreshold;  // Phase active while HP > hpThreshold
    std::vector<std::string> walkFrames;
    std::vector<std::string> eatFrames;
    std::vector<std::string> walkHitFrames;
    std::vector<std::string> eatHitFrames;
};

class Zombie : public AnimatedCharacter {
public:
    enum class State { WALK, EAT, DIE };

    Zombie(std::vector<ArmorPhase> phases,
           int row, float speed, int hp, int eatDamage)
        : AnimatedCharacter(phases[0].walkFrames, true, 45, true, 100),
          m_Phases(std::move(phases)),
          m_Row(row), m_Speed(speed), m_HP(hp), m_EatDamage(eatDamage) {
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
        // Hit flash: revert to normal variant after flash duration
        if (m_HitFlashTimer > 0) {
            --m_HitFlashTimer;
            if (m_HitFlashTimer == 0) {
                const auto& phase = m_Phases[m_PhaseIndex];
                if (m_State == State::WALK) {
                    SwitchAnimation(phase.walkFrames);
                } else if (m_State == State::EAT) {
                    SwitchAnimation(phase.eatFrames);
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
            return;
        }

        // Check armor phase transition (may skip multiple phases)
        while (m_PhaseIndex + 1 < m_Phases.size()
               && m_HP <= m_Phases[m_PhaseIndex].hpThreshold) {
            ++m_PhaseIndex;
        }

        // Hit flash with current phase's hit variant
        m_HitFlashTimer = HIT_FLASH_DURATION;
        const auto& phase = m_Phases[m_PhaseIndex];
        if (m_State == State::WALK) {
            SwitchAnimation(phase.walkHitFrames);
        } else if (m_State == State::EAT) {
            SwitchAnimation(phase.eatHitFrames);
        }
    }

    void StartEat() {
        if (m_State != State::EAT) {
            m_State = State::EAT;
            m_EatTimer = 0;
            m_HitFlashTimer = 0;
            SwitchAnimation(m_Phases[m_PhaseIndex].eatFrames);
        }
    }

    void StartWalk() {
        if (m_State != State::WALK) {
            m_State = State::WALK;
            m_HitFlashTimer = 0;
            SwitchAnimation(m_Phases[m_PhaseIndex].walkFrames);
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
    int GetEatDamage() const { return m_EatDamage; }

    // Subclass provides these for death animation
    virtual std::vector<std::string> GetDieBodyFrames() const = 0;
    virtual std::vector<std::string> GetDieHeadFrames() const = 0;

    // Utility for subclasses to build frame path lists
    static std::vector<std::string> MakeFrames(
        const std::string& dir, int count) {
        std::vector<std::string> paths;
        paths.reserve(count);
        for (int i = 0; i < count; ++i) {
            paths.push_back(
                RESOURCE_DIR + dir + std::to_string(i) + ".png");
        }
        return paths;
    }

protected:
    void SwitchAnimation(const std::vector<std::string>& frames) {
        std::size_t idx = 0;
        if (auto anim =
                std::dynamic_pointer_cast<Util::Animation>(m_Drawable)) {
            idx = anim->GetCurrentFrameIndex();
        }
        auto newAnim = std::make_shared<Util::Animation>(
            frames, true, 45, true, 100);
        if (idx < frames.size()) {
            newAnim->SetCurrentFrame(idx);
        }
        m_Drawable = newAnim;
    }

    std::vector<ArmorPhase> m_Phases;
    std::size_t m_PhaseIndex = 0;
    int m_Row;
    float m_Speed;
    int m_HP;
    int m_EatDamage;
    State m_State = State::WALK;
    int m_EatTimer = 0;
    int m_HitFlashTimer = 0;
    static constexpr int EAT_INTERVAL = 30;
    static constexpr int HIT_FLASH_DURATION = 8;
};

#endif // ZOMBIE_HPP
