#pragma once
#include <string>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    void playMusic(const std::string& path);
    void playSound(const std::string& path);
    void stopMusic();
};
