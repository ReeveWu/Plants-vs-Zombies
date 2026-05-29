#ifndef CHOMPER_HPP
#define CHOMPER_HPP

#include "entities/Plant.hpp"
#include "entities/Zombie.hpp"

#include <string>
#include <vector>

class Chomper : public Plant {
public:
    enum class ChompState { IDLE, BITE, CHEW };

    Chomper(int row, int col)
        : Plant(MakeFrames("chomper", 13), 7, row, col, 300, 100) {}

    ChompState GetChompState() const { return m_ChompState; }

    void UpdateBehavior(const PlantUpdateContext& context,
                        std::vector<PlantAction>& actions) override {
        Update();

        if (!CanChomp()) return;

        std::shared_ptr<Zombie> target;
        float bestDist = 1e9f;
        float plantX = m_Transform.translation.x;
        for (const auto& zombie : context.zombies) {
            if (!zombie->IsAlive() || zombie->GetRow() != m_Row) continue;
            if (!zombie->IsEating()) continue;
            float dx = zombie->GetX() - plantX;
            if (dx >= -10.0f && dx < bestDist) {
                bestDist = dx;
                target = zombie;
            }
        }

        if (target) {
            StartBite();
            actions.push_back(PlantAction::EatZombie(target));
        }
    }

    void Update() {
        switch (m_ChompState) {
        case ChompState::IDLE:
            break;
        case ChompState::BITE:
            if (IfAnimationEnds()) {
                m_ChompState = ChompState::CHEW;
                m_ChewTimer = 0;
                SwitchAnimation(MakeActionFrames("chomper/chew", 6));
            }
            break;
        case ChompState::CHEW:
            ++m_ChewTimer;
            if (m_ChewTimer >= CHEW_DURATION) {
                m_ChompState = ChompState::IDLE;
                SwitchAnimation(MakeFrames("chomper", 13));
            }
            break;
        }
    }

    // Returns true if chomper can attack right now
    bool CanChomp() const {
        return m_ChompState == ChompState::IDLE;
    }

    // Start bite animation — caller should kill the zombie
    void StartBite() {
        if (m_ChompState != ChompState::IDLE) return;
        m_ChompState = ChompState::BITE;
        // Non-looping bite animation
        auto biteFrames = MakeActionFrames("chomper/bite", 9);
        auto anim = std::make_shared<Util::Animation>(
            biteFrames, false, 80, false, 0);
        m_Drawable = anim;
        Play();
    }

    static constexpr int CHOMP_DAMAGE = 9999;
    static constexpr float CHOMP_RANGE = 50.0f;

private:
    ChompState m_ChompState = ChompState::IDLE;
    int m_ChewTimer = 0;
    static constexpr int CHEW_DURATION = 300;
};

#endif // CHOMPER_HPP
