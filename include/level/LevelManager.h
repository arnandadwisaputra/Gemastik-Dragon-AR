#pragma once
#include <string>

class LevelManager {
public:
    LevelManager();
    ~LevelManager();
    void setLevel(int levelId);
    int getCurrentLevel() const;
private:
    int currentLevel;
};
