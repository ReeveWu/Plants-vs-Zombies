#ifndef AUDIO_SYSTEM_HPP
#define AUDIO_SYSTEM_HPP

#include <map>
#include <memory>

namespace Util {
class BGM;
class SFX;
} // namespace Util

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    void Initialize();

    void PlayMenuMusic();
    void PlayGameplayMusic();

    void PlayCherryBoom();
    void PlayZombieBoom();
    void PlayZombieHit(int sourcePlantType);
    void PlayZombieDead();
    void PlayZombieChew();
    void PlaySunCollect();
    void PlayZombiesAreComing();
    void PlayGameClick();
    void PlayButtonClick();
    void PlayLawnMower();
    void PlayMoneyFalls();
    void PlayPlacePlant();
    void PlayWin();
    void PlayLose();

private:
    enum class MusicTrack {
        Menu,
        Gameplay,
    };

    enum class SoundCue {
        CherryBoom,
        ZombieBoom,
        ZombieHit1,
        ZombieHit2,
        ZombieHit3,
        ZombieDead,
        ZombieChew,
        SunCollect,
        ZombiesAreComing,
        GameClick,
        ButtonClick,
        LawnMower,
        MoneyFalls,
        PlacePlant,
        Win,
        Lose,
    };

    void LoadMusic(MusicTrack track, const char* path, int volume);
    void LoadSound(SoundCue cue, const char* path, int volume);
    void PlayMusic(MusicTrack track);
    void PlaySound(SoundCue cue);
    void FadeOutMusic(MusicTrack track, int milliseconds);
    SoundCue GetZombieHitCue(int sourcePlantType) const;

    static constexpr int SFX_CHANNEL_COUNT = 48;

    bool m_Initialized = false;

    std::map<MusicTrack, std::unique_ptr<Util::BGM>> m_Music;
    std::map<SoundCue, std::unique_ptr<Util::SFX>> m_Sounds;
    std::map<int, SoundCue> m_ZombieHitCueByPlantType;
};

#endif // AUDIO_SYSTEM_HPP
