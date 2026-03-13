#ifndef ANIMATED_CHARACTER_HPP
#define ANIMATED_CHARACTER_HPP

#include <string>
#include <vector>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"

class AnimatedCharacter : public Util::GameObject {
public:
    explicit AnimatedCharacter(const std::vector<std::string>& paths,
                               bool play = false,
                               std::size_t interval = 100,
                               bool looping = true,
                               std::size_t cooldown = 100);

    [[nodiscard]] bool IsPlaying() const;
    [[nodiscard]] bool IsLooping() const;
    [[nodiscard]] bool IfAnimationEnds() const;

    void Play();
    void Pause();
    void SetLooping(bool looping);

private:
    [[nodiscard]] std::shared_ptr<Util::Animation> GetAnimation() const;
};

#endif // ANIMATED_CHARACTER_HPP
