#pragma once
#include <string>

class SaveManager {
public:
    SaveManager();
    ~SaveManager();
    bool saveGame(const std::string& filepath, int score, int highScore);
    bool loadGame(const std::string& filepath, int& score, int& highScore);
};
