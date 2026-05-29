#ifndef CARD_SLOT_HPP
#define CARD_SLOT_HPP

#include "catalog/PlantCatalog.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

#include <glm/glm.hpp>
#include <string>

class CardSlot : public Util::GameObject {
public:
    CardSlot(PlantType type, const PlantDefinition& definition,
             const glm::vec2& pos)
        : m_Definition(definition), m_Type(type) {
        m_NormalImage = std::make_shared<Util::Image>(definition.cardImage);
        m_CDImage =
            std::make_shared<Util::Image>(definition.cardCooldownImage);
        SetDrawable(m_NormalImage);
        SetZIndex(60);
        m_Transform.scale = {0.45f, 0.45f};
        m_Transform.translation = pos;
    }

    bool IsAvailable(int currentSun) const {
        return m_Ready && currentSun >= m_Definition.cost;
    }

    int GetCost() const { return m_Definition.cost; }
    int GetIndex() const { return ToPlantTypeId(m_Type); }
    PlantType GetType() const { return m_Type; }
    bool IsReady() const { return m_Ready; }

    void Use() {
        m_Ready = false;
        m_CooldownTimer = m_Definition.cooldownFrames;
        SetDrawable(m_CDImage);
    }

    void UpdateCooldown() {
        if (m_Ready) return;
        if (--m_CooldownTimer <= 0) {
            m_Ready = true;
            SetDrawable(m_NormalImage);
        }
    }

    bool ContainsPoint(const glm::vec2& point) const {
        glm::vec2 pos = m_Transform.translation;
        float hw = 25.0f, hh = 35.0f;
        return point.x > pos.x - hw && point.x < pos.x + hw &&
               point.y > pos.y - hh && point.y < pos.y + hh;
    }

private:
    PlantDefinition m_Definition;
    PlantType m_Type;
    bool m_Ready = true;
    int m_CooldownTimer = 0;
    std::shared_ptr<Util::Image> m_NormalImage;
    std::shared_ptr<Util::Image> m_CDImage;
};

#endif // CARD_SLOT_HPP
