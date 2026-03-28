#ifndef POTATO_MINE_HPP
#define POTATO_MINE_HPP

#include "Plant.hpp"
#include "Util/Image.hpp"

#include <string>
#include <vector>

class PotatoMine : public Plant {
public:
    enum class MineState { IDLE, READY, BOOM, DONE };

    PotatoMine(int row, int col)
        : Plant(MakeFrames("mine", 1), 5, row, col, 300, 100) {}

    MineState GetMineState() const { return m_MineState; }
    bool IsDone() const { return m_MineState == MineState::DONE; }

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
