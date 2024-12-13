#include <iostream>

#include "AudioManager.hpp"

bool AudioManager::initialized_ = false;
Music AudioManager::music_ = {{0}};

void AudioManager::initialize(const std::string &musicFilePath, float volume) {
    if (initialized_) {
        std::cerr << "AudioManager is already initialized." << std::endl;
        return;
    }

    InitAudioDevice();
    music_ = LoadMusicStream(musicFilePath.c_str());
    SetMusicVolume(music_, volume);
    PlayMusicStream(music_);
    initialized_ = true;
}

void AudioManager::update() {
    if (initialized_) {
        UpdateMusicStream(music_);
    }
}

void AudioManager::cleanup() {
    if (initialized_) {
        UnloadMusicStream(music_);
        CloseAudioDevice();
        initialized_ = false;
    }
}

void AudioManager::set_volume(float volume) {
    if (initialized_) {
        SetMusicVolume(music_, volume);
    }
}

void AudioManager::pause() {
    if (initialized_) {
        PauseMusicStream(music_);
    }
}

void AudioManager::resume() {
    if (initialized_) {
        ResumeMusicStream(music_);
    }
}
