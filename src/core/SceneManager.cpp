#include "core/SceneManager.h"
#include <iostream>

SceneManager::SceneManager() : currentScene("Menu") {}
SceneManager::~SceneManager() {}

void SceneManager::changeScene(const std::string& sceneName) {
    currentScene = sceneName;
    std::cout << "Scene changed to: " << currentScene << std::endl;
}

std::string SceneManager::getCurrentScene() const {
    return currentScene;
}
