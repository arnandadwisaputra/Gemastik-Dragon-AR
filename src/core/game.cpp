#include "core/game.h"
#include "core/utils.h"
#include "core/Localization.h"
#include <sl.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

using namespace std;

void Game::load() {
    // 1. Load Backgrounds
    bgTex[0] = slLoadTexture(Utils::getAssetPath("background", "bg_level1_earth.png.png").c_str());
    bgTex[1] = slLoadTexture(Utils::getAssetPath("background", "bg_level2_asteroid_belt_tile.png").c_str());
    bgTex[2] = slLoadTexture(Utils::getAssetPath("background", "bg_level3_solar_activity_tile.png").c_str());
    bgTex[3] = slLoadTexture(Utils::getAssetPath("background", "bg_level4_deep_space_tile.png").c_str());
    bgTex[4] = slLoadTexture(Utils::getAssetPath("background", "bg_level5_nebula_tile.png").c_str());
    bgTex[5] = slLoadTexture(Utils::getAssetPath("background", "bg_level6_deep_space_tile.png").c_str());

    deepSpaceTex = slLoadTexture(Utils::getAssetPath("background", "bg_space_deep_tile.png.png").c_str());
    menuBgTex = slLoadTexture(Utils::getAssetPath("ui", "menu_bg.jpg").c_str());
    gameOverBgTex = slLoadTexture(Utils::getAssetPath("ui", "game_over.jpg").c_str());
    heartTex = slLoadTexture(Utils::getAssetPath("ui", "heart.png").c_str());

    // Load the main menu logo title sprite
    string titlePath = Utils::getAssetPath("ui", "dragon-asteroid-run-title.jpeg");
    menuTitleTex = slLoadTexture(titlePath.c_str());
    if (!Utils::getImageDimensions(titlePath, menuTitleWidth, menuTitleHeight)) {
        menuTitleWidth = 450;
        menuTitleHeight = 110;
    }

    // Load wormhole transition texture from effects directory
    wormholeTex = slLoadTexture(Utils::getAssetPath("effects", "wormhole-transition.png").c_str());

    // 2. Load Audio
    bgm = slLoadWAV(Utils::getAssetPath("audio", "bgm.wav").c_str());
    hitSfx = slLoadWAV(Utils::getAssetPath("audio", "hit.wav").c_str());
    winSfx = slLoadWAV(Utils::getAssetPath("audio", "hit.wav").c_str()); // Fallback or reload same
    selectSfx = slLoadWAV(Utils::getAssetPath("audio", "hit.wav").c_str());

    slSoundLoop(bgm);
    musicPlaying = true;

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

    // Initialize selectors and tracking
    menuSelectedIndex = 0;
    pauseSelectedIndex = 0;
    gameOverSelectedIndex = 0;
    quizSelectedIndex = 0;
    lastMouseX = -1.0f;
    lastMouseY = -1.0f;
    fadeInTimer = 0.0f;
}

bool Game::isHovered(float x, float y, float w, float h) {
    float mx = (float)slGetMouseX();
    float my = (float)slGetMouseY();
    return (mx > x - w / 2.0f && mx < x + w / 2.0f && my > y - h / 2.0f && my < y + h / 2.0f);
}

bool Game::isClicked(float x, float y, float w, float h) {
    if (slGetMouseButton(SL_MOUSE_BUTTON_LEFT)) {
        return isHovered(x, y, w, h);
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
    fadeInTimer = 0.6f; // Smooth fade-in

    // Clear active obstacles
    for (auto& obs : obstacles) {
        obs.reset();
    }

    // Reset player
    dragon.reset();

    // Trigger instant discoveries for specific levels
    if (currentLevel == 2) {
        triggerInfoPopup("ASTEROID");
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
        beginCutscene("ending.mp4");
        currentState = GameState::ENDING_CUTSCENE;
    } else {
        resetLevel(currentLevel);
        currentState = GameState::PLAYING;
    }
}

void Game::stopGameMusic() {
    if (musicPlaying) {
        slSoundStop(bgm);
        musicPlaying = false;
    }
}

void Game::startGameMusic() {
    if (!musicPlaying) {
        slSoundLoop(bgm);
        musicPlaying = true;
    }
}

void Game::beginCutscene(const string& videoFile) {
    stopGameMusic();
    videoPlayer.close();
    HWND hwnd = FindWindowA(nullptr, "Dragon Asteroid Run");
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
    string path = Utils::getAssetPath("video", videoFile);
    if (!videoPlayer.open(path)) {
        std::cerr << "Failed to open cutscene: " << path << std::endl;
    } else {
        videoPlayer.play();
    }
}

void Game::finishCutscene() {
    HWND hwnd = FindWindowA(nullptr, "Dragon Asteroid Run");
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        SetActiveWindow(hwnd);
    }
    videoPlayer.close();
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
    else if (currentLevel == 2) name = "ASTEROID";
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
        // Level 4: Deep space comets + meteoroids + asteroids
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
    if (dt > 0.15f) dt = 0.15f;

    // Track mouse coordinates to check for actual movement (hybrid input navigation)
    float mx = (float)slGetMouseX();
    float my = (float)slGetMouseY();
    bool mouseMoved = (lastMouseX != -1.0f) && (mx != lastMouseX || my != lastMouseY);
    lastMouseX = mx;
    lastMouseY = my;

    // MENU STATE
    if (currentState == GameState::MENU) {
        // Toggle debug mode with key D
        static bool lastDPressed = false;
        bool dPressed = slGetKey('D');
        if (dPressed && !lastDPressed) {
            debugMode = !debugMode;
        }
        lastDPressed = dPressed;

        static bool lastUpMenu = false;
        static bool lastDownMenu = false;
        bool upMenu = slGetKey(SL_KEY_UP) || slGetKey('W');
        bool downMenu = slGetKey(SL_KEY_DOWN) || slGetKey('S');
        
        if (upMenu && !lastUpMenu) {
            menuSelectedIndex = (menuSelectedIndex - 1 + 4) % 4;
            slSoundPlay(selectSfx);
        }
        if (downMenu && !lastDownMenu) {
            menuSelectedIndex = (menuSelectedIndex + 1) % 4;
            slSoundPlay(selectSfx);
        }
        lastUpMenu = upMenu;
        lastDownMenu = downMenu;

        // Mouse hover selection update (only if mouse coordinates actually changed)
        if (mouseMoved) {
            if (isHovered(400, 310, 220, 46)) menuSelectedIndex = 0;
            else if (isHovered(400, 240, 220, 46)) menuSelectedIndex = 1;
            else if (isHovered(400, 170, 220, 46)) menuSelectedIndex = 2;
            else if (isHovered(400, 100, 220, 46)) menuSelectedIndex = 3;
        }

        static bool lastEnter = false;
        static bool lastSpace = false;
        bool enterPressed = slGetKey(SL_KEY_ENTER);
        bool spacePressed = slGetKey(' ');
        bool activated = (enterPressed && !lastEnter) || (spacePressed && !lastSpace);
        lastEnter = enterPressed;
        lastSpace = spacePressed;

        // Check activation
        if (activated || slGetMouseButton(SL_MOUSE_BUTTON_LEFT)) {
            bool clickedStart = isClicked(400, 310, 220, 46);
            bool clickedEnc = isClicked(400, 240, 220, 46);
            bool clickedLang = isClicked(400, 170, 220, 46);
            bool clickedExit = isClicked(400, 100, 220, 46);

            if ((activated && menuSelectedIndex == 0) || clickedStart) {
                slSoundPlay(selectSfx);
                score = 0;
                lives = 3;
                beginCutscene("opening.mp4");
                currentState = GameState::OPENING_CUTSCENE;
            }
            else if ((activated && menuSelectedIndex == 1) || clickedEnc) {
                slSoundPlay(selectSfx);
                previousState = GameState::MENU;
                currentState = GameState::ENCYCLOPEDIA;
                encyclopediaManager.resetSelection();
            }
            else if ((activated && menuSelectedIndex == 2) || clickedLang) {
                slSoundPlay(selectSfx);
                if (Loc::getLanguage() == Language::ENGLISH) {
                    Loc::setLanguage(Language::INDONESIAN);
                    discoveryManager.localize(Language::INDONESIAN);
                } else {
                    Loc::setLanguage(Language::ENGLISH);
                    discoveryManager.localize(Language::ENGLISH);
                }
            }
            else if ((activated && menuSelectedIndex == 3) || clickedExit) {
                slClose();
                exit(0);
            }
        }
        return;
    }

    // OPENING CUTSCENE
    if (currentState == GameState::OPENING_CUTSCENE) {
        if (!videoPlayer.isOpen()) {
            finishCutscene();
            currentState = GameState::GAMEPLAY_BRIEFING;
            startGameMusic();
            return;
        }
        videoPlayer.update();
        if (videoPlayer.isFinished()) {
            finishCutscene();
            currentState = GameState::GAMEPLAY_BRIEFING;
            startGameMusic();
        }
        return;
    }

    // GAMEPLAY BRIEFING
    if (currentState == GameState::GAMEPLAY_BRIEFING) {
        static bool lastEnterBrief = false;
        static bool lastSpaceBrief = false;
        bool enterBrief = slGetKey(SL_KEY_ENTER);
        bool spaceBrief = slGetKey(' ');
        bool activatedBrief = (enterBrief && !lastEnterBrief) || (spaceBrief && !lastSpaceBrief);
        lastEnterBrief = enterBrief;
        lastSpaceBrief = spaceBrief;

        if (activatedBrief || isClicked(400, 100, 220, 46)) {
            slSoundPlay(selectSfx);
            resetLevel(1);
            currentState = GameState::PLAYING;
            startGameMusic();
        }
        return;
    }

    // MID CUTSCENE (after Level 5)
    if (currentState == GameState::MID_CUTSCENE) {
        if (!videoPlayer.isOpen()) {
            finishCutscene();
            resetLevel(6);
            if (currentState != GameState::INFO_POPUP) {
                currentState = GameState::PLAYING;
            }
            startGameMusic();
            return;
        }
        videoPlayer.update();
        if (videoPlayer.isFinished()) {
            finishCutscene();
            resetLevel(6);
            if (currentState != GameState::INFO_POPUP) {
                currentState = GameState::PLAYING;
            }
            startGameMusic();
        }
        return;
    }

    // ENDING CUTSCENE (after Level 6)
    if (currentState == GameState::ENDING_CUTSCENE) {
        if (!videoPlayer.isOpen()) {
            finishCutscene();
            currentState = GameState::MISSION_COMPLETE;
            startGameMusic();
            return;
        }
        videoPlayer.update();
        if (videoPlayer.isFinished()) {
            finishCutscene();
            currentState = GameState::MISSION_COMPLETE;
            startGameMusic();
        }
        return;
    }

    // MISSION COMPLETE
    if (currentState == GameState::MISSION_COMPLETE) {
        static bool lastEnterMission = false;
        static bool lastSpaceMission = false;
        bool enterMission = slGetKey(SL_KEY_ENTER);
        bool spaceMission = slGetKey(' ');
        bool activatedMission = (enterMission && !lastEnterMission) || (spaceMission && !lastSpaceMission);
        lastEnterMission = enterMission;
        lastSpaceMission = spaceMission;

        if (activatedMission || isClicked(400, 90, 220, 46)) {
            slSoundPlay(selectSfx);
            bool isNew = false;
            discoveryManager.unlockPhenomenon("EARTH", isNew);
            previousState = GameState::MISSION_COMPLETE;
            currentState = GameState::ENCYCLOPEDIA;
            encyclopediaManager.selectEarthEntry();
        }
        return;
    }

    // ENCYCLOPEDIA STATE
    if (currentState == GameState::ENCYCLOPEDIA) {
        encyclopediaManager.update(discoveryManager);
        if (slGetKey(SL_KEY_ESCAPE)) {
            currentState = previousState;
            slSoundPlay(selectSfx);
            if (previousState == GameState::MISSION_COMPLETE) {
                currentState = GameState::MENU;
                startGameMusic();
            }
        }
        return;
    }

    // PAUSE STATE
    if (currentState == GameState::PAUSE) {
        static bool lastUpPause = false;
        static bool lastDownPause = false;
        bool upPause = slGetKey(SL_KEY_UP) || slGetKey('W');
        bool downPause = slGetKey(SL_KEY_DOWN) || slGetKey('S');
        
        if (upPause && !lastUpPause) {
            pauseSelectedIndex = (pauseSelectedIndex - 1 + 3) % 3;
            slSoundPlay(selectSfx);
        }
        if (downPause && !lastDownPause) {
            pauseSelectedIndex = (pauseSelectedIndex + 1) % 3;
            slSoundPlay(selectSfx);
        }
        lastUpPause = upPause;
        lastDownPause = downPause;

        // Mouse hover selection update (only if mouse coordinates actually changed)
        if (mouseMoved) {
            if (isHovered(400, 320, 220, 46)) pauseSelectedIndex = 0;
            else if (isHovered(400, 250, 220, 46)) pauseSelectedIndex = 1;
            else if (isHovered(400, 180, 220, 46)) pauseSelectedIndex = 2;
        }

        static bool lastEnterP = false;
        static bool lastSpaceP = false;
        bool enterP = slGetKey(SL_KEY_ENTER);
        bool spaceP = slGetKey(' ');
        bool activatedPause = (enterP && !lastEnterP) || (spaceP && !lastSpaceP);
        lastEnterP = enterP;
        lastSpaceP = spaceP;

        if (activatedPause || slGetMouseButton(SL_MOUSE_BUTTON_LEFT)) {
            bool clickedResume = isClicked(400, 320, 220, 46);
            bool clickedEnc = isClicked(400, 250, 220, 46);
            bool clickedMenu = isClicked(400, 180, 220, 46);
            
            if ((activatedPause && pauseSelectedIndex == 0) || clickedResume) {
                currentState = GameState::PLAYING;
                slSoundPlay(selectSfx);
            }
            else if ((activatedPause && pauseSelectedIndex == 1) || clickedEnc) {
                previousState = GameState::PAUSE;
                currentState = GameState::ENCYCLOPEDIA;
                encyclopediaManager.resetSelection();
                slSoundPlay(selectSfx);
            }
            else if ((activatedPause && pauseSelectedIndex == 2) || clickedMenu) {
                currentState = GameState::MENU;
                slSoundPlay(selectSfx);
                startGameMusic();
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }
            }
        }
        return;
    }

    // GAME OVER STATE
    if (currentState == GameState::GAME_OVER) {
        static bool lastLeftGO = false;
        static bool lastRightGO = false;
        bool leftGO = slGetKey(SL_KEY_LEFT) || slGetKey('A') || slGetKey(SL_KEY_UP) || slGetKey('W');
        bool rightGO = slGetKey(SL_KEY_RIGHT) || slGetKey('D') || slGetKey(SL_KEY_DOWN) || slGetKey('S');
        
        if ((leftGO && !lastLeftGO) || (rightGO && !lastRightGO)) {
            gameOverSelectedIndex = (gameOverSelectedIndex + 1) % 2;
            slSoundPlay(selectSfx);
        }
        lastLeftGO = leftGO;
        lastRightGO = rightGO;

        // Mouse hover selection update (only if mouse coordinates actually changed)
        if (mouseMoved) {
            if (isHovered(300, 160, 180, 46)) gameOverSelectedIndex = 0;
            else if (isHovered(500, 160, 180, 46)) gameOverSelectedIndex = 1;
        }

        static bool lastEnterGO = false;
        static bool lastSpaceGO = false;
        bool enterGO = slGetKey(SL_KEY_ENTER);
        bool spaceGO = slGetKey(' ');
        bool activatedGO = (enterGO && !lastEnterGO) || (spaceGO && !lastSpaceGO);
        lastEnterGO = enterGO;
        lastSpaceGO = spaceGO;

        if (activatedGO || slGetMouseButton(SL_MOUSE_BUTTON_LEFT)) {
            bool clickedRetry = isClicked(300, 160, 180, 46);
            bool clickedMenu = isClicked(500, 160, 180, 46);

            if ((activatedGO && gameOverSelectedIndex == 0) || clickedRetry) {
                lives = 3;
                resetLevel(currentLevel);
                currentState = GameState::PLAYING;
                slSoundPlay(selectSfx);
            }
            else if ((activatedGO && gameOverSelectedIndex == 1) || clickedMenu) {
                currentState = GameState::MENU;
                slSoundPlay(selectSfx);
                startGameMusic();
            }
        }
        return;
    }

    // ENDING STATE
    if (currentState == GameState::ENDING) {
        bool clickedEnd = isClicked(400, 100, 320, 46);
        if (slGetKey(SL_KEY_ENTER) || slGetKey(' ') || slGetKey(SL_KEY_ESCAPE) || clickedEnd) {
            currentState = GameState::MENU;
            slSoundPlay(selectSfx);
        }
        return;
    }

    // POPUP STATES (PAUSED MECHANICS)
    if (currentState == GameState::INFO_POPUP) {
        if (slGetKey(SL_KEY_ENTER) || slGetKey(' ') || isClicked(400, 160, 200, 46)) {
            currentState = GameState::PLAYING;
            slSoundPlay(selectSfx);
        }
        return;
    }

    if (currentState == GameState::LEVEL_COMPLETE_INFO) {
        if (slGetKey(SL_KEY_ENTER) || slGetKey(' ') || isClicked(400, 150, 220, 46)) {
            currentState = GameState::QUESTION;
            quizSelectedAnswer = -1;
            quizSelectedIndex = 0; // Reset question index to first option (A)
            quizAnswered = false;
            slSoundPlay(selectSfx);
        }
        return;
    }

    if (currentState == GameState::QUESTION) {
        if (!quizAnswered) {
            static bool lastUpQ = false;
            static bool lastDownQ = false;
            bool upQ = slGetKey(SL_KEY_UP) || slGetKey('W');
            bool downQ = slGetKey(SL_KEY_DOWN) || slGetKey('S');
            
            if (upQ && !lastUpQ) {
                quizSelectedIndex = (quizSelectedIndex - 1 + 5) % 5;
                slSoundPlay(selectSfx);
            }
            if (downQ && !lastDownQ) {
                quizSelectedIndex = (quizSelectedIndex + 1) % 5;
                slSoundPlay(selectSfx);
            }
            lastUpQ = upQ;
            lastDownQ = downQ;

            // Mouse hover selection update (only if mouse coordinates actually changed)
            if (mouseMoved) {
                float optY = 320.0f;
                float optSpacingY = 36.0f;
                for (int i = 0; i < 4; ++i) {
                    if (isHovered(400, optY - i * optSpacingY, 660, 30)) {
                        quizSelectedIndex = i;
                    }
                }
                if (isHovered(400, 100, 240, 36)) {
                    quizSelectedIndex = 4;
                }
            }

            int directAnswer = -1;
            if (slGetKey('A')) directAnswer = 0;
            else if (slGetKey('B')) directAnswer = 1;
            else if (slGetKey('C')) directAnswer = 2;
            else if (slGetKey('D')) directAnswer = 3;

            bool shortcutBack = slGetKey('I'); // 'I' shortcut for Back to Info

            static bool lastEnterQ = false;
            static bool lastSpaceQ = false;
            bool enterQ = slGetKey(SL_KEY_ENTER);
            bool spaceQ = slGetKey(' ');
            bool activatedQ = (enterQ && !lastEnterQ) || (spaceQ && !lastSpaceQ);
            lastEnterQ = enterQ;
            lastSpaceQ = spaceQ;

            // Process option clicks or keys
            if (directAnswer != -1) {
                quizSelectedAnswer = directAnswer;
                quizAnswered = true;
                if (quizSelectedAnswer == activeDiscovery->correctAnswerIndex) {
                    quizAnswerCorrect = true;
                    score += 100;
                    slSoundPlay(winSfx);
                } else {
                    quizAnswerCorrect = false;
                    score -= 50;
                    if (score < 0) score = 0;
                    slSoundPlay(hitSfx);
                }
            }
            else if (shortcutBack || (activatedQ && quizSelectedIndex == 4) || isClicked(400, 100, 240, 36)) {
                currentState = GameState::LEVEL_COMPLETE_INFO;
                slSoundPlay(selectSfx);
            }
            else if (activatedQ && quizSelectedIndex >= 0 && quizSelectedIndex <= 3) {
                quizSelectedAnswer = quizSelectedIndex;
                quizAnswered = true;
                if (quizSelectedAnswer == activeDiscovery->correctAnswerIndex) {
                    quizAnswerCorrect = true;
                    score += 100;
                    slSoundPlay(winSfx);
                } else {
                    quizAnswerCorrect = false;
                    score -= 50;
                    if (score < 0) score = 0;
                    slSoundPlay(hitSfx);
                }
            }
            else {
                // Check mouse clicks on options A-D directly
                float optY = 320.0f;
                float optSpacingY = 36.0f;
                for (int i = 0; i < 4; ++i) {
                    float optionCenterY = optY - i * optSpacingY;
                    if (isClicked(400, optionCenterY, 660, 30)) {
                        quizSelectedAnswer = i;
                        quizAnswered = true;
                        if (quizSelectedAnswer == activeDiscovery->correctAnswerIndex) {
                            quizAnswerCorrect = true;
                            score += 100;
                            slSoundPlay(winSfx);
                        } else {
                            quizAnswerCorrect = false;
                            score -= 50;
                            if (score < 0) score = 0;
                            slSoundPlay(hitSfx);
                        }
                        break;
                    }
                }
            }
        } else {
            if (slGetKey(SL_KEY_ENTER) || slGetKey(' ') || isClicked(400, 105, 300, 36)) {
                currentState = GameState::WORMHOLE_TRANSITION;
                wormholeTimer = 0.0f;
                wormholeScale = 0.0f;
                wormholeRotation = 0.0f;
                slSoundPlay(selectSfx);
            }
        }
        return;
    }

    if (currentState == GameState::WORMHOLE_TRANSITION) {
        wormholeTimer += dt;
        wormholeRotation += 180.0f * dt;
        
        if (wormholeTimer < 0.8f) {
            wormholeScale = (wormholeTimer / 0.8f) * 1.5f;
        } else if (wormholeTimer < 1.5f) {
            wormholeScale = 1.5f;
        } else {
            wormholeScale = 1.5f + (wormholeTimer - 1.5f) * 2.0f;
        }

        // Pull dragon to center
        float px = dragon.getX();
        float py = dragon.getY();
        float dx = 400.0f - px;
        float dy = 300.0f - py;
        dragon.setPosition(px + dx * 4.0f * dt, py + dy * 4.0f * dt);

        if (wormholeTimer >= 2.2f) {
            if (currentLevel == 5) {
                beginCutscene("mid.mp4");
                currentState = GameState::MID_CUTSCENE;
            } else {
                startNextLevel();
            }
        }
        return;
    }

    // ACTIVE PLAYING STATE
    if (currentState == GameState::PLAYING) {
        // Toggle pause menu with Escape key (de-bounced)
        static bool lastEscPressed = false;
        bool escPressed = slGetKey(SL_KEY_ESCAPE);
        if (escPressed && !lastEscPressed) {
            currentState = GameState::PAUSE;
            pauseSelectedIndex = 0;
            slSoundPlay(selectSfx);
        }
        lastEscPressed = escPressed;

        levelTimer += dt;

        // Decrement fade in timer
        if (fadeInTimer > 0.0f) {
            fadeInTimer -= dt;
            if (fadeInTimer < 0.0f) fadeInTimer = 0.0f;
        }

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
            // Black Hole gravitational pull (environmental core / central force)
            float dx = 400.0f - dragon.getX();
            float dy = 300.0f - dragon.getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            // Sucked in condition
            if (dist < 40.0f) {
                slSoundPlay(winSfx);
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }
                beginCutscene("ending.mp4");
                currentState = GameState::ENDING_CUTSCENE;
                return;
            }

            if (dist > 5.0f) {
                // Pull progression (pull increases as time progresses and as distance shrinks)
                float basePull = 150.0f;
                float timeFactor = levelTimer * 15.0f;
                float closeFactor = 400.0f * (1.0f - dist / 400.0f);
                if (closeFactor < 0.0f) closeFactor = 0.0f;
                
                float pullAcc = basePull + timeFactor + closeFactor;
                
                gravityAccX = (dx / dist) * pullAcc;
                gravityAccY = (dy / dist) * pullAcc;
            }
        } 
        else if (currentLevel == 5) {
            // Drifting gravity sources (influence-radius based gravity)
            for (auto& obs : obstacles) {
                if (obs.isActive() && (obs.getType() == ObstacleType::PULSAR || obs.getType() == ObstacleType::GRAVITY_WELL)) {
                    float dx = obs.getX() - dragon.getX();
                    float dy = obs.getY() - dragon.getY();
                    float dist = std::sqrt(dx * dx + dy * dy);

                    float influenceRadius = 250.0f;
                    float innerRadius = 125.0f;

                    if (dist < influenceRadius && dist > 5.0f) {
                        float pullAcc = 0.0f;
                        if (dist >= innerRadius) {
                            // Outer zone: pull is lighter (0 to 200)
                            pullAcc = 200.0f * ((influenceRadius - dist) / innerRadius);
                        } else {
                            // Inner zone: pull is stronger (200 to 600)
                            pullAcc = 200.0f + 400.0f * ((innerRadius - dist) / innerRadius);
                        }

                        gravityAccX += (dx / dist) * pullAcc;
                        gravityAccY += (dy / dist) * pullAcc;
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
                        case ObstacleType::ASTEROID_BELT: item = "ASTEROID"; break;
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
                        gameOverSelectedIndex = 0; // Default index focus to Retry
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

void Game::drawRoundedRect(float x, float y, float w, float h, float r) {
    if (r > w / 2.0f) r = w / 2.0f;
    if (r > h / 2.0f) r = h / 2.0f;
    
    slRectangleFill(x, y, w - 2 * r, h);
    slRectangleFill(x, y, w, h - 2 * r);
    
    slCircleFill(x - w / 2.0f + r, y + h / 2.0f - r, r, 16);
    slCircleFill(x + w / 2.0f - r, y + h / 2.0f - r, r, 16);
    slCircleFill(x - w / 2.0f + r, y - h / 2.0f + r, r, 16);
    slCircleFill(x + w / 2.0f - r, y - h / 2.0f + r, r, 16);
}

void Game::drawCenteredText(const string& text, float centerX, float centerY, float sizeX, float sizeY, float spacing) {
    float startX = centerX - (text.length() - 1) * spacing / 2.0f;
    font.drawText(text, startX, centerY, sizeX, sizeY, spacing);
}

void Game::drawButton(float x, float y, float w, float h, const string& label, bool isSelected) {
    // 1. Draw rounded borders and backgrounds
    if (isSelected) {
        slSetForeColor(0.7f, 0.4f, 0.9f, 1.0f); // Violet border
        drawRoundedRect(x, y, w + 4, h + 4, 12.0f);
        
        slSetForeColor(0.3f, 0.1f, 0.5f, 0.95f); // Violet fill
        drawRoundedRect(x, y, w, h, 10.0f);
    } else {
        slSetForeColor(0.3f, 0.15f, 0.45f, 1.0f); // Darker border
        drawRoundedRect(x, y, w + 2, h + 2, 11.0f);
        
        slSetForeColor(0.15f, 0.08f, 0.28f, 0.9f); // Dark purple fill
        drawRoundedRect(x, y, w, h, 10.0f);
    }
    
    // 2. Reusable text fitting calculation with padding
    float paddingX = 16.0f;
    float maxWidth = w - paddingX;
    if (maxWidth < 20.0f) maxWidth = 20.0f;
    
    float baseSizeX = 15.0f;
    float baseSizeY = 18.0f;
    float baseSpacing = 11.0f;
    
    float baseWidth = label.length() * baseSpacing;
    float scale = 1.0f;
    if (baseWidth > maxWidth) {
        scale = maxWidth / baseWidth;
    }
    
    float sizeX = baseSizeX * scale;
    float sizeY = baseSizeY * scale;
    float spacing = baseSpacing * scale;
    float offsetY = -7.0f * scale; // Adjust vertical centering offset proportionally
    
    // 3. Render text
    if (isSelected) {
        slSetForeColor(1.0f, 0.9f, 0.2f, 1.0f); // Yellow highlight text
    } else {
        slSetForeColor(0.9f, 0.9f, 0.9f, 1.0f);
    }
    
    drawCenteredText(label, x, y + offsetY, sizeX, sizeY, spacing);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawScienceTicker(float dt) {
    slSetForeColor(0.04f, 0.02f, 0.1f, 0.85f);
    slRectangleFill(400, 25, 800, 30);
    
    slSetForeColor(0.4f, 0.2f, 0.6f, 1.0f);
    slLine(0, 40, 800, 40);
    slLine(0, 10, 800, 10);
    
    string tickerText = Loc::tr("ticker.level" + to_string(currentLevel));
    float textWidth = tickerText.length() * 10.0f;
    
    static int lastLevel = 1;
    static float tickerX = 800.0f;
    if (currentLevel != lastLevel) {
        tickerX = 800.0f;
        lastLevel = currentLevel;
    }
    
    tickerX -= 90.0f * dt;
    if (tickerX < -textWidth) {
        tickerX = 800.0f;
    }
    
    slSetForeColor(0.3f, 0.8f, 1.0f, 1.0f); // Cyan text
    font.drawText(tickerText, tickerX, 20, 13, 16, 10);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawPauseMenu() {
    slSetForeColor(0.02f, 0.02f, 0.05f, 0.75f);
    slRectangleFill(400, 300, 800, 600);
    
    slSetForeColor(0.08f, 0.04f, 0.16f, 0.95f);
    drawRoundedRect(400, 270, 320, 360, 16.0f);
    
    slSetForeColor(0.4f, 0.2f, 0.6f, 1.0f);
    slRectangleOutline(400, 270, 320, 360);
    
    slSetForeColor(0.3f, 0.8f, 1.0f, 1.0f);
    drawCenteredText(Loc::tr("pause.title"), 400, 410, 22, 28, 16);
    
    slSetForeColor(0.4f, 0.4f, 0.4f, 1.0f);
    slLine(270, 385, 530, 385);
    
    drawButton(400, 320, 220, 46, Loc::tr("pause.resume"), pauseSelectedIndex == 0);
    drawButton(400, 250, 220, 46, Loc::tr("pause.encyclopedia"), pauseSelectedIndex == 1);
    drawButton(400, 180, 220, 46, Loc::tr("pause.menu"), pauseSelectedIndex == 2);
}

void Game::drawHUD() {
    slSetForeColor(1, 1, 1, 1);
    font.drawText(Loc::tr("hud.score") + to_string(score), 40, 560, 14, 18, 10);
    font.drawText(Loc::tr("hud.level") + to_string(currentLevel), 250, 560, 14, 18, 10);

    for (int i = 0; i < lives; ++i) {
        slSprite(heartTex, 620.0f + (i * 35.0f), 570.0f, 25.0f, 25.0f);
    }

    if (currentLevel == 6) {
        dashTextBlinkTimer += (float)slGetDeltaTime();
        if (dragon.isDashReady()) {
            if (fmod(dashTextBlinkTimer, 0.6f) < 0.3f) {
                slSetForeColor(1.0, 0.9, 0.2, 1.0);
                font.drawText(Loc::tr("hud.dash_ready"), 290, 50, 14, 18, 10);
            }
        } else {
            slSetForeColor(0.5, 0.5, 0.5, 1.0);
            font.drawText(Loc::tr("hud.dash_charging"), 320, 50, 14, 18, 10);
        }
        slSetForeColor(1, 1, 1, 1);
    }
}

void Game::drawMenu() {
    slSprite(menuBgTex, 400, 300, 800, 600);

    // Draw main menu logo title sprite with preserved aspect ratio
    float maxLogoW = 420.0f;
    float maxLogoH = 140.0f;
    float logoAspect = (float)menuTitleWidth / (float)menuTitleHeight;
    float logoW = maxLogoW;
    float logoH = logoW / logoAspect;
    if (logoH > maxLogoH) {
        logoH = maxLogoH;
        logoW = logoH * logoAspect;
    }
    slSprite(menuTitleTex, 400, 480, logoW, logoH);

    slSetForeColor(1, 1, 1, 1);
    drawCenteredText(Loc::tr("menu.highscore") + to_string(highScore), 400, 380, 15, 20, 11);

    drawButton(400, 310, 220, 46, Loc::tr("menu.start"), menuSelectedIndex == 0);
    drawButton(400, 240, 220, 46, Loc::tr("menu.encyclopedia"), menuSelectedIndex == 1);
    drawButton(400, 170, 220, 46, Loc::tr("menu.language"), menuSelectedIndex == 2);
    drawButton(400, 100, 220, 46, Loc::tr("menu.exit"), menuSelectedIndex == 3);

    if (debugMode) {
        slSetForeColor(1.0, 0.2, 0.2, 1.0);
        drawCenteredText(Loc::tr("menu.debug_active"), 400, 45, 13, 16, 9);
    } else {
        slSetForeColor(0.4, 0.4, 0.4, 1.0);
        drawCenteredText(Loc::tr("menu.debug"), 400, 45, 13, 16, 9);
    }
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawGameplayBriefing() {
    slSprite(menuBgTex, 400, 300, 800, 600);

    slSetForeColor(0.5f, 0.25f, 0.75f, 1.0f);
    drawRoundedRect(400, 300, 684, 464, 18.0f);

    slSetForeColor(0.04f, 0.02f, 0.1f, 0.92f);
    drawRoundedRect(400, 300, 680, 460, 16.0f);

    slSetForeColor(1.0f, 0.9f, 0.2f, 1.0f);
    drawCenteredText(Loc::tr("briefing.line1"), 400, 470, 18, 22, 14);

    slSetForeColor(0.9f, 0.9f, 0.9f, 1.0f);
    drawCenteredText(Loc::tr("briefing.line2"), 400, 430, 14, 18, 10);
    drawCenteredText(Loc::tr("briefing.line3"), 400, 400, 14, 18, 10);
    drawCenteredText(Loc::tr("briefing.line4"), 400, 360, 14, 18, 10);
    drawCenteredText(Loc::tr("briefing.line5"), 400, 330, 14, 18, 10);
    drawCenteredText(Loc::tr("briefing.line6"), 400, 300, 14, 18, 10);

    slSetForeColor(0.3f, 0.8f, 1.0f, 1.0f);
    drawCenteredText(Loc::tr("briefing.controls"), 400, 240, 13, 16, 9);
    drawCenteredText(Loc::tr("briefing.dash"), 400, 210, 12, 15, 8);

    bool btnHovered = isHovered(400, 100, 220, 46);
    drawButton(400, 100, 220, 46, Loc::tr("briefing.continue"), btnHovered);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawMissionComplete() {
    slSprite(bgTex[5], 400, 300, 800, 600);

    slSetForeColor(0.01f, 0.01f, 0.03f, 0.88f);
    slRectangleFill(400, 300, 800, 600);

    slSetForeColor(0.5f, 0.25f, 0.75f, 1.0f);
    drawRoundedRect(400, 300, 704, 484, 18.0f);

    slSetForeColor(0.04f, 0.02f, 0.1f, 0.94f);
    drawRoundedRect(400, 300, 700, 480, 16.0f);

    slSetForeColor(1.0f, 0.9f, 0.2f, 1.0f);
    drawCenteredText(Loc::tr("mission.title"), 400, 490, 22, 28, 17);
    drawCenteredText(Loc::tr("mission.subtitle"), 400, 450, 14, 18, 10);

    slSetForeColor(0.4f, 0.4f, 0.4f, 1.0f);
    slLine(80, 425, 720, 425);

    slSetForeColor(0.9f, 0.9f, 0.9f, 1.0f);
    font.drawWrappedText(Loc::tr("mission.line1"), 90, 395, 620, 13, 16, 9, 22);
    font.drawWrappedText(Loc::tr("mission.line2"), 90, 340, 620, 13, 16, 9, 22);
    font.drawWrappedText(Loc::tr("mission.line3"), 90, 280, 620, 13, 16, 9, 22);
    font.drawWrappedText(Loc::tr("mission.line4"), 90, 230, 620, 13, 16, 9, 22);

    slSetForeColor(0.2f, 0.9f, 0.2f, 1.0f);
    drawCenteredText(Loc::tr("mission.line5"), 400, 175, 15, 19, 11);

    slSetForeColor(1.0f, 0.9f, 0.2f, 1.0f);
    drawCenteredText(Loc::tr("mission.hook"), 400, 140, 13, 16, 9);

    bool btnHovered = isHovered(400, 90, 220, 46);
    drawButton(400, 90, 220, 46, Loc::tr("mission.continue"), btnHovered);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawGameOver() {
    slSprite(gameOverBgTex, 400, 300, 800, 600);

    slSetForeColor(1, 1, 1, 1);
    drawCenteredText(Loc::tr("gameover.failed") + to_string(currentLevel), 400, 380, 16, 20, 12);
    drawCenteredText(Loc::tr("gameover.score") + to_string(score), 400, 320, 16, 20, 12);

    drawButton(300, 160, 180, 46, Loc::tr("gameover.retry"), gameOverSelectedIndex == 0);
    drawButton(500, 160, 180, 46, Loc::tr("gameover.menu"), gameOverSelectedIndex == 1);
}

void Game::drawInfoPopup() {
    if (!activeDiscovery) return;

    slSetForeColor(0.04f, 0.02f, 0.1f, 0.94f);
    drawRoundedRect(400, 300, 650, 380, 16.0f);
    
    slSetForeColor(0.5f, 0.25f, 0.75f, 1.0f);
    slRectangleOutline(400, 300, 650, 380);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    drawCenteredText(Loc::tr("popup.new_discovery"), 400, 440, 20, 25, 16);

    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    string displayName = activeDiscovery->name;
    replace(displayName.begin(), displayName.end(), '_', ' ');
    drawCenteredText(displayName, 400, 395, 18, 22, 14);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(120, 370, 680, 370);

    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawWrappedText(activeDiscovery->shortDescription, 120, 330, 560, 14, 18, 10, 24);

    bool btnHovered = isHovered(400, 160, 200, 46);
    drawButton(400, 160, 200, 46, Loc::tr("pause.resume"), btnHovered);
    
    slSetForeColor(0.5, 0.5, 0.5, 1.0);
    drawCenteredText(Loc::tr("popup.resume"), 400, 105, 11, 14, 8);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawLevelCompleteInfo() {
    if (!activeDiscovery) return;

    slSetForeColor(0.04f, 0.02f, 0.1f, 0.94f);
    drawRoundedRect(400, 300, 700, 420, 16.0f);
    
    slSetForeColor(0.5f, 0.25f, 0.75f, 1.0f);
    slRectangleOutline(400, 300, 700, 420);

    slSetForeColor(0.2, 0.9, 0.2, 1.0);
    drawCenteredText(Loc::tr("level.complete"), 400, 460, 22, 28, 17);

    slSetForeColor(0.6, 0.6, 0.6, 1.0);
    drawCenteredText(Loc::tr("level.complete_log"), 400, 420, 12, 15, 9);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(80, 395, 720, 395);

    slSetForeColor(0.95, 0.95, 0.95, 1.0);
    font.drawWrappedText(activeDiscovery->longDescription, 85, 355, 630, 14, 18, 10, 24);

    bool btnHovered = isHovered(400, 150, 220, 46);
    drawButton(400, 150, 220, 46, Loc::tr("pause.resume"), btnHovered);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    drawCenteredText(Loc::tr("level.complete_next"), 400, 100, 11, 14, 8);
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawQuestion() {
    if (!activeDiscovery) return;

    slSetForeColor(0.04f, 0.02f, 0.1f, 0.94f);
    drawRoundedRect(400, 300, 720, 450, 16.0f);
    
    slSetForeColor(0.5f, 0.25f, 0.75f, 1.0f);
    slRectangleOutline(400, 300, 720, 450);

    slSetForeColor(0.3, 0.8, 1.0, 1.0);
    drawCenteredText(Loc::tr("quiz.title") + to_string(currentLevel), 400, 480, 18, 22, 14);

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(60, 450, 740, 450);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    font.drawWrappedText(activeDiscovery->question, 70, 410, 660, 15, 20, 11, 26);

    float optY = 320.0f;
    float optSpacingY = 36.0f;
    char optionLabel[] = "A";

    for (int i = 0; i < 4; ++i) {
        float optionCenterY = optY - i * optSpacingY;
        bool isOptHovered = !quizAnswered && (quizSelectedIndex == i);
        
        if (isOptHovered) {
            slSetForeColor(0.2f, 0.1f, 0.35f, 0.6f);
            drawRoundedRect(400, optionCenterY, 660, 30, 6.0f);
        }

        optionLabel[0] = 'A' + i;
        string optionText = string(optionLabel) + ") " + activeDiscovery->options[i];
        
        if (quizAnswered) {
            if (i == activeDiscovery->correctAnswerIndex) {
                slSetForeColor(0.2, 0.9, 0.2, 1.0);
            } else if (i == quizSelectedAnswer) {
                slSetForeColor(0.9, 0.2, 0.2, 1.0);
            } else {
                slSetForeColor(0.5, 0.5, 0.5, 1.0);
            }
        } else {
            if (isOptHovered) {
                slSetForeColor(1.0, 0.9, 0.2, 1.0);
            } else {
                slSetForeColor(0.9, 0.9, 0.9, 1.0);
            }
        }
        
        font.drawText(optionText, 80, optionCenterY - 6.0f, 13, 16, 9);
    }

    slSetForeColor(0.4, 0.4, 0.4, 1.0);
    slLine(60, 180, 740, 180);

    if (!quizAnswered) {
        slSetForeColor(0.7, 0.7, 0.7, 1.0);
        drawCenteredText(Loc::tr("quiz.instructions"), 400, 135, 13, 16, 10);
        
        // Render Back to Info button (indices: A-D are 0-3, Back to Info is 4)
        drawButton(400, 100, 240, 36, Loc::tr("quiz.back_to_info"), quizSelectedIndex == 4);
    } else {
        if (quizAnswerCorrect) {
            slSetForeColor(0.2, 0.9, 0.2, 1.0);
            drawCenteredText(Loc::tr("quiz.correct"), 400, 145, 14, 18, 11);
        } else {
            slSetForeColor(0.9, 0.2, 0.2, 1.0);
            string correctStr = string(1, 'A' + activeDiscovery->correctAnswerIndex);
            drawCenteredText(Loc::tr("quiz.incorrect") + correctStr, 400, 145, 14, 18, 11);
        }
        
        bool btnHovered = isHovered(400, 105, 300, 36);
        drawButton(400, 105, 300, 36, Loc::tr("quiz.next"), btnHovered);
    }
    slSetForeColor(1, 1, 1, 1);
}

void Game::drawWormholeTransition() {
    if (currentLevel == 1) {
        slSprite(deepSpaceTex, 400, 300, 800, 600);
        slSprite(bgTex[0], 400, 300, 800, 600);
    } else {
        slSprite(bgTex[currentLevel - 1], bgX1, 300, 800, 600);
        slSprite(bgTex[currentLevel - 1], bgX2, 300, 800, 600);
    }

    for (auto& obs : obstacles) {
        obs.render();
    }

    dragon.render();

    slPush();
    slTranslate(400, 300);
    slRotate(wormholeRotation);
    slSprite(wormholeTex, 0, 0, 800 * wormholeScale, 600 * wormholeScale);
    slPop();

    if (wormholeTimer > 1.5f) {
        float alpha = (wormholeTimer - 1.5f) / 0.7f;
        if (alpha > 1.0f) alpha = 1.0f;
        slSetForeColor(0, 0, 0, alpha);
        slRectangleFill(400, 300, 800, 600);
        slSetForeColor(1, 1, 1, 1);
    }
}

void Game::drawEnding() {
    slSprite(bgTex[5], 400, 300, 800, 600);

    slSetForeColor(0.01, 0.01, 0.03, 0.85);
    slRectangleFill(400, 300, 800, 600);
    slSetForeColor(1, 1, 1, 1);

    slSetForeColor(1.0, 0.9, 0.2, 1.0);
    drawCenteredText(Loc::tr("ending.title"), 400, 480, 24, 30, 20);

    slSetForeColor(0.9, 0.9, 0.9, 1.0);
    font.drawWrappedText(Loc::tr("ending.text1"), 100, 400, 600, 13, 16, 9, 22);
    font.drawWrappedText(Loc::tr("ending.text2"), 100, 310, 600, 13, 16, 9, 22);

    slSetForeColor(0.2, 0.9, 0.2, 1.0);
    drawCenteredText(Loc::tr("ending.score") + to_string(score), 400, 200, 16, 20, 12);

    bool btnHovered = isHovered(400, 100, 320, 46);
    drawButton(400, 100, 320, 46, Loc::tr("ending.return"), btnHovered);
    slSetForeColor(1, 1, 1, 1);
}

void Game::render() {
    slSetForeColor(1, 1, 1, 1);

    if (currentState == GameState::MENU) {
        drawMenu();
    }
    else if (currentState == GameState::OPENING_CUTSCENE ||
             currentState == GameState::MID_CUTSCENE ||
             currentState == GameState::ENDING_CUTSCENE) {
        slSetForeColor(0, 0, 0, 1);
        slRectangleFill(400, 300, 800, 600);
        slSetForeColor(1, 1, 1, 1);
    }
    else if (currentState == GameState::GAMEPLAY_BRIEFING) {
        drawGameplayBriefing();
    }
    else if (currentState == GameState::MISSION_COMPLETE) {
        drawMissionComplete();
    }
    else if (currentState == GameState::ENCYCLOPEDIA) {
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
        // Active gameplay / popup screens / pause
        if (currentLevel == 1) {
            slSprite(deepSpaceTex, bgX1, 300, 800, 600);
            slSprite(deepSpaceTex, bgX2, 300, 800, 600);
            slSprite(bgTex[0], 400, 300, 800, 600);
        } else {
            slSprite(bgTex[currentLevel - 1], bgX1, 300, 800, 600);
            slSprite(bgTex[currentLevel - 1], bgX2, 300, 800, 600);
        }

        for (auto& obs : obstacles) {
            obs.render();
        }
        dragon.render();

        drawHUD();

        // Render Science Ticker when playing
        if (currentState == GameState::PLAYING) {
            drawScienceTicker((float)slGetDeltaTime());
        }

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
        else if (currentState == GameState::PAUSE) {
            drawPauseMenu();
        }

        // Render fade-in overlay if active
        if (currentState == GameState::PLAYING && fadeInTimer > 0.0f) {
            float alpha = fadeInTimer / 0.6f;
            slSetForeColor(0, 0, 0, alpha);
            slRectangleFill(400, 300, 800, 600);
            slSetForeColor(1, 1, 1, 1);
        }
    }
}
