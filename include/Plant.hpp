#ifndef PLANT_HPP
#define PLANT_HPP

#include "AnimatedCharacter.hpp"

#include <string>
#include <vector>

class Plant : public AnimatedCharacter {
public:
    static constexpr int DEFAULT_HP = 300;

    // Legacy constructor for non-refactored plants (cherrybomb, mine, chomper)
    Plant(int typeIndex, int row, int col)
        : AnimatedCharacter(GetFramePaths(typeIndex), true, 100, true, 100),
          m_TypeIndex(typeIndex), m_Row(row), m_Col(col) {
        m_Transform.scale = {0.9f, 0.9f};
    }

    virtual ~Plant() = default;

    int GetTypeIndex() const { return m_TypeIndex; }
    int GetRow() const { return m_Row; }
    int GetCol() const { return m_Col; }
    int GetHP() const { return m_HP; }
    bool IsDead() const { return m_HP <= 0; }

    void Hurt(int damage = 1) {
        m_HP -= damage;
        OnHurt();
    }

    virtual bool CanShoot() const { return false; }
    virtual bool TryProduceSun() { return false; }
    virtual bool IsIce() const { return false; }
    virtual int GetDamage() const { return 0; }

    virtual bool TryShoot() {
        if (!CanShoot()) return false;
        ++m_ShootTimer;
        if (m_ShootTimer >= GetShootCooldown()) {
            m_ShootTimer = 0;
            return true;
        }
        return false;
    }

    // Legacy static helper (used by non-refactored plant types)
    static std::vector<std::string> GetFramePaths(int typeIndex) {
        struct AnimData {
            const char* dir;
            int frameCount;
        };

        static constexpr AnimData DATA[] = {
            {"peashooter", 24},
            {"sunflower",  24},
            {"cherrybomb", 14},
            {"wallnut",    32},
            {"mine",       8},
            {"iceshooter", 15},
            {"chomper",    13},
            {"fastshooter", 15},
        };

        const auto& d = DATA[typeIndex - 1];
        return MakeFrames(d.dir, d.frameCount);
    }

    static std::vector<std::string> MakeFrames(
        const std::string& plantDir, int count) {
        std::vector<std::string> paths;
        paths.reserve(count);
        for (int i = 0; i < count; ++i) {
            paths.push_back(
                std::string(RESOURCE_DIR "/Plant/") + plantDir + "/idle/"
                + std::to_string(i) + ".png");
        }
        return paths;
    }

protected:
    // Subclass constructor
    Plant(const std::vector<std::string>& frames, int typeIndex,
          int row, int col, int hp, std::size_t interval = 100)
        : AnimatedCharacter(frames, true, interval, true, 100),
          m_TypeIndex(typeIndex), m_Row(row), m_Col(col), m_HP(hp) {
        m_Transform.scale = {0.9f, 0.9f};
    }

    virtual int GetShootCooldown() const { return 90; }

    virtual void OnHurt() {}

    void SwitchAnimation(const std::vector<std::string>& frames,
                         std::size_t interval = 100) {
        std::size_t idx = 0;
        if (auto anim =
                std::dynamic_pointer_cast<Util::Animation>(m_Drawable)) {
            idx = anim->GetCurrentFrameIndex();
        }
        auto newAnim = std::make_shared<Util::Animation>(
            frames, true, interval, true, 100);
        if (idx < frames.size()) {
            newAnim->SetCurrentFrame(idx);
        }
        m_Drawable = newAnim;
    }

    int m_TypeIndex;
    int m_Row;
    int m_Col;
    int m_HP = DEFAULT_HP;
    int m_ShootTimer = 0;
};

#endif // PLANT_HPP
