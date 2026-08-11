#include "core/game.h"
#include "core/utils.h"
#include <sl.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

void Game::load() {
    // 1. Load Backgrounds
    bgTex[0] = slLoadTexture(Utils::getAssetPath("background", "bg_level1_earth.png.png").c_str());
    bgTex[1] = slLoadTexture(Utils::getAssetPath("background", "bg_level2_asteroid_belt_tile.png").c_str());
    bgTex[2] = slLoadTexture(Utils::getAssetPath("background", "bg_level3_solar_activity_tile.png").c_str());
    bgTex[3] = slLoadTexture(Utils::getAssetPath("background", "bg_level4_deep_space_tile.png").c_str());
    bgTex[4] = slLoadTexture(Utils::getAssetPath("background", "bg_level5_nebula_tile.png").c_str());
    bgTex[5] = slLoadTexture(Utils::getAssetPath("background", "bg_level6_deep_space_tile.png").c_str());

    menuBgTex = slLoadTexture(Utils::getAssetPath("ui", "menu_bg.jpg").c_str());
    gameOverBgTex = slLoadTexture(Utils::getAssetPath("ui", "game_over.jpg").c_str());
    heartTex = slLoadTexture(Utils::getAssetPath("ui", "heart.png").c_str());

    // Use gravity well texture for wormhole sprite
    wormholeTex = slLoadTexture(Utils::getAssetPath("obstacle", "gravity_well_01.png").c_str());

    // 2. Load Audio
    bgm = slLoadWAV(Utils::getAssetPath("audio", "bgm.wav").c_str());
    hitSfx = slLoadWAV(Utils::getAssetPath("audio", "hit.wav").c_str());
    winSfx = slLoadWAV(Utils::getAssetPath("audio", "hit.wav").c_str()); // Fallback or reload same
    selectSfx = slLoadWAV(Utils::getAssetPath("audio", "hit.wav").c_str());

    slSoundLoop(bgm);

    // 3. Load Managers
    font.load();
    discoveryManager.init();
    dragon.load();

    // 4. Preallocate obstacles
    obstacles.resize(maxObstacles);
    for (auto& obs : obstacles) {
        obs.load();
    }

    // 5. Load Highscore
    loadHighScore();

    srand((unsigned)time(NULL));
}

bool Game::isClicked(float x, float y, float w, float h) {
    if (slGetMouseButton(SL_MOUSE_BUTTON_LEFT)) {
        float mx = (float)slGetMouseX();
        float my = (float)slGetMouseY();
        return (mx > x - w / 2.0f && mx < x + w / 2.0f && my > y - h / 2.0f && my < y + h / 2.0f);
    }
    return false;
}

void Game::loadHighScore() {
    FILE* f = fopen("save/highscore.txt", "r");
    if (f) {
        if (fscanf(f, "%d", &highScore) != 1) {
            highScore = 0;
        }
        fclose(f);
    } else {
        highScore = 0;
    }
}

void Game::saveHighScore() {
    CreateDirectoryA("save", NULL);
    FILE* f = fopen("save/highscore.txt", "w");
    if (f) {
        fprintf(f, "%d", highScore);
        fclose(f);
    }
}

void Game::resetLevel(int lvl) {
    currentLevel = lvl;
    levelTimer = 0.0f;
    difficultyMultiplier = 1.0f;
    spawnTimer = 0.0f;

    // Clear active obstacles
    for (auto& obs : obstacles) {
        obs.reset();
    }

    // Reset player
    dragon.reset();

    // Trigger instant discoveries for specific levels
    if (currentLevel == 2) {
        triggerInfoPopup("ASTEROID_BELT");
    } else if (currentLevel == 3) {
        triggerInfoPopup("SOLAR_FLARE");
    } else if (currentLevel == 5) {
        triggerInfoPopup("GRAVITY");
    } else if (currentLevel == 6) {
        triggerInfoPopup("BLACK_HOLE");
        // Immediately spawn the black hole at center (400, 300)
        obstacles[0].spawn(ObstacleType::BLACK_HOLE, 400.0f, 300.0f, 0.0f, 0.0f, 150.0f, 150.0f);
    }
}

void Game::startNextLevel() {
    currentLevel++;
    if (currentLevel > 6) {
        currentState = GameState::ENDING;
    } else {
        resetLevel(currentLevel);
        currentState = GameState::PLAYING;
    }
}

void Game::triggerInfoPopup(const string& name) {
    bool isNew = false;
    discoveryManager.unlockPhenomenon(name, isNew);
    if (isNew) {
        activeDiscovery = discoveryManager.getDiscovery(name);
        if (activeDiscovery) {
            currentState = GameState::INFO_POPUP;
        }
    }
}

void Game::triggerLevelComplete() {
    string name = "";
    if (currentLevel == 1) name = "SATELLITE";
    else if (currentLevel == 2) name = "ASTEROID_BELT";
    else if (currentLevel == 3) name = "SOLAR_FLARE";
    else if (currentLevel == 4) name = "COMET";
    else if (currentLevel == 5) name = "PULSAR";

    activeDiscovery = discoveryManager.getDiscovery(name);
    if (activeDiscovery) {
        currentState = GameState::LEVEL_COMPLETE_INFO;
    } else {
        // Fallback to wormhole transition directly
        currentState = GameState::WORMHOLE_TRANSITION;
        wormholeTimer = 0.0f;
    }
}

void Game::spawnObstacle() {
    // Find an inactive slot
    Asteroid* slot = nullptr;
    for (auto& obs : obstacles) {
        if (!obs.isActive()) {
            slot = &obs;
            break;
        }
    }

    if (!slot) return; // All slots full

    float startX = 850.0f;
    float startY = 80.0f + (rand() % 460);

    if (currentLevel == 1) {
        // Obstacles: ASTEROID, SATELLITE, SPACE_DEBRIS
        int r = rand() % 3;
        ObstacleType type = ObstacleType::ASTEROID;
        float size = 70.0f;
        if (r == 1) {
            type = ObstacleType::SATELLITE;
            size = 65.0f;
        } else if (r == 2) {
            type = ObstacleType::SPACE_DEBRIS;
            size = 60.0f;
        }

        float vx = -200.0f - (rand() % 80);
        slot->spawn(type, startX, startY, vx, 0.0f, size, size);
    } 
    else if (currentLevel == 2) {
        // Level 2: Asteroid Belt canyon style
        // Spawn standard asteroids forming a gap
        static float lastCanyonTime = 0.0f;
        float now = (float)slGetTime();
        if (now - lastCanyonTime >= 1.6f) {
            lastCanyonTime = now;
            float gapCenterY = 150.0f + (rand() % 300); // 150 to 450

            // Spawn a column of asteroids
            int spawnedCount = 0;
            for (float y = 50.0f; y <= 550.0f; y += 100.0f) {
                if (std::abs(y - gapCenterY) < 85.0f) {
                    continue; // Leave a clear gap
                }
                
                // Find next slot
                Asteroid* nextSlot = nullptr;
                for (auto& obs : obstacles) {
                    if (!obs.isActive()) {
                        nextSlot = &obs;
                        break;
                    }
                }
                if (nextSlot) {
                    float size = 80.0f + (rand() % 20);
                    nextSlot->spawn(ObstacleType::ASTEROID_BELT, startX + spawnedCount * 25.0f, y, -170.0f, 0.0f, size, size);
                    spawnedCount++;
                }
            }
        }
    } 
    else if (currentLevel == 3) {
        // Level 3: Solar flares from 4 directions targeting player
        static int flareDir = 0;
        float py = dragon.getY();
        float px = dragon.getX();

        float sizeX = 120.0f;
        float sizeY = 50.0f;

        if (flareDir == 0) { // LEFT
            slot->spawn(ObstacleType::SOLAR_FLARE, -80.0f, py, 260.0f, 0.0f, sizeX, sizeY);
        } else if (flareDir == 1) { // RIGHT
            slot->spawn(ObstacleType::SOLAR_FLARE, 880.0f, py, -260.0f, 0.0f, sizeX, sizeY);
        } else if (flareDir == 2) { // TOP
            slot->spawn(ObstacleType::SOLAR_FLARE, px, 680.0f, 0.0f, -260.0f, sizeY, sizeX);
        } else if (flareDir == 3) { // BOTTOM
            slot->spawn(ObstacleType::SOLAR_FLARE, px, -80.0f, 0.0f, 260.0f, sizeY, sizeX);
        }
        flareDir = (flareDir + 1) % 4;
    } 
    else if (currentLevel == 4) {
        // Level 4: Deep space diagonal comets + slow meteoroids + normal asteroids
        int r = rand() % 100;
        if (r < 35) {
            // Diagonal Comet (Fast)
            float startCometY = 200.0f + (rand() % 400); // Top half
            float vx = -380.0f;
            float vy = -120.0f - (rand() % 100);
            slot->spawn(ObstacleType::COMET, startX, startCometY, vx, vy, 85.0f, 50.0f);
        } else if (r < 70) {
            // Large slow Meteoroid
            float size = 95.0f;
            slot->spawn(ObstacleType::METEOROID, startX, startY, -110.0f, 0.0f, size, size);
        } else {
            // Normal Asteroid
            slot->spawn(ObstacleType::ASTEROID, startX, startY, -200.0f, 0.0f, 75.0f, 75.0f);
        }
    } 
    else if (currentLevel == 5) {
        // Level 5: Pulsars / Gravity wells drifting left + Celestial Object waves
        int r = rand() % 3;
        if (r == 0) {
            // Gravity well
            slot->spawn(ObstacleType::GRAVITY_WELL, startX, 300.0f, -65.0f, 0.0f, 130.0f, 130.0f);
        } else if (r == 1) {
            // Pulsar
            slot->spawn(ObstacleType::PULSAR, startX, 150.0f + (rand() % 300), -80.0f, 0.0f, 110.0f, 110.0f);
        } else {
            // Celestial object moving in wave
            float startWaveY = 150.0f + (rand() % 300);
            slot->spawn(ObstacleType::CELESTIAL_OBJECT, startX, startWaveY, -160.0f, 50.0f, 75.0f, 75.0f);
        }
    }
}

void Game::update() {
    float dt = (float)slGetDeltaTime();

    // Clamp delta time to avoid physics breaks during lag spikes
    if (dt > 0.15f) dt = 0.15f;

    // MENU STATE
    if (currentState == GameState::MENU) {
        // Toggle debug mode with key D
        static bool lastDPressed = false;
        bool dPressed = slGetKey('D');
        if (dPressed && !lastDPressed) {
            debugMode = !debugMode;
        }
        lastDPressed = dPressed;

        if (isClicked(400, 260, 220, 50) || slGetKey(SL_KEY_ENTER)) {
            currentState = GameState::PLAYING;
            resetLevel(1);
            score = 0;
            lives = 3;
        }
        else if (isClicked(400, 190, 220, 50) || slGetKey('E')) {
            currentState = GameState::ENCYCLOPEDIA;
            encyclopediaManager.resetSelection();
        }
        else if (isClicked(400, 120, 220, 50) || slGetKey('Q')) {
            slClose();
            exit(0);
        }
        return;
    }

    // ENCYCLOPEDIA STATE
    if (currentState == GameState::ENCYCLOPEDIA) {
        encyclopediaManager.update(discoveryManager);
        if (slGetKey(SL_KEY_ESCAPE)) {
            currentState = GameState::MENU;
        }
        return;
    }

    // GAME OVER STATE
    if (currentState == GameState::GAME_OVER) {
        if (isClicked(300, 160, 180, 50) || slGetKey(' ') || slGetKey(SL_KEY_ENTER)) {
            lives = 3;
            resetLevel(currentLevel);
            currentState = GameState::PLAYING;
        }
        else if (isClicked(500, 160, 180, 50) || slGetKey(SL_KEY_ESCAPE)) {
            currentState = GameState::MENU;
        }
        return;
    }

    // ENDING STATE
    if (currentState == GameState::ENDING) {
        if (slGetKey(SL_KEY_ENTER) || slGetKey(' ') || slGetKey(SL_KEY_ESCAPE)) {
            currentState = GameState::MENU;
        }
        return;
    }

    // POPUP STATES (PAUSED MECHANICS)
    if (currentState == GameState::INFO_POPUP) {
        if (slGetKey(SL_KEY_ENTER) || slGetKey(' ')) {
            currentState = GameState::PLAYING;
        }
        return;
    }

    if (currentState == GameState::LEVEL_COMPLETE_INFO) {
        if (slGetKey(SL_KEY_ENTER) || slGetKey(' ')) {
            currentState = GameState::QUESTION;
            quizSelectedAnswer = -1;
            quizAnswered = false;
        }
        return;
    }

    if (currentState == GameState::QUESTION) {
        if (!quizAnswered) {
            if (slGetKey('A')) quizSelectedAnswer = 0;
            else if (slGetKey('B')) quizSelectedAnswer = 1;
            else if (slGetKey('C')) quizSelectedAnswer = 2;
            else if (slGetKey('D')) quizSelectedAnswer = 3;

            if (quizSelectedAnswer != -1) {
                quizAnswered = true;
                if (quizSelectedAnswer == activeDiscovery->correctAnswerIndex) {
                    quizAnswerCorrect = true;
                    score += 100;
                } else {
                    quizAnswerCorrect = false;
                }
            }
        } else {
            if (slGetKey(SL_KEY_ENTER) || slGetKey(' ')) {
                currentState = GameState::WORMHOLE_TRANSITION;
                wormholeTimer = 0.0f;
                wormholeScale = 0.0f;
                wormholeRotation = 0.0f;
            }
        }
        return;
    }

    if (currentState == GameState::WORMHOLE_TRANSITION) {
        wormholeTimer += dt;
        wormholeRotation += 150.0f * dt;
        if (wormholeTimer < 0.9f) {
            wormholeScale = wormholeTimer / 0.9f;
        } else {
            wormholeScale = (1.8f - wormholeTimer) / 0.9f;
        }

        if (wormholeTimer >= 1.8f) {
            startNextLevel();
        }
        return;
    }

    // ACTIVE PLAYING STATE
    if (currentState == GameState::PLAYING) {
        // Toggle pause info overlay if Escape is pressed (Standard pause is handled)
        static bool lastEscPressed = false;
        bool escPressed = slGetKey(SL_KEY_ESCAPE);
        if (escPressed && !lastEscPressed) {
            // Return to menu or keep paused. The instructions say "game pause pada info/question". 
            // Escape can simply quit to menu
            currentState = GameState::MENU;
            if (score > highScore) {
                highScore = score;
                saveHighScore();
            }
        }
        lastEscPressed = escPressed;

        levelTimer += dt;

        // Background scrolling
        bgX1 -= bgSpeed * dt;
        bgX2 -= bgSpeed * dt;
        if (bgX1 <= -400) bgX1 = bgX2 + 800;
        if (bgX2 <= -400) bgX2 = bgX1 + 800;

        // Difficulty increases over time
        difficultyMultiplier += difficultyRate * dt;

        // Spawning obstacles
        if (currentLevel != 6) {
            spawnTimer += dt;
            float currentSpawnInterval = baseSpawnInterval / difficultyMultiplier;
            if (spawnTimer >= currentSpawnInterval) {
                spawnObstacle();
                spawnTimer = 0.0f;
            }
        }

        // Net gravity force on player calculation
        float gravityAccX = 0.0f;
        float gravityAccY = 0.0f;

        if (currentLevel == 6) {
            // Black Hole gravitational pull
            float dx = 400.0f - dragon.getX();
            float dy = 300.0f - dragon.getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            // Sucked in condition
            if (dist < 40.0f) {
                currentState = GameState::ENDING;
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }
                return;
            }

            if (dist > 5.0f) {
                // Pull increases as level time progresses
                float G = 240000.0f + levelTimer * 50000.0f;
                float force = G / (dist * dist);
                if (force > 1500.0f) force = 1500.0f; // Limit extreme force
                
                gravityAccX = (dx / dist) * force;
                gravityAccY = (dy / dist) * force;
            }
        } 
        else if (currentLevel == 5) {
            // Drifting gravity sources
            for (auto& obs : obstacles) {
                if (obs.isActive() && (obs.getType() == ObstacleType::PULSAR || obs.getType() == ObstacleType::GRAVITY_WELL)) {
                    float dx = obs.getX() - dragon.getX();
                    float dy = obs.getY() - dragon.getY();
                    float dist = std::sqrt(dx * dx + dy * dy);

                    if (dist > 25.0f) {
                        float G = 150000.0f;
                        float force = G / (dist * dist);
                        if (force > 250.0f) force = 250.0f; // limit pull

                        gravityAccX += (dx / dist) * force;
                        gravityAccY += (dy / dist) * force;
                    }
                }
            }
        }

        // Update player
        dragon.update(dt, currentLevel, gravityAccX, gravityAccY);

        // Update obstacles
        for (auto& obs : obstacles) {
            if (obs.isActive()) {
                float dummyX = 0, dummyY = 0;
                // Move custom celestial wave paths
                if (obs.getType() == ObstacleType::CELESTIAL_OBJECT) {
                    // Update Y position by wave
                    float newY = obs.getY() + std::sin((float)slGetTime() * 3.5f) * 120.0f * dt;
                    obs.spawn(ObstacleType::CELESTIAL_OBJECT, obs.getX(), newY, -160.0f, 0.0f, 75.0f, 75.0f);
                }
                obs.update(dt, difficultyMultiplier, dragon.getX(), dragon.getY(), dummyX, dummyY);
            }
        }

        // Collisions check
        for (auto& obs : obstacles) {
            if (obs.isActive() && obs.getType() != ObstacleType::BLACK_HOLE) {
                if (obs.checkCollision(dragon.getX(), dragon.getY())) {
                    slSoundPlay(hitSfx);
                    lives--;
                    
                    // Trigger discovery info on collision (if not unlocked)
                    string item = "";
                    switch (obs.getType()) {
                        case ObstacleType::ASTEROID: item = "ASTEROID"; break;
                        case ObstacleType::SATELLITE: item = "SATELLITE"; break;
                        case ObstacleType::SPACE_DEBRIS: item = "SPACE_DEBRIS"; break;
                        case ObstacleType::COMET: item = "COMET"; break;
                        case ObstacleType::METEOROID: item = "METEOROID"; break;
                        case ObstacleType::PULSAR: item = "PULSAR"; break;
                        default: break;
                    }

                    obs.deactivate(); // Consume obstacle

                    if (!item.empty()) {
                        triggerInfoPopup(item);
                    }

                    if (lives <= 0) {
                        currentState = GameState::GAME_OVER;
                        if (score > highScore) {
                            highScore = score;
                            saveHighScore();
                        }
                    }
                    break;
                }
            }
        }

        // Score tick
        static float scoreTimer = 0;
        scoreTimer += dt;
        if (scoreTimer >= 1.0f) {
            score += 3 * currentLevel;
            scoreTimer = 0.0f;
        }

        // Check level timer complete
        if (levelTimer >= getLevelDuration()) {
            if (currentLevel < 6) {
                triggerLevelComplete();
            }
        }
    }
}

void Game::drawHUD() {
    // Score labels
    font.drawText("SCORE: " + to_string(score), 40, 560, 14, 18, 10);
    font.drawText("LEVEL: " + to_string(currentLevel), 250, 560, 14, 18, 10);

    // Render lives (hearts)
    for (int i = 0; i < lives; ++i) {
        slSprite(heartTex, 620.0f + (i * 35.0f), 570.0f, 25.0f, 25.0f);
    }

    // Render Level 6 Dash prompt
    if (currentLevel == 6) {
        dashTextBlinkTimer += (float)slGetDeltaTime();
        if (dragon.isDashReady()) {
            if (std::fmod(dashTextBlinkTimer, 0.6f) < 0.3f) {
                slSetForeColor(1.0, 0.9, 0.2, 1.0); // yellow
                font.drawText("DASH READY [SPACEBAR]", 290, 50, 14, 18, 10);
            }
        } else {
            slSetForeColor(0.5, 0.5, 0.5, 1.0); // grey
            font.drawText("DASH CHARGING...", 320, 50, 14, 18, 10);
        }
        slSetForeColor(1, 1, 1, 1);
    }
}

void Game::drawMenu() {
    slSprite(menuBgTex, 400, 300, 800, 600);

    // Title
    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawText("DRAGON ASTEROID RUN", 160, 480, 24, 30, 20);

    slSetForeColor(1, 1, 1, 1);
    font.drawText("HIGH SCORE: " + to_string(highScore), 310, 400, 15, 20, 11);

    // Buttons
    // Play button
    slSetForeColor(0.1, 0.15, 0.3, 0.85);
    slRectangleFill(400, 260, 220, 46);
    slSetForeColor(1, 1, 1, 1);
    font.drawText("START GAME", 335, 250, 16, 20, 12);

    // Encyclopedia button
    slSetForeColor(0.1, 0.15, 0.3, 0.85);
    slRectangleFill(400, 190, 220, 46);
    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawText("ENCYCLOPEDIA", 330, 180, 16, 20, 12);

    // Exit button
    slSetForeColor(0.1, 0.15, 0.3, 0.85);
    slRectangleFill(400, 120, 220, 46);
    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawText("QUIT GAME", 345, 110, 16, 20, 12);

    // Debug label
    if (debugMode) {
        slSetForeColor(1.0, 0.2, 0.2, 1.0);
        font.drawText("[DEBUG LEVEL TIMERS ACTIVE]", 230, 50, 14, 18, 10);
    } else {
        slSetForeColor(0.4, 0.4, 0.4, 1.0);
        font.drawText("PRESS [D] TO TOGGLE DEBUG MODE", 220, 50, 14, 18, 10);
    }
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawGameOver() {
    slSprite(gameOverBgTex, 400, 300, 800, 600);

    slSetForeColor(1.0, 0.1, 0.1, 1.0);
    font.drawText("GAME OVER", 280, 450, 28, 36, 22);

    slSetForeColor(1, 1, 1, 1);
    font.drawText("FAILED IN LEVEL " + to_string(currentLevel), 300, 380, 16, 20, 12);
    font.drawText("FINAL SCORE: " + to_string(score), 310, 320, 16, 20, 12);

    // Retry Button
    slSetForeColor(0.1, 0.3, 0.1, 0.85);
    slRectangleFill(300, 160, 180, 46);
    slSetForeColor(1, 1, 1, 1);
    font.drawText("RETRY", 265, 150, 16, 20, 12);

    // Exit Button
    slSetForeColor(0.3, 0.1, 0.1, 0.85);
    slRectangleFill(500, 160, 180, 46);
    slSetForeColor(1, 1, 1, 1);
    font.drawText("MENU", 470, 150, 16, 20, 12);
}

void Game::drawInfoPopup() {
    if (!activeDiscovery) return;

    // Dark semi-transparent box
    slSetForeColor(0.02, 0.02, 0.05, 0.92);
    slRectangleFill(400, 300, 650, 380);
    slSetForeColor(0.3, 0.8, 1.0, 1.0);
    slRectangleOutline(400, 300, 650, 380);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawText("NEW DISCOVERY!", 260, 440, 20, 25, 16);

    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawText(activeDiscovery->name, 100.0f + (600.0f - activeDiscovery->name.length()*14.0f)/2.0f, 395, 18, 22, 14);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(120, 370, 680, 370);

    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawWrappedText(activeDiscovery->shortDescription, 120, 330, 560, 14, 18, 10, 24);

    slSetForeColor(0.5, 0.5, 0.5, 1.0);
    font.drawText("PRESS [ENTER] TO RESUME EXPLORATION", 210, 150, 12, 16, 9);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawLevelCompleteInfo() {
    if (!activeDiscovery) return;

    slSetForeColor(0.02, 0.02, 0.06, 0.94);
    slRectangleFill(400, 300, 700, 420);
    slSetForeColor(0.2, 0.9, 0.2, 1.0);
    slRectangleOutline(400, 300, 700, 420);

    slSetForeColor(0.2, 0.9, 0.2, 1.0);
    font.drawText("LEVEL COMPLETE!", 260, 460, 22, 28, 17);

    slSetForeColor(0.6, 0.6, 0.6, 1.0);
    font.drawText("LOG ENCRYPTED PHENOMENON:", 230, 420, 12, 15, 9);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(80, 395, 720, 395);

    slSetForeColor(0.95, 0.95, 0.95, 1.0);
    font.drawWrappedText(activeDiscovery->longDescription, 85, 355, 630, 14, 18, 10, 24);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawText("PRESS [ENTER] FOR TRANSITION QUIZ", 210, 120, 12, 16, 9);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawQuestion() {
    if (!activeDiscovery) return;

    slSetForeColor(0.02, 0.02, 0.06, 0.94);
    slRectangleFill(400, 300, 720, 450);
    slSetForeColor(0.3, 0.8, 1.0, 1.0);
    slRectangleOutline(400, 300, 720, 450);

    // Heading
    slSetForeColor(0.3, 0.8, 1.0, 1.0);
    font.drawText("TRANSITION QUIZ - LEVEL " + to_string(currentLevel), 190, 480, 18, 22, 14);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(60, 450, 740, 450);

    // Question
    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawWrappedText(activeDiscovery->question, 70, 410, 660, 15, 20, 11, 26);

    // Options
    float optY = 320.0f;
    float optSpacingY = 36.0f;
    char optionLabel[] = "A";

    for (int i = 0; i < 4; ++i) {
        optionLabel[0] = 'A' + i;
        std::string optionText = std::string(optionLabel) + ") " + activeDiscovery->options[i];
        
        if (quizAnswered) {
            if (i == activeDiscovery->correctAnswerIndex) {
                slSetForeColor(0.2, 0.9, 0.2, 1.0); // Green for correct answer
            } else if (i == quizSelectedAnswer) {
                slSetForeColor(0.9, 0.2, 0.2, 1.0); // Red for selected wrong answer
            } else {
                slSetForeColor(0.5, 0.5, 0.5, 1.0); // Grey for unselected wrong options
            }
        } else {
            slSetForeColor(0.9, 0.9, 0.9, 1.0);
        }
        
        font.drawText(optionText, 80, optY - i * optSpacingY, 13, 16, 9);
    }

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(60, 180, 740, 180);

    // Result or prompt
    if (!quizAnswered) {
        slSetForeColor(0.7, 0.7, 0.7, 1.0);
        font.drawText("PRESS [A], [B], [C], OR [D] TO CHOOSE", 210, 135, 13, 16, 10);
    } else {
        if (quizAnswerCorrect) {
            slSetForeColor(0.2, 0.9, 0.2, 1.0);
            font.drawText("CORRECT ANSWER! +100 SCORE", 230, 145, 14, 18, 11);
        } else {
            slSetForeColor(0.9, 0.2, 0.2, 1.0);
            std::string correctStr = std::string(1, 'A' + activeDiscovery->correctAnswerIndex);
            font.drawText("INCORRECT! CORRECT ANSWER WAS: " + correctStr, 170, 145, 14, 18, 11);
        }
        slSetForeColor(1.0, 0.9, 0.2, 1.0);
        font.drawText("PRESS [ENTER] TO ENTER WORMHOLE", 230, 105, 12, 16, 9);
    }
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawWormholeTransition() {
    // Visual effect: render background of level
    slSprite(bgTex[currentLevel - 1], bgX1, 300, 800, 600);
    slSprite(bgTex[currentLevel - 1], bgX2, 300, 800, 600);

    // Draw player inside wormhole
    dragon.render();

    // Draw wormhole overlay
    slPush();
    slTranslate(400, 300);
    slRotate(wormholeRotation);
    slSprite(wormholeTex, 0, 0, 800 * wormholeScale, 600 * wormholeScale);
    slPop();
}

void Game::drawEnding() {
    // Render deep space background
    slSprite(bgTex[5], 400, 300, 800, 600);

    // Semi transparent overlay
    slSetForeColor(0.01, 0.01, 0.03, 0.85);
    slRectangleFill(400, 300, 800, 600);
    slSetForeColor(1, 1, 1, 1);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawText("MISSION COMPLETE", 220, 480, 24, 30, 20);

    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawWrappedText("The Dragon was pulled deep into the core of the Black Hole anomaly. Fulfilling the final sequence, it released its life energy - a force tied to Earth's vital ecosystems.", 100, 400, 600, 13, 16, 9, 22);

    font.drawWrappedText("This energy successfully stabilized the gravitational distortions, saving Earth's system from collapse. In the story, the connection between biological life and cosmic gravity cores has been preserved.", 100, 310, 600, 13, 16, 9, 22);

    slSetForeColor(0.2, 0.9, 0.2, 1.0);
    font.drawText("FINAL SCORE: " + to_string(score), 290, 200, 16, 20, 12);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawText("PRESS [ENTER] TO RETURN TO MENU", 230, 100, 13, 17, 10);
    slSetForeColor(1, 1, 1, 1);
}

void Game::render() {
    slSetForeColor(1, 1, 1, 1);

    if (currentState == GameState::MENU) {
        drawMenu();
    } 
    else if (currentState == GameState::ENCYCLOPEDIA) {
        // Draw background from menu
        slSprite(menuBgTex, 400, 300, 800, 600);
        encyclopediaManager.render(discoveryManager, font);
    }
    else if (currentState == GameState::GAME_OVER) {
        drawGameOver();
    }
    else if (currentState == GameState::ENDING) {
        drawEnding();
    }
    else if (currentState == GameState::WORMHOLE_TRANSITION) {
        drawWormholeTransition();
    }
    else {
        // Active gameplay / popup screens
        // Render backgrounds
        slSprite(bgTex[currentLevel - 1], bgX1, 300, 800, 600);
        slSprite(bgTex[currentLevel - 1], bgX2, 300, 800, 600);

        // Render entities
        for (auto& obs : obstacles) {
            obs.render();
        }
        dragon.render();

        // Render HUD
        drawHUD();

        // Render popups over the game
        if (currentState == GameState::INFO_POPUP) {
            drawInfoPopup();
        } 
        else if (currentState == GameState::LEVEL_COMPLETE_INFO) {
            drawLevelCompleteInfo();
        } 
        else if (currentState == GameState::QUESTION) {
            drawQuestion();
        }
    }
}
