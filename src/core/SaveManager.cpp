#include "core/SaveManager.h"
#include <iostream>

SaveManager::SaveManager() {}
SaveManager::~SaveManager() {}

bool SaveManager::saveGame(const std::string& filepath, int score, int highScore) {
    std::cout << "Saving game (Score: " << score << ", HighScore: " << highScore << ") to " << filepath << std::endl;
    return true;
}

bool SaveManager::loadGame(const std::string& filepath, int& score, int& highScore) {
    std::cout << "Loading game from " << filepath << std::endl;
    score = 0;
    highScore = 0;
    return true;
}
