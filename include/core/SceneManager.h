#pragma once
#include <string>

class SceneManager {
public:
    SceneManager();
    ~SceneManager();
    void changeScene(const std::string& sceneName);
    std::string getCurrentScene() const;
private:
    std::string currentScene;
};
