#ifndef CARD_SLOT_HPP
#define CARD_SLOT_HPP

#include "catalog/PlantCatalog.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class CardSlot : public Util::GameObject {
public:
    CardSlot(PlantType type, const PlantDefinition& definition,
             const glm::vec2& pos)
        : m_Definition(definition), m_Type(type) {
        m_NormalImage = std::make_shared<Util::Image>(definition.cardImage);
        m_CDImage =
            std::make_shared<Util::Image>(definition.cardCooldownImage);
        LoadCooldownImages();
        SetDrawable(m_NormalImage);
        SetZIndex(60);
        m_Transform.scale = {0.45f, 0.45f};
        m_Transform.translation = pos;
    }

    bool IsAvailable(int currentSun) const {
        return (m_NoCooldown || m_Ready) && currentSun >= m_Definition.cost;
    }

    int GetCost() const { return m_Definition.cost; }
    int GetIndex() const { return ToPlantTypeId(m_Type); }
    PlantType GetType() const { return m_Type; }
    bool IsReady() const { return m_Ready; }

    void Use() {
        if (m_NoCooldown) {
            m_Ready = true;
            m_CooldownTimer = 0;
            SetDrawable(m_NormalImage);
            return;
        }
        m_Ready = false;
        m_CooldownTimer = m_Definition.cooldownFrames;
        m_TotalCooldownTimer = m_Definition.cooldownFrames;
        SetCooldownDrawable();
    }

    void UpdateCooldown() {
        if (m_Ready) return;
        --m_CooldownTimer;
        if (m_CooldownTimer <= 0) {
            m_Ready = true;
            m_TotalCooldownTimer = 0;
            SetDrawable(m_NormalImage);
            return;
        }
        SetCooldownDrawable();
    }

    void SetNoCooldown(bool enabled) {
        m_NoCooldown = enabled;
        if (!m_NoCooldown) return;

        m_Ready = true;
        m_CooldownTimer = 0;
        m_TotalCooldownTimer = 0;
        SetDrawable(m_NormalImage);
    }

    bool ContainsPoint(const glm::vec2& point) const {
        glm::vec2 pos = m_Transform.translation;
        float hw = 25.0f, hh = 35.0f;
        return point.x > pos.x - hw && point.x < pos.x + hw &&
               point.y > pos.y - hh && point.y < pos.y + hh;
    }

private:
    void LoadCooldownImages() {
        if (!m_Definition.isPlant || m_Definition.idleDirectory.empty()) {
            return;
        }

        m_CooldownImages.reserve(COOLDOWN_FRAME_COUNT);
        const std::string dir = std::string(RESOURCE_DIR) + "/Card/cooldown/" +
                                m_Definition.idleDirectory + "/";
        for (int i = 0; i < COOLDOWN_FRAME_COUNT; ++i) {
            const std::string prefix = i < 10 ? "0" : "";
            m_CooldownImages.push_back(
                std::make_shared<Util::Image>(
                    dir + prefix + std::to_string(i) + ".png"));
        }
    }

    void SetCooldownDrawable() {
        if (m_CooldownImages.empty() || m_TotalCooldownTimer <= 0) {
            SetDrawable(m_CDImage);
            return;
        }

        const int elapsed = m_TotalCooldownTimer - m_CooldownTimer;
        int index = elapsed * (COOLDOWN_FRAME_COUNT - 1) / m_TotalCooldownTimer;
        if (index < 0) index = 0;
        if (index >= COOLDOWN_FRAME_COUNT) index = COOLDOWN_FRAME_COUNT - 1;
        SetDrawable(m_CooldownImages[static_cast<std::size_t>(index)]);
    }

    PlantDefinition m_Definition;
    PlantType m_Type;
    bool m_Ready = true;
    bool m_NoCooldown = false;
    int m_CooldownTimer = 0;
    int m_TotalCooldownTimer = 0;
    std::shared_ptr<Util::Image> m_NormalImage;
    std::shared_ptr<Util::Image> m_CDImage;
    std::vector<std::shared_ptr<Util::Image>> m_CooldownImages;

    static constexpr int COOLDOWN_FRAME_COUNT = 60;
};

#endif // CARD_SLOT_HPP
