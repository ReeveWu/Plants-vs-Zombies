#ifndef ZOMBIE_CORPSE_HPP
#define ZOMBIE_CORPSE_HPP

#include "AnimatedCharacter.hpp"

#include <string>
#include <vector>

class ZombieCorpse : public AnimatedCharacter {
public:
    ZombieCorpse(const std::vector<std::string>& frames,
                 glm::vec2 position, glm::vec2 scale = {0.9f, 0.9f})
        : AnimatedCharacter(frames, false, 100, false, 0) {
        m_Transform.translation = position;
        m_Transform.scale = scale;
        SetVisible(true);
        Play();
    }

    bool IsFinished() const { return IfAnimationEnds(); }
};

#endif // ZOMBIE_CORPSE_HPP
