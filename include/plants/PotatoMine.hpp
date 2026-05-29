#ifndef POTATO_MINE_HPP
#define POTATO_MINE_HPP

#include "entities/Plant.hpp"
#include "Util/Image.hpp"
#include "entities/Zombie.hpp"

#include <string>
#include <vector>

class PotatoMine : public Plant {
public:
    enum class MineState { IDLE, READY, BOOM, DONE };

    PotatoMine(int row, int col)
        : Plant(MakeFrames("mine", 1), 5, row, col, 300, 100) {}

    MineState GetMineState() const { return m_MineState; }
    bool IsDone() const { return m_MineState == MineState::DONE; }

    void UpdateBehavior(const PlantUpdateContext& context,
                        std::vector<PlantAction>& actions) override {
        Update();

        if (m_MineState == MineState::READY) {
            for (const auto& zombie : context.zombies) {
                if (!zombie->IsAlive() || zombie->GetRow() != m_Row) continue;
                float dx = zombie->GetX() - m_Transform.translation.x;
                if (dx >= -10.0f && dx <= 40.0f) {
                    Explode();
                    actions.push_back(
                        PlantAction::DamageZombie(zombie, EXPLODE_DAMAGE));
                    break;
                }
            }
        }

        if (IsDone()) {
            actions.push_back(PlantAction::RemovePlant(m_Row, m_Col));
        }
    }

    void Update() {
        switch (m_MineState) {
        case MineState::IDLE:
            ++m_ArmedTimer;
            if (m_ArmedTimer >= ARM_DELAY) {
                m_MineState = MineState::READY;
                SwitchAnimation(MakeActionFrames("mine/ready", 8));
            }
            break;
        case MineState::READY:
            break;
        case MineState::BOOM:
            ++m_BoomTimer;
            if (m_BoomTimer >= BOOM_DURATION) {
                m_MineState = MineState::DONE;
                SetVisible(false);
            }
            break;
        case MineState::DONE:
            break;
        }
    }

    void Explode() {
        if (m_MineState != MineState::READY) return;
        m_MineState = MineState::BOOM;
        m_BoomTimer = 0;
        // Show boom image
        m_Drawable = std::make_shared<Util::Image>(
            std::string(RESOURCE_DIR) + "/Plant/mine/boom/image.png");
    }

    static constexpr int EXPLODE_DAMAGE = 1800;

private:
    MineState m_MineState = MineState::IDLE;
    int m_ArmedTimer = 0;
    int m_BoomTimer = 0;
    static constexpr int ARM_DELAY = 300;
    static constexpr int BOOM_DURATION = 30;
};

#endif // POTATO_MINE_HPP
