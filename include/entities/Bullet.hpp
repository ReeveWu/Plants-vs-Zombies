#ifndef BULLET_HPP
#define BULLET_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Bullet : public Util::GameObject {
public:
    explicit Bullet(const glm::vec2& startPos, int damage = 20,
                    bool isIce = false)
        : GameObject(
              std::make_unique<Util::Image>(
                  isIce ? RESOURCE_DIR "/Bullet/icepea.png"
                        : RESOURCE_DIR "/Bullet/pea.png"),
              21.0f),
          m_Damage(damage), m_IsIce(isIce) {
        m_Transform.translation = startPos;
        
        static float zOffset = 0.0f;
        SetZIndex(21.0f + zOffset);
        zOffset += 0.001f;
        if (zOffset > 1.0f) zOffset = 0.0f;
    }

    bool IsIce() const { return m_IsIce; }

    void Update() { m_Transform.translation.x += SPEED; }

    bool HitCheck(const glm::vec2& pos) const {
        float dx = pos.x - m_Transform.translation.x;
        float dy = std::abs(pos.y - m_Transform.translation.y);
        return dx > 0.0f && dx < 30.0f && dy < 30.0f;
    }

    bool IsOffScreen() const { return m_Transform.translation.x > 700.0f; }
    bool IsHit() const { return m_Hit; }
    void MarkHit() { m_Hit = true; SetVisible(false); }

    int GetDamage() const { return m_Damage; }
    int GetRow() const { return m_Row; }
    void SetRow(int row) { m_Row = row; }

private:
    static constexpr float SPEED = 4.0f;
    int m_Damage;
    int m_Row = -1;
    bool m_Hit = false;
    bool m_IsIce = false;
};

#endif // BULLET_HPP
