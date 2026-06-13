#include "systems/AudioSystem.hpp"

#include "catalog/PlantCatalog.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"

AudioSystem::AudioSystem() = default;
AudioSystem::~AudioSystem() = default;

void AudioSystem::Initialize() {
    if (m_Initialized) return;

    Mix_AllocateChannels(SFX_CHANNEL_COUNT);

    LoadMusic(MusicTrack::Menu, RESOURCE_DIR "/Sound/menu-bg.mp3", 64);
    LoadMusic(MusicTrack::Gameplay, RESOURCE_DIR "/Sound/play-day-bg.mp3", 64);

    LoadSound(SoundCue::CherryBoom, RESOURCE_DIR "/Sound/cherry-boom.mp3", 64);
    LoadSound(SoundCue::ZombieBoom, RESOURCE_DIR "/Sound/zombie-boom.mp3", 64);
    LoadSound(SoundCue::ZombieHit1, RESOURCE_DIR "/Sound/hit-zombie-1.mp3", 42);
    LoadSound(SoundCue::ZombieHit2, RESOURCE_DIR "/Sound/hit-zombie-2.mp3", 42);
    LoadSound(SoundCue::ZombieHit3, RESOURCE_DIR "/Sound/hit-zombie-3.mp3", 42);
    LoadSound(SoundCue::ZombieDead, RESOURCE_DIR "/Sound/zombie-dead.mp3", 118);
    LoadSound(SoundCue::ZombieChew, RESOURCE_DIR "/Sound/zombie-chew.mp3", 45);
    LoadSound(SoundCue::SunCollect, RESOURCE_DIR "/Sound/sun-collect.mp3", 50);
    LoadSound(SoundCue::ZombiesAreComing,
              RESOURCE_DIR "/Sound/zombies-are-coming.mp3", 128);
    LoadSound(SoundCue::GameClick, RESOURCE_DIR "/Sound/click.mp3", 55);
    LoadSound(SoundCue::ButtonClick, RESOURCE_DIR "/Sound/button-click.mp3",
              60);
    LoadSound(SoundCue::LawnMower, RESOURCE_DIR "/Sound/lawnmower.mp3", 64);
    LoadSound(SoundCue::MoneyFalls, RESOURCE_DIR "/Sound/moneyfalls.mp3", 64);
    LoadSound(SoundCue::PlacePlant, RESOURCE_DIR "/Sound/place-plant.mp3", 64);
    LoadSound(SoundCue::Win, RESOURCE_DIR "/Sound/win.mp3", 64);
    LoadSound(SoundCue::Lose, RESOURCE_DIR "/Sound/lose.mp3", 64);

    m_ZombieHitCueByPlantType = {
        {ToPlantTypeId(PlantType::Peashooter), SoundCue::ZombieHit1},
        {ToPlantTypeId(PlantType::IceShooter), SoundCue::ZombieHit2},
        {ToPlantTypeId(PlantType::FastShooter), SoundCue::ZombieHit3},
    };

    m_Initialized = true;
}

void AudioSystem::PlayMenuMusic() {
    PlayMusic(MusicTrack::Menu);
}

void AudioSystem::PlayGameplayMusic() {
    PlayMusic(MusicTrack::Gameplay);
}

void AudioSystem::PlayCherryBoom() {
    PlaySound(SoundCue::CherryBoom);
}

void AudioSystem::PlayZombieBoom() {
    PlaySound(SoundCue::ZombieBoom);
}

void AudioSystem::PlayZombieHit(int sourcePlantType) {
    PlaySound(GetZombieHitCue(sourcePlantType));
}

void AudioSystem::PlayZombieDead() {
    PlaySound(SoundCue::ZombieDead);
}

void AudioSystem::PlayZombieChew() {
    PlaySound(SoundCue::ZombieChew);
}

void AudioSystem::PlaySunCollect() {
    PlaySound(SoundCue::SunCollect);
}

void AudioSystem::PlayZombiesAreComing() {
    PlaySound(SoundCue::ZombiesAreComing);
}

void AudioSystem::PlayGameClick() {
    PlaySound(SoundCue::GameClick);
}

void AudioSystem::PlayButtonClick() {
    PlaySound(SoundCue::ButtonClick);
}

void AudioSystem::PlayLawnMower() {
    PlaySound(SoundCue::LawnMower);
}

void AudioSystem::PlayMoneyFalls() {
    PlaySound(SoundCue::MoneyFalls);
}

void AudioSystem::PlayPlacePlant() {
    PlaySound(SoundCue::PlacePlant);
}

void AudioSystem::PlayWin() {
    FadeOutMusic(MusicTrack::Gameplay, 500);
    PlaySound(SoundCue::Win);
}

void AudioSystem::PlayLose() {
    FadeOutMusic(MusicTrack::Gameplay, 500);
    PlaySound(SoundCue::Lose);
}

void AudioSystem::LoadMusic(MusicTrack track, const char* path, int volume) {
    auto music = std::make_unique<Util::BGM>(path);
    music->SetVolume(volume);
    m_Music[track] = std::move(music);
}

void AudioSystem::LoadSound(SoundCue cue, const char* path, int volume) {
    auto sound = std::make_unique<Util::SFX>(path);
    sound->SetVolume(volume);
    m_Sounds[cue] = std::move(sound);
}

void AudioSystem::PlayMusic(MusicTrack track) {
    auto music = m_Music.find(track);
    if (music != m_Music.end()) {
        music->second->Play();
    }
}

void AudioSystem::PlaySound(SoundCue cue) {
    auto sound = m_Sounds.find(cue);
    if (sound != m_Sounds.end()) {
        sound->second->Play();
    }
}

void AudioSystem::FadeOutMusic(MusicTrack track, int milliseconds) {
    auto music = m_Music.find(track);
    if (music != m_Music.end()) {
        music->second->FadeOut(milliseconds);
    }
}

AudioSystem::SoundCue AudioSystem::GetZombieHitCue(int sourcePlantType) const {
    auto cue = m_ZombieHitCueByPlantType.find(sourcePlantType);
    if (cue != m_ZombieHitCueByPlantType.end()) {
        return cue->second;
    }
    return SoundCue::ZombieHit1;
}
