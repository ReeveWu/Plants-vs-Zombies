#ifndef STATUS_EFFECT_HPP
#define STATUS_EFFECT_HPP

#include <string>

class StatusEffect {
public:
    virtual ~StatusEffect() = default;

    // Returns true if the effect is still active after this tick.
    virtual bool Tick() = 0;

    // Speed multiplier applied while this effect is active (1.0 = normal).
    virtual float GetSpeedMultiplier() const = 0;

    // The sprite variant folder name to use ("idle" = normal, "frozen", etc.).
    virtual std::string GetVariant() const = 0;
};

class FrostEffect : public StatusEffect {
public:
    explicit FrostEffect(int duration = FROST_DURATION)
        : m_Remaining(duration) {}

    bool Tick() override {
        if (m_Remaining > 0) --m_Remaining;
        return m_Remaining > 0;
    }

    float GetSpeedMultiplier() const override { return 0.5f; }
    std::string GetVariant() const override { return "frozen"; }

private:
    static constexpr int FROST_DURATION = 300; // ~5 seconds at 60 fps
    int m_Remaining;
};

#endif // STATUS_EFFECT_HPP
