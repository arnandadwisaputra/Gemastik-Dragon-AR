#include "level/LevelManager.h"
#include <iostream>

LevelManager::LevelManager() : currentLevel(1) {}
LevelManager::~LevelManager() {}

void LevelManager::setLevel(int levelId) {
    currentLevel = levelId;
    std::cout << "Level changed to: " << currentLevel << std::endl;
}

int LevelManager::getCurrentLevel() const {
    return currentLevel;
}
