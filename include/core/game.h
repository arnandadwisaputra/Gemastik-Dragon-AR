#pragma once
#include "entity/asteroid.h"
#include "entity/dragon.h"
#include "rendering/BitmapFont.h"
#include "ui/DiscoveryManager.h"
#include "ui/EncyclopediaManager.h"
#include "core/VideoPlayer.h"
#include <string>
#include <vector>

enum class GameState {
  MENU = 0,
  OPENING_CUTSCENE,
  GAMEPLAY_BRIEFING,
  PLAYING,
  PAUSE,
  INFO_POPUP,
  LEVEL_COMPLETE_INFO,
  QUESTION,
  WORMHOLE_TRANSITION,
  MID_CUTSCENE,
  GAME_OVER,
  ENDING_CUTSCENE,
  MISSION_COMPLETE,
  ENDING,
  ENCYCLOPEDIA
};

class Game {
public:
  void load();
  void update();
  void render();

private:
  // Core states
  GameState currentState = GameState::MENU;
  GameState previousState = GameState::MENU;
  int currentLevel = 1;
  float levelTimer = 0.0f;
  int lives = 3;
  int score = 0;
  int highScore = 0;
  bool debugMode = false;

  // Active selections
  int menuSelectedIndex = 0;
  int pauseSelectedIndex = 0;
  int gameOverSelectedIndex = 0;
  int quizSelectedIndex = 0;

  // Mouse tracking
  float lastMouseX = -1.0f;
  float lastMouseY = -1.0f;

  // Assets
  int bgTex[6];      // Backgrounds for levels 1-6
  int deepSpaceTex;  // Deep space looping background for Level 1
  int menuBgTex;     // Main menu background
  int menuTitleTex;  // Main menu title logo sprite
  int menuTitleWidth = 1;
  int menuTitleHeight = 1;
  int gameOverBgTex; // Game over background
  int heartTex;      // Heart icon for lives
  int wormholeTex;   // Wormhole sprite for transition
  int endingBgTex;   // Ending screen background

  // Buttons (using text now, but keep textures if needed)
  int startBtnTex, retryBtnTex, exitBtnTex;

  // Audio
  int bgm;
  int hitSfx;
  int winSfx;    // Sfx for correct answer / level complete
  int selectSfx; // Sfx for menu select

  // Entities
  Dragon dragon;
  std::vector<Asteroid> obstacles;
  const int maxObstacles = 15;

  // Managers
  BitmapFont font;
  DiscoveryManager discoveryManager;
  EncyclopediaManager encyclopediaManager;
  VideoPlayer videoPlayer;
  bool musicPlaying = false;

  // Background scrolling
  float bgX1 = 400.0f;
  float bgX2 = 1200.0f;
  float bgSpeed = 120.0f;

  // Spawning parameters
  float spawnTimer = 0.0f;
  float baseSpawnInterval = 1.8f;
  float difficultyMultiplier = 1.0f;
  float difficultyRate = 0.015f; // Speed/spawn frequency increases by this rate per second

  // Active popup states
  const Discovery *activeDiscovery = nullptr;
  std::string activePopupText = "";
  int quizSelectedAnswer = -1; // -1: none, 0-3: A-D
  bool quizAnswered = false;
  bool quizAnswerCorrect = false;

  // Wormhole effect states
  float wormholeTimer = 0.0f;
  float wormholeScale = 0.0f;
  float wormholeRotation = 0.0f;

  // Dash ready text blink state
  float dashTextBlinkTimer = 0.0f;
  float fadeInTimer = 0.0f;

  // Helper functions
  float getLevelDuration() const { return debugMode ? 10.0f : 180.0f; }
  bool isClicked(float x, float y, float w, float h);
  bool isHovered(float x, float y, float w, float h);
  void resetLevel(int lvl);
  void startNextLevel();
  void spawnObstacle();
  void triggerInfoPopup(const std::string &name);
  void triggerLevelComplete();
  void stopGameMusic();
  void startGameMusic();
  void beginCutscene(const std::string &videoFile);
  void finishCutscene();
  
  // Custom UI Design System Drawing Helpers
  void drawRoundedRect(float x, float y, float w, float h, float r);
  void drawCenteredText(const std::string& text, float centerX, float centerY, float sizeX = 16.0f, float sizeY = 20.0f, float spacing = 12.0f);
  void drawButton(float x, float y, float w, float h, const std::string& label, bool isSelected);
  void drawScienceTicker(float dt);
  void drawPauseMenu();

  void drawHUD();
  void drawMenu();
  void drawGameplayBriefing();
  void drawMissionComplete();
  void drawGameOver();
  void drawInfoPopup();
  void drawLevelCompleteInfo();
  void drawQuestion();
  void drawWormholeTransition();
  void drawEnding();
  void saveHighScore();
  void loadHighScore();
};
