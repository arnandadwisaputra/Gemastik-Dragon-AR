#include "entity/asteroid.h"
#include "core/utils.h"
#include <sl.h>
#include <cmath>
#include <cstdlib>

bool Asteroid::texturesLoaded = false;
std::unordered_map<ObstacleType, int> Asteroid::obstacleTextures;
int Asteroid::baseAsteroidTexs[5] = {0};

void Asteroid::load() {
    if (!texturesLoaded) {
        // Load standard asteroid variations (used for type ASTEROID)
        for (int i = 0; i < 5; ++i) {
            std::string path = Utils::getAssetPath("asteroid", std::to_string(i) + ".png");
            baseAsteroidTexs[i] = slLoadTexture(path.c_str());
        }

        // Load obstacle textures
        obstacleTextures[ObstacleType::ASTEROID] = slLoadTexture(Utils::getAssetPath("obstacle", "asteroid_01.png").c_str());
        obstacleTextures[ObstacleType::SATELLITE] = slLoadTexture(Utils::getAssetPath("obstacle", "satellite_01.png").c_str());
        obstacleTextures[ObstacleType::SPACE_DEBRIS] = slLoadTexture(Utils::getAssetPath("obstacle", "space_debris_01.png").c_str());
        obstacleTextures[ObstacleType::ASTEROID_BELT] = slLoadTexture(Utils::getAssetPath("obstacle", "asteroid_belt_01.png").c_str());
        obstacleTextures[ObstacleType::SOLAR_FLARE] = slLoadTexture(Utils::getAssetPath("obstacle", "solar_flare_01.png").c_str());
        obstacleTextures[ObstacleType::COMET] = slLoadTexture(Utils::getAssetPath("obstacle", "comet_01.png").c_str());
        obstacleTextures[ObstacleType::METEOROID] = slLoadTexture(Utils::getAssetPath("obstacle", "meteoroid_01.png").c_str());
        obstacleTextures[ObstacleType::PULSAR] = slLoadTexture(Utils::getAssetPath("obstacle", "pulsar_01.png").c_str());
        obstacleTextures[ObstacleType::GRAVITY_WELL] = slLoadTexture(Utils::getAssetPath("obstacle", "gravity_well_01.png").c_str());
        obstacleTextures[ObstacleType::CELESTIAL_OBJECT] = slLoadTexture(Utils::getAssetPath("obstacle", "celestial_object_01.png").c_str());
        obstacleTextures[ObstacleType::BLACK_HOLE] = slLoadTexture(Utils::getAssetPath("obstacle", "blackhole_01.png").c_str());

        texturesLoaded = true;
    }
    reset();
}

void Asteroid::reset() {
    active = false;
    posX = 900.0f;
    posY = 300.0f;
    velX = 0.0f;
    velY = 0.0f;
    sizeX = 80.0f;
    sizeY = 80.0f;
    radius = 35.0f;
    angle = 0.0f;
    rotSpeed = 0.0f;
    timer = 0.0f;
}

void Asteroid::spawn(ObstacleType t, float startX, float startY, float vx, float vy, float sx, float sy) {
    type = t;
    posX = startX;
    posY = startY;
    velX = vx;
    velY = vy;
    sizeX = sx;
    sizeY = sy;
    radius = (sx + sy) / 4.0f; // Approx radius
    angle = (float)(rand() % 360);
    rotSpeed = (float)(rand() % 90 - 45); // Random rotation speed
    active = true;
    timer = 0.0f;

    // Adjust specific properties
    if (type == ObstacleType::BLACK_HOLE) {
        radius = 70.0f; // Large collision area
    } else if (type == ObstacleType::SOLAR_FLARE) {
        radius = 25.0f; // Solar flares can be narrow
    }
}

void Asteroid::update(double dt, float speedMultiplier, float playerX, float playerY, float& playerVelX, float& playerVelY) {
    if (!active) return;

    timer += (float)dt;

    // Apply basic motion
    posX += velX * (float)dt * speedMultiplier;
    posY += velY * (float)dt * speedMultiplier;
    angle += rotSpeed * (float)dt;

    // Boundary check to auto-deactivate out-of-bounds obstacles
    if (type != ObstacleType::BLACK_HOLE && type != ObstacleType::PULSAR && type != ObstacleType::GRAVITY_WELL) {
        if (posX < -150.0f || posX > 1050.0f || posY < -150.0f || posY > 750.0f) {
            active = false;
        }
    }

    // Solar Flare custom rotation & movement behavior if needed
    // (Solar flares just travel in their straight line direction across screen)

    // Gravity Well & Black Hole attraction physics
    if (type == ObstacleType::PULSAR || type == ObstacleType::GRAVITY_WELL || type == ObstacleType::BLACK_HOLE) {
        float dx = posX - playerX;
        float dy = posY - playerY;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > 25.0f) {
            float force = 0.0f;
            if (type == ObstacleType::PULSAR || type == ObstacleType::GRAVITY_WELL) {
                float G = 150000.0f; // Gravity strength
                force = G / (distance * distance);
                if (force > 250.0f) force = 250.0f; // Cap
            } else if (type == ObstacleType::BLACK_HOLE) {
                float G = 400000.0f; // Stronger attraction
                force = G / (distance * distance);
                if (force > 600.0f) force = 600.0f;
            }

            playerVelX += (dx / distance) * force * (float)dt;
            playerVelY += (dy / distance) * force * (float)dt;
        }
    }
}

void Asteroid::render() {
    if (!active) return;

    int texture = 0;
    if (type == ObstacleType::ASTEROID) {
        // Use standard variations for asteroids
        texture = baseAsteroidTexs[0];
    } else {
        auto it = obstacleTextures.find(type);
        if (it != obstacleTextures.end()) {
            texture = it->second;
        }
    }

    if (texture != 0) {
        slPush();
        slTranslate(posX, posY);
        slRotate(angle);
        slSprite(texture, 0, 0, sizeX, sizeY);
        slPop();
    }
}

bool Asteroid::checkCollision(float px, float py, float playerRadius) {
    if (!active) return false;

    float dx = px - posX;
    float dy = py - posY;
    float dist = std::sqrt(dx * dx + dy * dy);

    // Simple circle collision
    return (dist < (radius + playerRadius));
}
