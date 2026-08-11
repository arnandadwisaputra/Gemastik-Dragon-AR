#include "entity/dragon.h"
#include "core/utils.h"
#include <sl.h>
#include <cmath>
#include <string>

using namespace std;

void Dragon::load() {
    for (int i = 0; i < 9; ++i) {
        string path = Utils::getAssetPath("dragon", to_string(i) + ".png");
        tex[i] = slLoadTexture(path.c_str());
    }
    reset();
}

void Dragon::reset() {
    x = 200.0f;
    y = 300.0f;
    gravityVelX = 0.0f;
    gravityVelY = 0.0f;
    dashCooldownTimer = 0.0f;
    lastSpacePressed = false;
    frame = 0;
    animTime = 0.0;
}

void Dragon::update(double dt, int level, float gravityAccX, float gravityAccY) {
    float steerX = 0.0f;
    float steerY = 0.0f;
    float speed = 300.0f;

    // Read steering inputs
    bool up = slGetKey(SL_KEY_UP) || slGetKey('W');
    bool down = slGetKey(SL_KEY_DOWN) || slGetKey('S');
    bool left = slGetKey(SL_KEY_LEFT) || slGetKey('A');
    bool right = slGetKey(SL_KEY_RIGHT) || slGetKey('D');

    if (up) steerY = speed;
    if (down) steerY = -speed;
    if (left) steerX = -speed;
    if (right) steerX = speed;

    if (level <= 3) {
        // Level 1-3: Vertical movement only
        x = 200.0f;
        y += steerY * (float)dt;
        gravityVelX = 0.0f;
        gravityVelY = 0.0f;
    } 
    else if (level == 4) {
        // Level 4: 360 movement
        x += steerX * (float)dt;
        y += steerY * (float)dt;
        gravityVelX = 0.0f;
        gravityVelY = 0.0f;
    }
    else if (level == 5) {
        // Level 5: 360 movement affected by gravity force
        gravityVelX += gravityAccX * (float)dt;
        gravityVelY += gravityAccY * (float)dt;
        
        // Apply friction to gravity velocity
        gravityVelX *= 0.95f;
        gravityVelY *= 0.95f;

        x += (steerX + gravityVelX) * (float)dt;
        y += (steerY + gravityVelY) * (float)dt;
    }
    else if (level == 6) {
        // Level 6: Auto-pulled by black hole gravity, normal keys disabled, only dash is active
        bool space = slGetKey(' ') || slGetKey(SL_KEY_ENTER);

        // Dash mechanic
        if (space && !lastSpacePressed && dashCooldownTimer <= 0.0f) {
            float dx = x - 400.0f; // Away from center black hole at (400, 300)
            float dy = y - 300.0f;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 1.0f) {
                // Strong dash away from black hole
                gravityVelX += (dx / dist) * 1100.0f;
                gravityVelY += (dy / dist) * 1100.0f;
                dashCooldownTimer = 0.5f; // 0.5s cooldown
            }
        }
        lastSpacePressed = space;

        // Apply gravity velocity
        gravityVelX += gravityAccX * (float)dt;
        gravityVelY += gravityAccY * (float)dt;

        // Apply friction
        gravityVelX *= 0.96f;
        gravityVelY *= 0.96f;

        x += gravityVelX * (float)dt;
        y += gravityVelY * (float)dt;

        if (dashCooldownTimer > 0.0f) {
            dashCooldownTimer -= (float)dt;
        }
    }

    // Clamp inside window boundaries (except when sucked in in level 6)
    if (level != 6) {
        if (x < 50.0f) x = 50.0f;
        if (x > 750.0f) x = 750.0f;
        if (y < 50.0f) y = 50.0f;
        if (y > 550.0f) y = 550.0f;
    } else {
        // Keep inside slightly wider screen boundary, but let them reach (400, 300)
        if (x < 0.0f) x = 0.0f;
        if (x > 800.0f) x = 800.0f;
        if (y < 0.0f) y = 0.0f;
        if (y > 600.0f) y = 600.0f;
    }

    // Animation frame update (frame changes every 0.1s)
    animTime += dt;
    if (animTime >= 0.1) {
        frame = (frame + 1) % 9;
        animTime = 0.0;
    }
}

void Dragon::render() {
    slSprite(tex[frame], x, y, 100, 100);
}
