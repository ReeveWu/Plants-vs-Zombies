#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "AnimatedCharacter.hpp"
#include "GridSystem.hpp"
#include "StatusEffect.hpp"
#include "Util/Animation.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Four frame lists for one (phase × variant) cell.
struct VariantFrames {
    std::vector<std::string> walkFrames;
    std::vector<std::string> eatFrames;
    std::vector<std::string> walkHitFrames;
    std::vector<std::string> eatHitFrames;
};

// Represents one visual stage of a zombie's armor/appearance.
// Phases are ordered strongest → weakest; the last phase (hpThreshold=0)
// is the "naked" zombie look.
// Each phase carries multiple variants keyed by name ("idle", "frozen", …).
struct ArmorPhase {
    int hpThreshold;  // Phase active while HP > hpThreshold
    std::unordered_map<std::string, VariantFrames> variants;

    // Convenience: look up a variant, fall back to "idle" if missing.
    const VariantFrames& GetFrames(const std::string& variant) const {
        auto it = variants.find(variant);
        if (it != variants.end()) return it->second;
        return variants.at("idle");
    }
};

class Zombie : public AnimatedCharacter {
public:
    enum class State { WALK, EAT, DIE };

    Zombie(std::vector<ArmorPhase> phases,
           int row, float speed, int hp, int eatDamage, int interval = 45)
        : AnimatedCharacter(
              phases[0].GetFrames("idle").walkFrames, true, interval, true, 100),
          m_Phases(std::move(phases)),
          m_Row(row), m_BaseSpeed(speed), m_Speed(speed),
          m_HP(hp), m_EatDamage(eatDamage), m_AnimInterval(interval) {
        m_Transform.scale = {0.9f, 0.9f};
    }

    virtual ~Zombie() = default;

    // --- Effect system ---------------------------------------------------

    void ApplyEffect(std::unique_ptr<StatusEffect> effect) {
        // Check if same variant already active: refresh instead of stack
        std::string newVar = effect->GetVariant();
        for (auto& e : m_Effects) {
            if (e->GetVariant() == newVar) {
                e = std::move(effect); // replace / refresh
                RefreshEffectState();
                return;
            }
        }
        m_Effects.push_back(std::move(effect));
        RefreshEffectState();
    }

    // --- Per-frame update ------------------------------------------------

    void Update() {
        // Tick effects & remove expired ones
        bool effectsChanged = false;
        m_Effects.erase(
            std::remove_if(m_Effects.begin(), m_Effects.end(),
                [&](std::unique_ptr<StatusEffect>& e) {
                    if (!e->Tick()) { effectsChanged = true; return true; }
                    return false;
                }),
            m_Effects.end());
        if (effectsChanged) RefreshEffectState();

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
                const auto& vf = CurrentFrames();
                if (m_State == State::WALK) {
                    SwitchAnimation(vf.walkFrames);
                } else if (m_State == State::EAT) {
                    SwitchAnimation(vf.eatFrames);
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
        const auto& vf = CurrentFrames();
        if (m_State == State::WALK) {
            SwitchAnimation(vf.walkHitFrames);
        } else if (m_State == State::EAT) {
            SwitchAnimation(vf.eatHitFrames);
        }
    }

    void StartEat() {
        if (m_State != State::EAT) {
            m_State = State::EAT;
            m_EatTimer = 0;
            m_HitFlashTimer = 0;
            SwitchAnimation(CurrentFrames().eatFrames);
        }
    }

    void StartWalk() {
        if (m_State != State::WALK) {
            m_State = State::WALK;
            m_HitFlashTimer = 0;
            SwitchAnimation(CurrentFrames().walkFrames);
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
            frames, true, m_AnimInterval, true, 100);
        if (idx < frames.size()) {
            newAnim->SetCurrentFrame(idx);
        }
        m_Drawable = newAnim;
    }

    // Returns the VariantFrames for the current phase + active variant.
    const VariantFrames& CurrentFrames() const {
        return m_Phases[m_PhaseIndex].GetFrames(m_ActiveVariant);
    }

    // Recompute speed multiplier and variant from active effects.
    void RefreshEffectState() {
        float mult = 1.0f;
        std::string variant = "idle";
        for (const auto& e : m_Effects) {
            mult *= e->GetSpeedMultiplier();
            // Last non-idle variant wins (could add priority later).
            std::string v = e->GetVariant();
            if (v != "idle") variant = v;
        }
        m_Speed = m_BaseSpeed * mult;

        // If variant changed, switch animation
        if (variant != m_ActiveVariant) {
            m_ActiveVariant = variant;
            const auto& vf = CurrentFrames();
            if (m_HitFlashTimer > 0) {
                if (m_State == State::WALK) SwitchAnimation(vf.walkHitFrames);
                else if (m_State == State::EAT) SwitchAnimation(vf.eatHitFrames);
            } else {
                if (m_State == State::WALK) SwitchAnimation(vf.walkFrames);
                else if (m_State == State::EAT) SwitchAnimation(vf.eatFrames);
            }
        }
    }

    std::vector<ArmorPhase> m_Phases;
    std::size_t m_PhaseIndex = 0;
    int m_Row;
    float m_BaseSpeed;
    float m_Speed;
    int m_HP;
    int m_EatDamage;
    int m_AnimInterval;
    State m_State = State::WALK;
    int m_EatTimer = 0;
    int m_HitFlashTimer = 0;
    std::string m_ActiveVariant = "idle";
    std::vector<std::unique_ptr<StatusEffect>> m_Effects;
    static constexpr int EAT_INTERVAL = 30;
    static constexpr int HIT_FLASH_DURATION = 8;
};

#endif // ZOMBIE_HPP
