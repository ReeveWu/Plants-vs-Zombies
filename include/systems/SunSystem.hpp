#ifndef SUN_SYSTEM_HPP
#define SUN_SYSTEM_HPP

#include "entities/Sun.hpp"
#include "ui/SunCounter.hpp"
#include "Util/Renderer.hpp"

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

class SunSystem {
public:
    void Reset(int initialAmount, std::shared_ptr<SunCounter> counter);
    void Update(Util::Renderer& root);
    bool TryCollect(const glm::vec2& click);
    void SpawnPlantSun(const glm::vec2& plantPosition, Util::Renderer& root);
    void Clear(Util::Renderer& root);

    int GetAmount() const { return m_Amount; }
    void Spend(int amount);
    void SetUnlimited(bool enabled);
    bool IsUnlimited() const { return m_Unlimited; }

private:
    void SpawnSkySun(Util::Renderer& root);
    void SyncCounter();

    std::vector<std::shared_ptr<Sun>> m_Suns;
    std::shared_ptr<SunCounter> m_Counter;
    int m_Amount = 50;
    int m_AmountBeforeUnlimited = 50;
    int m_SpawnTimer = 0;
    bool m_Unlimited = false;

    static constexpr int SKY_SPAWN_INTERVAL = 300;
    static constexpr int UNLIMITED_AMOUNT = 9999;
};

#endif // SUN_SYSTEM_HPP
