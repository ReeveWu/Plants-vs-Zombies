#ifndef SUN_HPP
#define SUN_HPP

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Sun : public Util::GameObject {
public:
    enum class State { FALLING, IDLE, COLLECTING, DONE };

    /// Sky sun: falls from above screen to a random target position
    explicit Sun(const glm::vec2& target);

    /// Plant sun: emitted near a sunflower, drops slightly
    Sun(const glm::vec2& plantPos, bool fromPlant);

    void Update();
    bool TryCollect(const glm::vec2& clickPos);

    State GetState() const { return m_State; }

    static constexpr int VALUE = 25;

private:
    State m_State = State::FALLING;
    glm::vec2 m_FallTarget;
    glm::vec2 m_CollectTarget;

    static constexpr float FALL_SPEED = 1.5f;
    static constexpr float COLLECT_EASE = 0.08f;
    static constexpr float CLICK_RADIUS = 35.0f;

    static std::vector<std::string> GetFramePaths();
};

#endif // SUN_HPP
