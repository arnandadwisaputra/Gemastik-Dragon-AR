#pragma once
#include <string>

class EncyclopediaManager {
public:
    EncyclopediaManager();
    ~EncyclopediaManager();
    void showEntry(const std::string& entryName);
};
