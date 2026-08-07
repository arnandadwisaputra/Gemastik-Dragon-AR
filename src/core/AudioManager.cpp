#include "core/AudioManager.h"
#include <iostream>

AudioManager::AudioManager() {}
AudioManager::~AudioManager() {}

void AudioManager::playMusic(const std::string& path) {
    std::cout << "Playing background music: " << path << std::endl;
}

void AudioManager::playSound(const std::string& path) {
    std::cout << "Playing sound effect: " << path << std::endl;
}

void AudioManager::stopMusic() {
    std::cout << "Music stopped." << std::endl;
}
