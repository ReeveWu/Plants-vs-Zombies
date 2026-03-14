#ifndef PLANT_HPP
#define PLANT_HPP

#include "AnimatedCharacter.hpp"

#include <string>
#include <vector>

class Plant : public AnimatedCharacter {
public:
    static constexpr int DEFAULT_HP = 300;
    static constexpr int PEASHOOTER_TYPE = 1;
    static constexpr int ICESHOOTER_TYPE = 6;
    static constexpr int FASTSHOOTER_TYPE = 8;

    Plant(int typeIndex, int row, int col)
        : AnimatedCharacter(GetFramePaths(typeIndex), true, 100, true, 100),
          m_TypeIndex(typeIndex), m_Row(row), m_Col(col) {
        m_Transform.scale = {0.9f, 0.9f};
    }

    int GetTypeIndex() const { return m_TypeIndex; }
    int GetRow() const { return m_Row; }
    int GetCol() const { return m_Col; }
    int GetHP() const { return m_HP; }
    bool IsDead() const { return m_HP <= 0; }

    void Hurt(int damage = 1) { m_HP -= damage; }

    bool CanShoot() const {
        return (m_TypeIndex == PEASHOOTER_TYPE
             || m_TypeIndex == ICESHOOTER_TYPE
             || m_TypeIndex == FASTSHOOTER_TYPE);
    }

    bool TryShoot() {
        if (!CanShoot()) return false;
        ++m_ShootTimer;
        if (m_ShootTimer >= SHOOT_COOLDOWN) {
            m_ShootTimer = 0;
            return true;
        }
        return false;
    }

    static std::vector<std::string> GetFramePaths(int typeIndex) {
        struct AnimData {
            const char* dir;
            int frameCount;
        };

        static constexpr AnimData DATA[] = {
            {"Peashooter", 24},
            {"sunflower",  24},
            {"cherrybomb", 14},
            {"wallnut",    32},
            {"mine",       8},
            {"iceshooter", 15},
            {"chomper",    13},
            {"fastshooter", 15},
        };

        const auto& d = DATA[typeIndex - 1];
        std::vector<std::string> paths;
        paths.reserve(d.frameCount);
        for (int i = 0; i < d.frameCount; ++i) {
            paths.push_back(std::string(RESOURCE_DIR "/Plant/") + d.dir + "/"
                            + std::to_string(i) + ".png");
        }
        return paths;
    }

private:
    int m_TypeIndex;
    int m_Row;
    int m_Col;
    int m_HP = DEFAULT_HP;
    int m_ShootTimer = 0;
    static constexpr int SHOOT_COOLDOWN = 90;
};

#endif // PLANT_HPP
