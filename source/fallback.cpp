// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provded by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

using namespace std;
#include "fallback.h"
#include <time.h>
#include "level.h"
#include <fstream>
#include <iostream>
#include "editor.h"
#include "fileHandler.h"
#include "Explosion.h"
// Animations
#include "FadeTo.h"
#include "PinchScale.h"
#include "PunchScale.h"
#include "DirectionBounce.h"
#include "MoveTo.h"
#include "ScaleXTo.h"
#include "Shake.h"

//=============================================================================
// Constructor
//=============================================================================
Fallback::Fallback()
{
	editor = new Editor();
	fallingPowerUpPtr = NULL;
	racerSpawnTimer = 0;
	hasPowerUp = false;
	bIsMoving = false;
	powerUpTimer = 0;
	titleLoadingTimer = 0;
	titleLoading = false;
	currentPowerUp = FAST; // not actually applied, null would be better
	animId = 0;
	timer = 0;
	score = 0;
	isPaused = false;
}

//=============================================================================
// Destructor
//=============================================================================
Fallback::~Fallback()
{
	releaseAll();           // call onLostDevice() for every graphics item

	// remove all running animations
	racers.clear();
	m_AnimationManager.abortAllProcesses(true);

	SAFE_DELETE(fallingPowerUpPtr);
	SAFE_DELETE(editor);
}


//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Fallback::initialize(HWND hwnd)
{
	Game::initialize(hwnd); // throws GameError

	initBackgrounds();
	initButtons();

	// Init DirectX font 
	if (dxScoreFont.initialize(graphics, 62, true, false, "Agdasima") == false)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing score font"));

	if (dxBallCount.initialize(graphics, 34, true, false, "Agdasima") == false)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ball count font"));

	// init the console log
	console.initialize(graphics);

	// for testing
	if (skipTitleScreen) {
		startNewGame();
	} else {
		setTitleScreen();
	}

	//audio->playCue(MUSIC_LOOP);

	return;
}


/// <summary>
/// Begins a new game from the Title Screen
/// </summary>
void Fallback::startNewGame()
{
	// set proper bg screen state
	setGameScreen();

	// load game sprites
	initSprites();

	// stop any other animations
	m_AnimationManager.clearAllProcesses();
	explosionManager.clearAllParticles();
	racers.clear();

	// reset game variables
	resetGame();

	loadRandomLevel();

	// play!
	//restartBall();
}

/// <summary>
/// Resets score, board, and general game state
/// </summary>
void Fallback::resetGame()
{
	hasPowerUp = false;
	gameOver = false;
	isPaused = false;
	ballCount = MAX_BALLS;
	timer = 0;
	powerUpTimer = 0;
	score = 0;
	currentLevel = 0; // points into levels vector, 0 is the first level
	console.resetLog();
}

void Fallback::exitGame()
{
	console.setLogText("");
	isPaused = false;
	ballCount = 0;

	// remove animations
	explosionManager.clearAllParticles();
	m_AnimationManager.clearAllProcesses();

	// clean up game
	blocks.clear();
	racers.clear();

	SAFE_DELETE(fallingPowerUpPtr);

	// go to main menu
	setTitleScreen();
}

//=============================================================================
// Initializes all the game sprites from textures
//=============================================================================
void Fallback::initSprites() {
	// misc graphics
	initMessageSprites();
	// create our game object and graphics
	initPlayerArrow();
	// set up the blocks
	initFloor();
	// ball sprites
	initBall();
	// ui/hud
	initUI();

	// power ups texture
	if (!powerUpTexture.initialize(graphics, POWERUP_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing power up texture"));
	}

}


/// <summary>
/// Load background image(s)
/// </summary>
void Fallback::initBackgrounds()
{
	// background texture
	if (!backgroundTexture.initialize(graphics, BG_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing background texture"));
	}
	// game bg image
	if (!backgroundImage.initialize(graphics, 0, 0, 2, &backgroundTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game bg image"));
	}
	// title
	if (!titleTexture.initialize(graphics, TITLE_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing title texture"));
	}

	// game bg image
	if (!titleImage.initialize(graphics, 0, 0, 1, &titleTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing title image"));
	}
	titleImage.setPosition(GAME_WIDTH / 2 - titleImage.getWidth() / 2 + 5, 178);
	//titleImage.setColorFilter(graphicsNS::WHITE & graphicsNS::ALPHA25);
}

void Fallback::initButtons()
{
	// buttons texture
	if (!buttonTexture.initialize(graphics, NG_BUTTON_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing button texture"));
	}

	// create buttons from above spritesheet/texture
	if (!newGameButton.initialize(this, 256, 64, 3, &buttonTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing button image"));
	}
	newGameButton.setCurrentFrame(0);
	newGameButton.setPosition(400 - newGameButton.getSpriteData().width / 2, 356);

}

void Fallback::initMessageSprites()
{
	// background texture
	if (!gameOverTexture.initialize(graphics, GAME_OVER_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing message texture"));
	}

	// game bg image
	if (!gameOverImage.initialize(graphics, 0, 0, 0, &gameOverTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game over image"));
	}
	gameOverImage.setPosition(0, GAME_HEIGHT / 2);



}

//=============================================================================
// Player texture and entity init
//=============================================================================
void Fallback::initPlayerArrow()
{
	if (!shipTexture.initialize(graphics, PLAYER_PATH)) {
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing player texture"));
	}
	if (!player.initialize(this, playerNS::WIDTH, playerNS::HEIGHT, playerNS::TEXTURE_COLS, &shipTexture))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing player entity"));

	//ship.setFrames(playerNS::SHIP_START_FRAME, playerNS::SHIP_END_FRAME);
	player.setCurrentFrame(0);

	// start center, near the bottom
	player.setX(GAME_WIDTH / 2 - playerNS::WIDTH / 2);
	player.setY(GAME_HEIGHT / 2 - playerNS::HEIGHT / 2);
	player.setVelocity(VECTOR2(0, 0)); // start standing still

}


//=============================================================================
// Initialize ball texture/images
//=============================================================================
void Fallback::initBall()
{
	if (!iconTexture.initialize(graphics, ICONS_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ball texture"));
	}
	if (!ball.initialize(this, ballNS::WIDTH, ballNS::HEIGHT, ballNS::TEXTURE_COLS, &iconTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ball entity"));
	}
	ball.setCurrentFrame(0);

	// ball shadow image
	if (!shadowBallImage.initialize(graphics, ballNS::WIDTH, ballNS::HEIGHT, ballNS::TEXTURE_COLS, &iconTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ball shadow image"));
	}
	shadowBallImage.setCurrentFrame(4);

}

void Fallback::initUI()
{
	// ball count icon image
	if (!ballCountIcon.initialize(this->graphics, ballNS::WIDTH, ballNS::HEIGHT, ballNS::TEXTURE_COLS, &iconTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ball count icon"));
	}
	ballCountIcon.setCurrentFrame(0);
	ballCountIcon.setPosition(736, 68);

	// ball count X icon
	if (!ballCountXImage.initialize(graphics, ballNS::WIDTH, ballNS::HEIGHT, ballNS::TEXTURE_COLS, &iconTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ball count X icon"));
	}
	ballCountXImage.setCurrentFrame(1);
	ballCountXImage.setPosition(ballCountIcon.getX() + ballCountIcon.getWidth() + 4, ballCountIcon.getY());

	// power up stuff
	if (!uiCurrentPowerUpDiamond.initialize(this->graphics, powerupNS::WIDTH, powerupNS::HEIGHT, powerupNS::TEXTURE_COLS, &powerUpTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing power up UI icon"));
	}
	uiCurrentPowerUpDiamond.setCurrentFrame(7);
	uiCurrentPowerUpDiamond.setPosition(GAME_WIDTH / 2 - powerupNS::WIDTH, GAME_HEIGHT - powerupNS::HEIGHT * 1.5);

	if (!uiCurrentPowerUpIcon.initialize(this->graphics, powerupNS::WIDTH, powerupNS::HEIGHT, powerupNS::TEXTURE_COLS, &powerUpTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing power up Letter"));
	}
	uiCurrentPowerUpIcon.setCurrentFrame(currentPowerUp);
	uiCurrentPowerUpIcon.setPosition(uiCurrentPowerUpDiamond.getPosition());

}

//=============================================================================
// Initialize block texture/images
//=============================================================================
void Fallback::initFloor()
{
	// load our texture, reuse it for all Floor Entities
	if (!floorTexture.initialize(graphics, TILE_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Floor texture"));
	}
}

void Fallback::loadLevelFiles() {
	levels.clear();
	//loadLevelFromFile(0); // Editor default
	//loadLevelFromFile(1);
	//loadLevelFromFile(2);
	//loadLevelFromFile(3);
}

void Fallback::startNextLevel()
{
	// nothing to see here
}

/// <summary>
/// Creates the blocks in the vector from the list given level data which was previously loaded
/// </summary>
/// <param name="levelNumber">number that matches level number</param>
void Fallback::loadLevel(int levelNumber)
{
	const int START_X = 114;
	const int START_Y = 100;
	const int COLS = 9;
	const int ROWS = 3;

	blocks.clear();
	blocks.reserve(21); // 27 is max space needed

	// load up vector with blocks from the level data
	int y = START_Y;
	for (int i = 0; i < ROWS; i++) {

		int x = START_X;
		for (int j = 0; j < COLS; j++) {

			if (levels.at(levelNumber).data.at(i * COLS + j) == NONE) {
				// skip
			} else {
				Block newBlock;

				if (!newBlock.initialize(this, blockNS::WIDTH, blockNS::HEIGHT, blockNS::TEXTURE_COLS, &floorTexture))
				{
					throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing block entity"));
				}
				newBlock.setCurrentFrame(0);
				newBlock.setPosition(x, y);
				newBlock.setVelocity(VECTOR2(0, 0));

				// add to vector
				blocks.push_back(newBlock);
			}

			// move to the right
			x += blockNS::WIDTH;
		}

		// set new row downward
		y += blockNS::HEIGHT;
	}
}

/// <summary>
/// Generates text paths
/// </summary>
void Fallback::loadRandomLevel()
{
	const int START_X = player.getX();
	const int START_Y = player.getY();

	blocks.clear();
	blocks.reserve(21);

	srand((unsigned)time(0));
	int y = START_Y;
	int x = START_X;
	for (int i = 0; i < 10; i++) {
		Block newBlock;

		if (!newBlock.initialize(this, blockNS::WIDTH, blockNS::HEIGHT, blockNS::TEXTURE_COLS, &floorTexture))
		{
			throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing block entity"));
		}

		newBlock.setPosition(x, y);
		newBlock.setVelocity(VECTOR2(0, 0));

		// add to vector
		blocks.push_back(newBlock);


		// set new row downward
		y -= blockNS::HEIGHT;
	}
}

//=============================================================================
// Loads a level from disk
//=============================================================================
bool Fallback::loadLevelFromFile(int n)
{
	Level loadedLevel;
	FileHandler loader;

	if (loader.loadLevelFromFile(loadedLevel, n)) {
		// editor level
		if (n == 0) {
			// replacing
			if (levels.size() > 0) {
				levels.at(0) = loadedLevel;
				return true;
			}
		}

		levels.push_back(loadedLevel);
	} else {
		return false;
	}

	return true;
}

#pragma region Update
//=============================================================================
// Update all game items
//=============================================================================
void Fallback::update(float frameTime)
{
	// handle inputs on Title Screen only
	if (currentScreen == TITLE) {
		// process animations
		m_AnimationManager.updateProcesses(frameTime);
		updateTitleScreen(frameTime);
	}

	// handle Game updates and inputs
	if (currentScreen == GAME) {
		CheckPauseInput();
		if (!gameOver) {
			CheckCheatInput();
		}

		if (!isPaused) {
			if (!gameOver) {
				// update position of all game objects
				updateGameScreen(frameTime);
			} else {
				// game over
				updateGameOverScreen(frameTime);
			} // end game over

			// always update effects
			//updateEffects(frameTime);

		} // isPaused

	} // GAME screen

	if (currentScreen == EDITOR) {
		editor->update(frameTime);
		// process animations
		m_AnimationManager.updateProcesses(frameTime);
	}

	// Always update the following
	// every 5 seconds there is a chance to spawn racers
	if (!isPaused) {
	}

	// check if we want to exit
	CheckForExit();
}

/// <summary>
/// Updates elements for the title screen only, including a timer for some elements
/// </summary>
/// <param name="frameTime">current frame time</param>
void Fallback::updateTitleScreen(float frameTime)
{
	// wait for bg and title to fade in
	titleLoadingTimer += frameTime;
	if (titleLoadingTimer > 2.0f) {
		titleLoadingTimer = 0;
		titleLoading = false;
	}

	if (newGameButton.isMouseOver()) {
		// over, allow clicks
		if (input->getMouseLButton()) {
			startNewGame();
		}
	}

	// too lazy for the mouse
	if (input->wasKeyPressed(ENTER_KEY)) {
		startNewGame();
	}
}

void Fallback::updateGameScreen(float frameTime) {


	player.update(frameTime);

	// update all other sprites to move opposite player move direction
	switch (player.moveDirection) {
		case UP:
			backgroundImage.setY(backgroundImage.getY() + 1);
			break;
		case RIGHT:
			backgroundImage.setX(backgroundImage.getX() - 1);
			break;
		case DOWN:
			backgroundImage.setY(backgroundImage.getY() - 1);
			break;
		case LEFT:
			backgroundImage.setX(backgroundImage.getX() + 1);
			break;
	}

	//if (ballResetting) {
	//	// move ball with ship
	//	ball.setPosition((player.getX() + player.getWidth() / 2) - ball.getWidth() / 2, player.getY() - ball.getHeight() - 1);
	//	// allow input to launch
	//	if (input->wasKeyPressed(PLAYER_UP_KEY)) {
	//		launchBall();
	//	}
	//} else {
	//	ball.update(frameTime);
	//}

	// handle power ups timer
	//if (hasPowerUp) {
	//	powerUpTimer += frameTime;
	//	if (powerUpTimer > POW_TIME_LIMIT) {
	//		removePowerUp();
	//	}
	//}

	// every interval adjust ball trail
	//timer += frameTime;
	//if (timer > BALLSHADOW_INTERVAL) {
	//	recentBallPositions.push_back(VECTOR2(ball.getX(), ball.getY()));

	//	if (recentBallPositions.size() > 5) {
	//		// remove first
	//		recentBallPositions.erase(recentBallPositions.begin());
	//	}
	//	timer = 0;
	//}

	//// check if the ball went off below ship
	//if (ball.getY() > GAME_HEIGHT) {
	//	loseBall();
	//	restartBall();
	//}
}

void Fallback::updateGameOverScreen(float frameTime)
{
	// pick out a block and bounce it
	timer += frameTime;
	float duration = 0.75f;
	if (timer > duration) {
		int index = rand() % blocks.size();
		StrongAnimationPtr animPtr;
		if (index % 2 == 0) {
			animPtr = std::make_shared<PinchScale>(&blocks.at(index), duration, 0.8f);
		} else {
			animPtr = std::make_shared<PunchScale>(&blocks.at(index), duration, 1.2f);
		}
		m_AnimationManager.attachProcess(animPtr);
		timer = 0;
	}
}

/// <summary>
/// Updates particles and effects
/// </summary>
/// <param name="frameTime">current frame time</param>
void Fallback::updateEffects(float frameTime)
{
	// update these unless paused	
	if (fallingPowerUpPtr) {
		fallingPowerUpPtr->update(frameTime);
		// off the screen?
		if (fallingPowerUpPtr->getY() > GAME_HEIGHT) {
			SAFE_DELETE(fallingPowerUpPtr);
		}
	}

	// update particles
	explosionManager.update(frameTime);
	// update Entity tweens/animations
	m_AnimationManager.updateProcesses(frameTime);
}
#pragma endregion

#pragma region Racers
void Fallback::spawnRacers()
{
	// chance
	srand((unsigned)time(0));
	int numberToSpawn = 0;

	numberToSpawn = 3; // rand() % 4;
	Vector2 position = { GAME_WIDTH, rand() % GAME_HEIGHT };
	for (int i = 0; i < numberToSpawn; i++) {
		spawnRacerAnimation(position);
		position.x += 25;
		position.y += 3;
	}

}

void Fallback::spawnRacerAnimation(Vector2 startPos)
{
	Image racersImage;
	racersImage.myId = ++animId;

	if (!racersImage.initialize(graphics, 32, 2, 0, &detailsTexture))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing racers image"));

	racersImage.setPosition(startPos);
	Vector2 end = startPos;
	end.x -= GAME_WIDTH + racersImage.getWidth(); // go  832 pixels from start

	racers.push_back(racersImage);

	//  = rand() % 100 + 1;     // v2 in the range 1 to 100
	StrongAnimationPtr racerMove = std::make_shared<MoveTo>(&racers.back(), rand() % 4 + 2, end);
	m_AnimationManager.attachProcess(racerMove);
}

void Fallback::cleanUpRacerList()
{
	std::list<Image>::iterator it = racers.begin();
	while (it != racers.end()) {
		if (it->canDestroy()) {
			racers.erase(it++);
		}
		if (it != racers.end()) {
			it++;
		}
	}
}
#pragma endregion


#pragma region PowerUps

// Creates the Entity on the screen
void Fallback::spawnPowerUp(VECTOR2 position)
{
	// spawn powerup
	if (fallingPowerUpPtr == NULL) {
		const int n = rand() % 7;
		fallingPowerUpPtr = new PowerUp(static_cast<POWERUP>(n), position);
		fallingPowerUpPtr->initialize(this, 32, 32, 8, &powerUpTexture);
	}
}

// applies the collided Power up
void Fallback::applyPowerUp()
{
	hasPowerUp = true;
	audio->playCue(POWER_UP);
	powerUpTimer = 0;
	currentPowerUp = fallingPowerUpPtr->getPowerUpType();

	if (currentPowerUp == MYSTERY) {
		int pick = rand() % 6; // pick one of the others
		currentPowerUp = static_cast<POWERUP>(pick);
	}

	// apply to the correct Entity and spawn animations
	//currentPowerUp = WARP;
	StrongAnimationPtr anim;
	switch (currentPowerUp) {
		case ZOOM:
			ball.applyPowerUp(currentPowerUp);
			player.applyPowerUp(currentPowerUp);
			break;
		case SLOW: // slow ball
			ball.applyPowerUp(currentPowerUp);
		case FAST: // same as below
		case WARP:
			player.applyPowerUp(currentPowerUp);
			break;
		case GROW:
			anim = std::make_shared<ScaleXTo>(&player, 0.5f, 1.5f);
			m_AnimationManager.attachProcess(anim);
			break;
		case TINY:
			anim = std::make_shared<ScaleXTo>(&player, 0.5f, 0.5f);
			m_AnimationManager.attachProcess(anim);
			break;
	}

	// update HUD icons
	uiCurrentPowerUpIcon.setCurrentFrame(currentPowerUp);
	currentPowerUpColor = fallingPowerUpPtr->getColor();

	// bounce the UI power up icon
	StrongAnimationPtr bounce = std::make_shared<PunchScale>(&uiCurrentPowerUpDiamond, 0.2f, 1.5f);
	m_AnimationManager.attachProcess(bounce);
}

void Fallback::removePowerUp()
{
	if (hasPowerUp) {
		switch (currentPowerUp) {
			case ZOOM: // ball speed
			case SLOW: // ball speed
				ball.removePowerUp();
			case FAST: // ship speed
				player.resetSpeed();
				break;
			case WARP:
				player.removeWrapAround();
				break;
				// allow end of power up animations
			case GROW: // same as below
			case TINY:
				// animate to normal width
				StrongAnimationPtr reset = std::make_shared<ScaleXTo>(&player, 0.5, 1.0f);
				m_AnimationManager.attachProcess(reset);
				break;
		}
	}

	audio->playCue(LOSE_POWERUP);
	hasPowerUp = false;
	powerUpTimer = 0;

}
#pragma endregion

void Fallback::CheckPauseInput()
{
	if (currentScreen == GAME) {
		// SPACE pauses
		if (input->wasKeyPressed(SPACE_KEY)) {
			isPaused = !isPaused;
		}
	}
}


void Fallback::CheckCheatInput()
{
	if (currentScreen == GAME) {
		// next level
		if (input->wasKeyPressed(PLAYER_DOWN_KEY)) {
			startNextLevel();
		}
	}
}

bool Fallback::isGameOver()
{
	if (ballCount < 1) {
		// game over
		return true;
	}

	return false;
}

//=============================================================================
// Reset things so a new ball works like the beginning of the game
//=============================================================================
void Fallback::loseBall()
{
	ballCount--;

	// shake ship and bg for feedback
	shakeScreen();

	if (isGameOver()) {
		handleGameOver();
	} else {
		// clean up, restart ball
		audio->playCue(LOSE_BALL);

		// we lose power ups
		if (hasPowerUp) {
			removePowerUp();
		}

		// remove falling power up, too
		if (fallingPowerUpPtr) {
			const Vector2 place = fallingPowerUpPtr->getPosition();
			explosionManager.spawnExplosion(this, &iconTexture, { place.x, place.y });
			SAFE_DELETE(fallingPowerUpPtr);
		}

		// bounce ball UI icon
		StrongAnimationPtr animPtr = std::make_shared<PunchScale>(&ballCountIcon, 0.2f, 1.5f);
		m_AnimationManager.attachProcess(animPtr);

		restartBall();
	}
}

void Fallback::shakeScreen()
{
	Vector2 shakeLimits = { 10.0f, 10.0f };
	StrongAnimationPtr shipShake = std::make_shared<Shake>(&player, 0.5, shakeLimits);
	m_AnimationManager.attachProcess(shipShake);
	StrongAnimationPtr bgShake = std::make_shared<Shake>(&backgroundImage, 0.5, shakeLimits);
	m_AnimationManager.attachProcess(bgShake);
}

/// <summary>
/// Handles game over tasks and animations
/// </summary>
void Fallback::handleGameOver()
{
	audio->playCue(GAME_OVER);
	// show screen
	gameOver = true;
	// bring in message
	gameOverImage.setColorFilter(graphicsNS::FB_TRANSPARENT);
	StrongAnimationPtr animPtr = std::make_shared<FadeTo>(&gameOverImage, 1.25f, 1.0f);
	m_AnimationManager.attachProcess(animPtr);

	// blow up the ship
	explosionManager.spawnExplosion(this, &iconTexture, { player.getX() + 15, player.getCenterY() });
	explosionManager.spawnExplosion(this, &iconTexture, { player.getX() + player.getWidth() - 15, player.getCenterY() });
	explosionManager.spawnExplosion(this, &iconTexture, { player.getX() + player.getCenterX(), player.getCenterY() });
}

//=============================================================================
// Sets the ball at the staring position on top of the ship, awaiting input to launch
//=============================================================================
void Fallback::restartBall()
{
	ballResetting = true;
	recentBallPositions.clear();
}

void Fallback::launchBall()
{
	ball.launch();
	ballResetting = false;
	audio->playCue(BOUNCE_SHIP);
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Fallback::ai()
{}

//=============================================================================
// Handle collisions
//=============================================================================
void Fallback::collisions()
{
	if (isGameOver()) return;

	VECTOR2 collisionVector, collisionPosition;

	if (!isPaused) {

		// collision ball with block
		bool hitThisFrame = false;
		for (int i = 0; i < blocks.size(); i++) {
			// must use .at() to properly access the actual block object
			// .at() returns a "reference".. hence a pointer is needed to capture it properly
			Block* const block = &blocks.at(i);
			
			// can't collide with animating Blocks
			if (block->getIsAnimating()) {
				continue; // skip
			}

			if (ball.collidesWith(blocks.at(i), collisionVector)) {
				int direction = 0; // used to determine the direction of impact
				hitThisFrame = true;
				
				ball.bounce(collisionVector, block->getSpriteData(), direction);
				
				if (hitThisFrame) {
					break; // exit loop since we already hit a block
				}

			} // end collision if
		} // end blocks loop

		// see if we got'em all
		checkGameOver();
	}

}

//=============================================================================
// Remove block that was hit by the ball
//=============================================================================
void Fallback::removeBlock(int index)
{
	audio->playCue(DESTROY_BLOCK);

	// explode
	const VECTOR2 pos = {
		blocks.at(index).getCenterX(),
		blocks.at(index).getCenterY()
	};

	// send parts flying
	explosionManager.spawnExplosion(this, &iconTexture, pos);

	// destroying blocks increases the ball speed
	ball.bumpSpeedUp();

	// no power up entity in play and ship has no powerup
	if (fallingPowerUpPtr == NULL && !hasPowerUp) {
		spawnPowerUp(pos);
	}


	blocks.erase(blocks.begin() + index);

}

void Fallback::checkGameOver()
{
	return;
}

//=============================================================================
// Render game items
//=============================================================================
void Fallback::render()
{
	try {
		graphics->spriteBegin();

		// screen/game state
		switch (currentScreen) {
			case TITLE:
				renderTitleScreen();
				break;
			case GAME:
				renderGameScreen();
				break;
			case EDITOR:
				backgroundImage.draw();
				renderRacers();
				editor->draw();
				break;
		}

		graphics->spriteEnd();
	}
	catch (...) {
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::render"));
	}

}

void Fallback::renderTitleScreen()
{
	backgroundImage.draw();
	newGameButton.draw();

	titleImage.draw();
	console.renderLog();
}

/// <summary>
/// Loops thru and renders any background vfx
/// </summary>
void Fallback::renderRacers()
{
	int i = 0;
	for (auto& racer : racers) {
		if (i % 2 == 0) {
			racer.draw(graphicsNS::ALPHA75);
		} else {
			racer.draw();
		}
		++i;
	}
}

/// <summary>
/// Preps the move to the gameplay screen
/// </summary>
void Fallback::setGameScreen()
{
	// shift to next sprite frame for the game bg
	backgroundImage.setX(-static_cast<int>(GAME_WIDTH));
	backgroundImage.setColorFilter(graphicsNS::WHITE);
	currentScreen = GAME;
}

void Fallback::setEditorScreen()
{
	backgroundImage.setX(-static_cast<int>(GAME_WIDTH));
	currentScreen = EDITOR;
}

/// <summary>
/// Preps to move to the Title Screen
/// </summary>
void Fallback::setTitleScreen()
{
	currentScreen = TITLE;
	titleLoading = true;
	titleLoadingTimer = 0;

	// set bg 
	backgroundImage.setX(0);
	backgroundImage.setColorFilter(graphicsNS::ALPHA25);
	StrongAnimationPtr bgAnim = std::make_shared<FadeTo>(&backgroundImage, 2.0f, 1.0f);
	m_AnimationManager.attachProcess(bgAnim);

	titleImage.setColorFilter(graphicsNS::ALPHA25);
	StrongAnimationPtr titleAnim = std::make_shared<FadeTo>(&titleImage, 0.75, 1.0f);
	m_AnimationManager.attachProcess(titleAnim);


}

void Fallback::launchEditor()
{
	if (!floorTexture.getTexture()) {
		initFloor();
	}

	// share our stuff
	if (editor->initialized == false) {
		if (editor->initialize(this, &buttonTexture, &floorTexture, &console)) {
			// TODO handle error
		}
	}

	if (editor->initialized) {
		// let's go!
		editor->start();
		setEditorScreen();
	}

}

void Fallback::renderGameScreen()
{
	backgroundImage.draw();

	// draw paths
	for (int i = 0; i < blocks.size(); i++) {
		blocks.at(i).draw();
	}

	if (gameOver) {
		// show message
		gameOverImage.draw();
	} else {
		// only draw these when playing
		player.draw();
	}

	// always draw the following

	// particles
	//explosionManager.draw();

	// UI
	//renderUI();
	console.renderLog();
}

void Fallback::renderUI()
{
	RECT scoreRect;
	scoreRect.left = 654;	// upper left X
	scoreRect.top = 10;		// upper left Y
	scoreRect.right = GAME_WIDTH - 8; // lower right X
	scoreRect.bottom = scoreRect.top + 48;	// lower right Y

	// score shadow
	dxScoreFont.setFontColor(graphicsNS::GRAY & graphicsNS::ALPHA50);
	dxScoreFont.print(std::to_string(score), scoreRect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	// score main font, adjust rect position up and left
	scoreRect.left = 649;	// upper left X
	scoreRect.top = 7;		// upper left Y
	scoreRect.right = GAME_WIDTH - 11;
	scoreRect.bottom = scoreRect.top + 48;
	dxScoreFont.setFontColor(graphicsNS::WHITE);
	dxScoreFont.print(std::to_string(score), scoreRect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	// ball count number
	scoreRect.left = GAME_WIDTH - 24;	// upper left X
	scoreRect.top += 51;				// upper left Y
	scoreRect.right = GAME_WIDTH;		// lower right X
	scoreRect.bottom = scoreRect.top + 32;	// lower right Y
	dxBallCount.setFontColor(getBallCountColor());
	dxBallCount.print(std::to_string(ballCount), scoreRect, DT_LEFT | DT_SINGLELINE);

	// ball count icon and x
	ballCountIcon.draw(ballCountIcon.getColorFilter(), true);
	ballCountXImage.draw();

	// power ups
	if (hasPowerUp) {
		uiCurrentPowerUpDiamond.draw(currentPowerUpColor, true);
		uiCurrentPowerUpIcon.draw();
	}
}

COLOR_ARGB Fallback::getBallCountColor()
{
	switch (ballCount) {
		case 1:
			return graphicsNS::FB_HARD;
			break;
		case 2:
			return graphicsNS::FB_STRONG;
			break;
		case 3:
			return graphicsNS::FB_INVINCIBLE;
			break;
	}

	return graphicsNS::WHITE;
}

//=============================================================================
// ESC key quits the game
//=============================================================================
void Fallback::CheckForExit() {
	// ESC key always quits
	if (input->wasKeyPressed(ESC_KEY)) {
		switch (currentScreen) {
			case TITLE:
				PostQuitMessage(0); // quits app
				break;
			case GAME:
				exitGame();
				break;
			case EDITOR:
				exitEditor();
				break;
		}
	}
}

void Fallback::exitEditor()
{
	// clean up
	console.setLogText("");
	//loadLevelFiles();
	setTitleScreen();
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Fallback::releaseAll()
{
	backgroundTexture.onLostDevice();
	titleTexture.onLostDevice();
	iconTexture.onLostDevice();
	shipTexture.onLostDevice();
	floorTexture.onLostDevice();
	buttonTexture.onLostDevice();
	gameOverTexture.onLostDevice();
	detailsTexture.onLostDevice();

	dxScoreFont.onLostDevice();
	dxBallCount.onLostDevice();
	console.onLostDevice();

	Game::releaseAll();
	return;
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void Fallback::resetAll()
{
	backgroundTexture.onResetDevice();
	titleTexture.onResetDevice();
	iconTexture.onResetDevice();
	shipTexture.onResetDevice();
	detailsTexture.onResetDevice();
	floorTexture.onResetDevice();
	buttonTexture.onResetDevice();
	gameOverTexture.onResetDevice();

	dxScoreFont.onResetDevice();
	dxBallCount.onResetDevice();
	console.onResetDevice();

	Game::resetAll();
	return;
}
