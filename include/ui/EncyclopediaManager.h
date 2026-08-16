#pragma once
#include <string>
#include "ui/DiscoveryManager.h"
#include "rendering/BitmapFont.h"

class EncyclopediaManager {
public:
    EncyclopediaManager();
    ~EncyclopediaManager();

    void update(DiscoveryManager& discoveryManager);
    void render(DiscoveryManager& discoveryManager, BitmapFont& font);
    void resetSelection();
    void selectEarthEntry();

private:
    int selectedIndex = 0;
    bool lastUpPressed = false;
    bool lastDownPressed = false;
    int entryCount = 9;
};
