#include "AnimatedCharacter.hpp"

AnimatedCharacter::AnimatedCharacter(const std::vector<std::string>& paths,
                                     bool play,
                                     std::size_t interval,
                                     bool looping,
                                     std::size_t cooldown)
    : GameObject(
          std::make_shared<Util::Animation>(paths, play, interval, looping,
                                            cooldown),
          0) {}

std::shared_ptr<Util::Animation> AnimatedCharacter::GetAnimation() const {
    return std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
}

bool AnimatedCharacter::IsPlaying() const {
    return GetAnimation()->GetState() == Util::Animation::State::PLAY;
}

bool AnimatedCharacter::IsLooping() const {
    return GetAnimation()->GetLooping();
}

bool AnimatedCharacter::IfAnimationEnds() const {
    return GetAnimation()->GetState() == Util::Animation::State::ENDED;
}

void AnimatedCharacter::Play() {
    GetAnimation()->Play();
}

void AnimatedCharacter::Pause() {
    GetAnimation()->Pause();
}

void AnimatedCharacter::SetLooping(bool looping) {
    GetAnimation()->SetLooping(looping);
}
