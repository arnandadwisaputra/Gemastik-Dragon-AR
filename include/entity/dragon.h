#pragma once

class Dragon {
public:
    void load();
    void update(double dt, int level, float gravityAccX, float gravityAccY);
    void render();
    void reset();

    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float px, float py) { x = px; y = py; }
    
    float getGravityVelX() const { return gravityVelX; }
    float getGravityVelY() const { return gravityVelY; }
    void setGravityVel(float vx, float vy) { gravityVelX = vx; gravityVelY = vy; }

    bool isDashReady() const { return dashCooldownTimer <= 0.0f; }

private:
    int tex[9]; // 9 anim frames
    int frame = 0;
    double animTime = 0.0f;

    float x = 200.0f;
    float y = 300.0f;

    // Movement & physics states
    float gravityVelX = 0.0f;
    float gravityVelY = 0.0f;
    float dashCooldownTimer = 0.0f;

    bool lastSpacePressed = false;
    bool facingRight = true;
};
