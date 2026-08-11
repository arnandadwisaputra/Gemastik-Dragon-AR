#include "ui/EncyclopediaManager.h"
#include <sl.h>
#include <iostream>

EncyclopediaManager::EncyclopediaManager() : selectedIndex(0), lastUpPressed(false), lastDownPressed(false) {}

EncyclopediaManager::~EncyclopediaManager() {}

void EncyclopediaManager::update(DiscoveryManager& discoveryManager) {
    bool up = slGetKey(SL_KEY_UP);
    bool down = slGetKey(SL_KEY_DOWN);

    // Arrow Up pressed
    if (up && !lastUpPressed) {
        selectedIndex = (selectedIndex - 1 + 9) % 9;
    }
    // Arrow Down pressed
    if (down && !lastDownPressed) {
        selectedIndex = (selectedIndex + 1) % 9;
    }

    lastUpPressed = up;
    lastDownPressed = down;
}

void EncyclopediaManager::render(DiscoveryManager& discoveryManager, BitmapFont& font) {
    // 1. Draw a semi-transparent dark overlay for background
    slSetForeColor(0.02, 0.02, 0.05, 0.9);
    slRectangleFill(400, 300, 800, 600);

    // Reset color to white for text drawing
    slSetForeColor(1, 1, 1, 1);

    // 2. Draw Title
    slSetForeColor(0.3, 0.8, 1.0, 1.0); // Light blue
    font.drawText("SPACE ENCYCLOPEDIA", 100, 540, 24, 30, 20);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(50, 510, 750, 510);

    // 3. Draw Left Column (List of entries)
    float startX = 50.0f;
    float startY = 460.0f;
    float spacingY = 38.0f;

    auto& discoveries = discoveryManager.getAllDiscoveries();

    for (int i = 0; i < 9; ++i) {
        float y = startY - i * spacingY;
        bool isSelected = (i == selectedIndex);
        const auto& d = discoveries[i];

        // Format name
        std::string displayName = d.name;
        if (!d.discovered) {
            displayName = "??? LOCKED ???";
        }

        if (isSelected) {
            // Draw marker
            slSetForeColor(1.0, 0.9, 0.2, 1.0); // Yellow
            font.drawText("> " + displayName, startX, y, 16, 20, 12);
        } else {
            if (d.discovered) {
                slSetForeColor(0.9, 0.9, 0.9, 1.0); // Off-white
            } else {
                slSetForeColor(0.5, 0.5, 0.5, 1.0); // Dark grey
            }
            font.drawText("  " + displayName, startX, y, 16, 20, 12);
        }
    }

    // Draw Vertical Divider
    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(320, 100, 320, 480);

    // 4. Draw Right Area (Details of selected entry)
    if (selectedIndex >= 0 && selectedIndex < 9) {
        const auto& d = discoveries[selectedIndex];
        float detailX = 350.0f;

        if (d.discovered) {
            // Title
            slSetForeColor(1.0, 0.9, 0.2, 1.0); // Yellow
            font.drawText(d.name, detailX, 460, 22, 26, 17);

            // Category Label
            slSetForeColor(0.6, 0.6, 0.6, 1.0);
            font.drawText("SCIENCE PHENOMENON DETECTED", detailX, 425, 12, 15, 9);

            slSetForeColor(0.3, 0.8, 1.0, 1.0);
            slLine(detailX, 410, 750, 410);

            // Description
            slSetForeColor(0.9, 0.9, 0.9, 1.0);
            font.drawWrappedText(d.longDescription, detailX, 370, 400, 14, 18, 10, 24);
        } else {
            // Locked screen
            slSetForeColor(0.5, 0.5, 0.5, 1.0);
            font.drawText("ENTRY LOCKED", detailX, 460, 22, 26, 17);

            slSetForeColor(0.3, 0.3, 0.3, 1.0);
            slLine(detailX, 410, 750, 410);

            slSetForeColor(0.6, 0.6, 0.6, 1.0);
            font.drawWrappedText("Explore the space environments in Dragon Asteroid Run. Discover this phenomenon during your mission to unlock detailed scientific data here.", detailX, 370, 400, 14, 18, 10, 24);
        }
    }

    // 5. Draw Footer
    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(50, 80, 750, 80);

    slSetForeColor(0.7, 0.7, 0.7, 1.0);
    font.drawText("UP/DOWN: NAVIGATE  |  ESC: RETURN TO MENU", 180, 50, 14, 18, 10);
}

void EncyclopediaManager::resetSelection() {
    selectedIndex = 0;
}
