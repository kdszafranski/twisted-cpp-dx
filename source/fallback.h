// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provded by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

#ifndef _BREAKOUT_H_             // prevent multiple definitions if this 
#define _BREAKOUT_H_             // ..file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "game.h"
#include <vector>
#include <list>
#include "textureManager.h"
#include "image.h"
#include "Player.h"
#include "ball.h"
#include "block.h"
#include "textDX.h"
#include "button.h"
#include "textButton.h"
#include "console.h"
#include "editor.h"
#include "AnimationManager.h"
#include "level.h"
#include "Explosion.h"
#include <memory>
#include "PowerUp.h"

// Should be in enums.h but includes are a jumbled uber-mess
ePlayerMoveDirection& operator+=(ePlayerMoveDirection& orig, int) {
    orig = static_cast<ePlayerMoveDirection>(orig + 1);
    if (orig > LEFT) {
        orig = UP;
        return orig;
    } else {
        return orig;
    }
}


//=============================================================================
// Create game class
//=============================================================================
class Fallback : public Game
{
private:
    // variables
    // testing
    bool skipTitleScreen = true;
    int animId;

    std::vector<Block> blocks;
    std::vector<Level> levels;
    std::list<Image> racers;
    bool gameOver = false;
    bool bIsMoving = false;
    bool hasPowerUp = false;
    bool ballResetting = false;
    bool titleLoading = false;
    bool bIsOnPath = false;
    int score;
    int ballCount;
    int currentLevel;
    float racerSpawnTimer;
    float powerUpTimer;
    float powerUpTimeLimit;
    float titleLoadingTimer;

    // Game objects
    POWERUP currentPowerUp;
    TextDX dxScoreFont;
    TextDX dxBallCount;
    Console console;
    Editor* editor;
    AnimationManager m_AnimationManager;
    Explosion explosionManager;
    PowerUp* fallingPowerUpPtr;

    // textures/sprites
    TextureManager backgroundTexture;
    TextureManager titleTexture;
    TextureManager iconTexture;
    TextureManager playerTexture;
    TextureManager detailsTexture;
    TextureManager floorTexture;
    TextureManager buttonTexture;
    TextureManager gameOverTexture;
    TextureManager powerUpTexture;
    TextureManager logoTexture;
    Image backgroundImage;
    Image titleImage;
    Image gameOverImage;
    Image logoImage;
    Button newGameButton;
    Button editorButton;
    Button creditsButton;
    // ui images
    Image ballCountXImage;
    Image ballCountIcon;
    Image uiCurrentPowerUpIcon;
    Image uiCurrentPowerUpDiamond;
    COLOR_ARGB currentPowerUpColor;

    // ball shadow
    float timer;
    Image shadowBallImage;
    std::vector<VECTOR2> recentBallPositions;

    // sprites
    Player player;
    Ball ball;
    Block block;

    // pausing
    bool isPaused;


public:
    // Constructor
    Fallback();

    // Destructor
    virtual ~Fallback();

    // Initialize the game
    void initialize(HWND hwnd);
    void resetGame();

    // Game Loop stuff
    void update(float frameTime);      // must override pure virtual from Game
    void updateTitleScreen(float frameTime);
    void updateGameScreen(float frameTime);
    void updateFloorTiles(float frameTime, ePlayerMoveDirection pDir);

    void updateGameOverScreen(float frameTime);
    // update extraneous elements
    void updateEffects(float frameTime);
    
    void ai();          // "
    void collisions() override;  // "
    void render();  // render game objects
    void renderTitleScreen();
    void renderRacers();
    void renderGameScreen();
    void renderUI(); // display UI
    void releaseAll();
    void resetAll();
    void loseBall();    // reduce ball count
    bool isGameOver();  // check if we're out of balls
    // do game over things
    void shakeScreen();
    void handleGameOver();
    // periodically spawns details in the bg
    void spawnRacers();
    // actually creates the racer animation instances
    void spawnRacerAnimation(Vector2 position);
    void cleanUpRacerList();
    
    // gameplay methods
    void removeBlock(int index);
    void restartBall();
    void launchBall();
    COLOR_ARGB getBallCountColor();
    bool isValidLocation(int x, int y);
    void spawnPowerUp(VECTOR2 position);
    void applyPowerUp();
    void removePowerUp();

    // Game state/flow
    void startNewGame();
    void initBackgrounds();
    void initButtons();
    void initSprites();
    void initPlayerArrow();
    void initBall();
    void initUI();
    void initFloor();
    void initMessageSprites();
    void setTitleScreen();
    void setGameScreen();
    void exitGame();
    void setEditorScreen();
    // checks if there are blocks left
    void checkGameOver();

    // Editor
    void launchEditor();
    void exitEditor();

    // level loading
    void loadLevelFiles();  // loads all levels from disk
    // Advances the level
    void startNextLevel();
    // Loads given level
    void loadLevel(int level);
    // loads a randomly generated level
    void loadRandomLevel();
    // loads the level from a file
    bool loadLevelFromFile(int n);


    // maze gen methods
    Vec2Int makeStraightaway(int distance, ePlayerMoveDirection direction, int x, int y);

    // helper to handle exit inputs
    void checkForExit(); 
    // helper to handle pause input
    void checkPauseInput(); 
    // helper to handle cheat inputs
    void checkCheatInput();

    AnimationManager* getAnimationManager() { return &m_AnimationManager; }
};

#endif
