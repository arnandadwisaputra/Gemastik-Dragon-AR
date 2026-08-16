#pragma once
#include <string>

class VideoPlayer {
public:
    VideoPlayer();
    ~VideoPlayer();

    bool open(const std::string& path);
    void play();
    void update();
    bool isFinished() const;
    bool isOpen() const;
    void close();

private:
    struct Impl;
    Impl* impl = nullptr;
    friend class MediaEngineEvents;
};

