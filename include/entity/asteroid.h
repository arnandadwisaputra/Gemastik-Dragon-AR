#pragma once
#include <string>
#include <unordered_map>

enum class ObstacleType {
    ASTEROID = 0,
    SATELLITE,
    SPACE_DEBRIS,
    ASTEROID_BELT,
    SOLAR_FLARE,
    COMET,
    METEOROID,
    PULSAR,
    GRAVITY_WELL,
    CELESTIAL_OBJECT,
    BLACK_HOLE
};

class Asteroid {
public:
    static bool texturesLoaded;
    static std::unordered_map<ObstacleType, int> obstacleTextures;
    static int baseAsteroidTexs[5]; // Variasi asteroid 0-4

    void load();
    void reset();
    void spawn(ObstacleType type, float startX, float startY, float speedX, float speedY, float sizeX = 80.0f, float sizeY = 80.0f);
    void update(double dt, float speedMultiplier, float playerX, float playerY, float& playerVelX, float& playerVelY);
    void render();
    bool checkCollision(float px, float py, float playerRadius = 35.0f);
    
    float getX() const { return posX; }
    float getY() const { return posY; }
    ObstacleType getType() const { return type; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    float getRadius() const { return radius; }

private:
    ObstacleType type = ObstacleType::ASTEROID;
    float posX = 900.0f;
    float posY = 300.0f;
    float velX = -200.0f;
    float velY = 0.0f;
    float sizeX = 80.0f;
    float sizeY = 80.0f;
    float radius = 35.0f;
    float angle = 0.0f;
    float rotSpeed = 0.0f;
    bool active = false;

    // Untuk custom behavior
    int direction = 0; // Solar flare direction (0: L, 1: R, 2: T, 3: B)
    float timer = 0.0f;
};
