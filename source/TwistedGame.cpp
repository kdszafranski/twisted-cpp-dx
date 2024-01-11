// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provded by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

using namespace std;
#include "TwistedGame.h"
#include <time.h>
#include "level.h"
#include <fstream>
#include <iostream>
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
TwistedGame::TwistedGame()
{
	fallingPowerUpPtr = NULL;
	hasPowerUp = false;
	bIsMoving = false;
	bIsOnPath = false;
	powerUpTimer = 0;
	titleLoadingTimer = 0;
	titleLoading = false;
	currentPowerUp = FAST; // not actually applied, null would be better
	animId = 0;
	score = 0;
	isPaused = false;

}

//=============================================================================
// Destructor
//=============================================================================
TwistedGame::~TwistedGame()
{
	releaseAll();           // call onLostDevice() for every graphics item

	// remove all running animations
	turnables.clear();
	console.resetLog();

	m_AnimationManager.abortAllProcesses(true);

	SAFE_DELETE(fallingPowerUpPtr);
}


//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void TwistedGame::initialize(HWND hwnd)
{
	Game::initialize(hwnd); // throws GameError

	initBackgrounds();
	initButtons();

	// Init DirectX font 
	if (dxScoreFont.initialize(graphics, 62, true, false, "Agdasima") == false)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing score font"));

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
void TwistedGame::startNewGame()
{
	// set proper bg screen state
	setGameScreen();

	// load game sprites
	initSprites();

	// stop any other animations
	m_AnimationManager.clearAllProcesses();
	explosionManager.clearAllParticles();
	console.resetLog();

	// reset game variables
	resetGame();

	maze.Generate();

	//loadRandomLevel();

	// play!
	//restartBall();
}

/// <summary>
/// Resets score, board, and general game state
/// </summary>
void TwistedGame::resetGame()
{
	hasPowerUp = false;
	gameOver = false;
	isPaused = false;
	bIsOnPath = true;
	ballCount = MAX_BALLS;
	powerUpTimer = 0;
	score = 0;
	currentLevel = 0; // points into levels vector, 0 is the first level
	console.resetLog();
}

void TwistedGame::exitGame()
{
	//console.log("");
	isPaused = false;
	ballCount = 0;

	// remove animations
	explosionManager.clearAllParticles();
	m_AnimationManager.clearAllProcesses();

	// clean up game
	blocks.clear();
	turnables.clear();

	SAFE_DELETE(fallingPowerUpPtr);

	// go to main menu
	setTitleScreen();
}

//=============================================================================
// Initializes all the game sprites from textures
//=============================================================================
void TwistedGame::initSprites() {
	// misc graphics
	initMessageSprites();
	// create our game object and graphics
	initPlayerArrow();
	// set up the blocks
	initFloor();
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
void TwistedGame::initBackgrounds()
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

void TwistedGame::initButtons()
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

void TwistedGame::initMessageSprites()
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
void TwistedGame::initPlayerArrow()
{
	if (!playerTexture.initialize(graphics, PLAYER_PATH)) {
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing player texture"));
	}
	if (!player.initialize(this, playerNS::WIDTH, playerNS::HEIGHT, playerNS::TEXTURE_COLS, &playerTexture))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing player entity"));

	//ship.setFrames(playerNS::SHIP_START_FRAME, playerNS::SHIP_END_FRAME);
	player.Reset();
	player.setCurrentFrame(0);

	// start center
	player.setX(GAME_WIDTH / 2 - playerNS::WIDTH / 2);
	player.setY(GAME_HEIGHT / 2 - playerNS::HEIGHT / 2);
	player.setVelocity(VECTOR2(0, 0)); // start standing still

}


void TwistedGame::initUI()
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

	// logo
	if (!logoTexture.initialize(graphics, LOGO_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing logo texture"));
	}
	if (!logoImage.initialize(this->graphics, 246, 188, 1, &logoTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Logo image"));
	}
	logoImage.setPosition(GAME_WIDTH - logoImage.getWidth() - 8, GAME_HEIGHT - logoImage.getHeight() - 8);

}

//=============================================================================
// Initialize block texture/images
//=============================================================================
void TwistedGame::initFloor()
{
	// load our texture, reuse it for all Floor Entities
	if (!floorTexture.initialize(graphics, TILE_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Floor texture"));
	}
	// load our texture, reuse it for all Floor Entities
	if (!turnableTexture.initialize(graphics, TURNABLE_BASE_PATH))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Turnable texture"));
	}
}

void TwistedGame::loadLevelFiles() {
	levels.clear();
	//loadLevelFromFile(0); // Editor default
	//loadLevelFromFile(1);
	//loadLevelFromFile(2);
	//loadLevelFromFile(3);
}

void TwistedGame::startNextLevel()
{
	// nothing to see here
}

/// <summary>
/// Creates the blocks in the vector from the list given level data which was previously loaded
/// </summary>
/// <param name="levelNumber">number that matches level number</param>
void TwistedGame::loadLevel(int levelNumber)
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
void TwistedGame::loadRandomLevel()
{
	const int START_X = GAME_WIDTH / 2 - blockNS::WIDTH / 2;
	const int START_Y = GAME_HEIGHT / 2 - blockNS::HEIGHT / 2;
	Vec2Int currentPosition = { START_X, START_Y };

	// clear vector
	blocks.clear();
	turnables.clear();

	currentPosition = makeStraightaway(5, UP, currentPosition.x, currentPosition.y);


	srand((unsigned)time(0));
	ePlayerMoveDirection direction = UP;
	ePlayerMoveDirection lastDirection = direction;
	int const distance = 0;
	
	currentPosition = makeTurnable(lastDirection, currentPosition.x, currentPosition.y);

	currentPosition = makeStraightaway(5, UP, currentPosition.x, currentPosition.y);
	currentPosition = makeStraightaway(5, UP, currentPosition.x, currentPosition.y);
	currentPosition = makeTurnable(lastDirection, currentPosition.x, currentPosition.y);
	currentPosition = makeStraightaway(5, UP, currentPosition.x, currentPosition.y);


	/*
	for (int i = 0; i < 20; i++) {
		direction = static_cast<ePlayerMoveDirection>( rand() % 4 + 1 );	
		distance = 3; // rand() % 4 + 2;
		console.log("Running at " + std::to_string(i));
		if (abs(direction - lastDirection) == 2) {
			// move around clockwise
			direction += 1;
		}
		// make sections
		switch (direction) {
			case UP:
				currentPosition = makeStraightaway(distance, UP, currentPosition.x, currentPosition.y);
				break;
			case DOWN:
				currentPosition = makeStraightaway(distance, DOWN, currentPosition.x, currentPosition.y);
				break;
			case RIGHT:
				currentPosition = makeStraightaway(distance, RIGHT, currentPosition.x, currentPosition.y);
				break;
			case LEFT:
				currentPosition = makeStraightaway(distance, LEFT, currentPosition.x, currentPosition.y);
				break;
			default:
				distance += 1;
		}

		lastDirection = direction;

		// if we are crossing, we'll do a bunch of checking for no reason
		if (!isValidLocation(currentPosition.x, currentPosition.y)) {
			console.log("ending early");
			return;
		}
	}
	*/

}

//=============================================================================
// Loads a level from disk
//=============================================================================
bool TwistedGame::loadLevelFromFile(int n)
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
void TwistedGame::update(float frameTime)
{
	// handle inputs on Title Screen only
	if (currentScreen == TITLE) {
		// process animations
		m_AnimationManager.updateProcesses(frameTime);
		updateTitleScreen(frameTime);
	}

	// handle Game updates and inputs
	if (currentScreen == GAME) {
		checkPauseInput();
		if (!gameOver) {
			checkCheatInput();
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

	// check if we want to exit
	checkForExit();
}

/// <summary>
/// Updates elements for the title screen only, including a timer for some elements
/// </summary>
/// <param name="frameTime">current frame time</param>
void TwistedGame::updateTitleScreen(float frameTime)
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

void TwistedGame::updateGameScreen(float frameTime) {

	player.update(frameTime);

	// update all other sprites to move opposite player move direction
	//

	// bg art
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

	// all floor tiles
	updateFloorTiles(frameTime, player.moveDirection);
	updateTurnables(frameTime, player.moveDirection);

	if (input->wasKeyPressed(ROTATE_KEY)) {
		console.log("Rotate Turnables");
		for (std::vector<Turnable>::iterator it = turnables.begin(); it != turnables.end(); ++it) {
			it->rotate();
		}
	}

}

void TwistedGame::updateFloorTiles(float frameTime, ePlayerMoveDirection pDir)
{
	for (int i = 0; i < blocks.size(); i++) {
		switch (pDir) {
			case UP:
				blocks.at(i).setY(blocks.at(i).getY() + 1);
				break;
			case RIGHT:
				blocks.at(i).setX(blocks.at(i).getX() - 1);
				break;
			case DOWN:
				blocks.at(i).setY(blocks.at(i).getY() - 1);
				break;
			case LEFT:
				blocks.at(i).setX(blocks.at(i).getX() + 1);
				break;
		}
	}
}

void TwistedGame::updateTurnables(float frameTime, ePlayerMoveDirection pDir)
{
	for (std::vector<Turnable>::iterator it = turnables.begin(); it != turnables.end(); ++it) {
		
		it->update(frameTime); // resets rotated box status

		switch (pDir) {
			case UP:
				it->setY(it->getY() + 1);
				break;
			case RIGHT:
				it->setX(it->getX() - 1);
				break;
			case DOWN:
				it->setY(it->getY() - 1);
				break;
			case LEFT:
				it->setX(it->getX() + 1);
				break;
		}
	}
}

void TwistedGame::updateGameOverScreen(float frameTime)
{
}

/// <summary>
/// Updates particles and effects
/// </summary>
/// <param name="frameTime">current frame time</param>
void TwistedGame::updateEffects(float frameTime)
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

#pragma region PowerUps

// Creates the Entity on the screen
void TwistedGame::spawnPowerUp(VECTOR2 position)
{
	// spawn powerup
	if (fallingPowerUpPtr == NULL) {
		const int n = rand() % 7;
		fallingPowerUpPtr = new PowerUp(static_cast<POWERUP>(n), position);
		fallingPowerUpPtr->initialize(this, 32, 32, 8, &powerUpTexture);
	}
}

// applies the collided Power up
void TwistedGame::applyPowerUp()
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
			player.applyPowerUp(currentPowerUp);
			break;
		case SLOW: // slow 
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

void TwistedGame::removePowerUp()
{
	if (hasPowerUp) {
		switch (currentPowerUp) {
			case ZOOM: // ball speed
			case SLOW: // ball speed
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

void TwistedGame::checkPauseInput()
{
	if (currentScreen == GAME) {
		// SPACE pauses
		if (input->wasKeyPressed(SPACE_KEY)) {
			isPaused = !isPaused;
			console.AddLine( isPaused? "Paused" : "Unpaused");
		}
	}
}


void TwistedGame::checkCheatInput()
{
	if (currentScreen == GAME) {
		// next level
		if (input->wasKeyPressed(PLAYER_DOWN_KEY)) {
			startNextLevel();
		}

		if (input->wasKeyPressed(ENTER_KEY)) {
			startNewGame();
		}
	}
}

bool TwistedGame::isGameOver()
{
	// we're good until we fall off
	if (bIsOnPath) {		
		return false;
	}

	return true;
}

void TwistedGame::shakeScreen()
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
void TwistedGame::handleGameOver()
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
// Artificial Intelligence
//=============================================================================
void TwistedGame::ai()
{}

//=============================================================================
// Handle collisions
//=============================================================================
void TwistedGame::collisions()
{
	if (isGameOver()) return; // no need to do this if we're done

	VECTOR2 collisionVector, collisionPosition;

	if (!isPaused) {
		for (int i = 0; i < blocks.size(); i++) {
			// .at() returns a "reference".. hence a pointer is needed to capture it properly
			Block* const block = &blocks.at(i);
			
			// can't collide with animating Blocks
			if (block->getIsAnimating()) {
				continue; // skip
			}

			if (player.collidesWith(blocks.at(i), collisionVector)) {	
				bIsOnPath = true;
				return; // we just need to be on a block
			}

		} // end blocks loop

		bIsOnPath = false;
		
		// only check these if not on floor tile
		for (int i = 0; i < turnables.size(); i++) {

			if (player.collidesWith(turnables.at(i), collisionVector)) {
				bIsOnPath = true;
				console.log("on Turnable");
				return;
			}
			
		}
		
		bIsOnPath = false;
		gameOver = true;

	}

}

//=============================================================================
// Remove block that was hit by the ball
//=============================================================================
void TwistedGame::removeBlock(int index)
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
	//ball.bumpSpeedUp();

	// no power up entity in play and ship has no powerup
	if (fallingPowerUpPtr == NULL && !hasPowerUp) {
		spawnPowerUp(pos);
	}


	blocks.erase(blocks.begin() + index);

}

void TwistedGame::checkGameOver()
{
	gameOver = false;
	return;
}

//=============================================================================
// Render game items
//=============================================================================
void TwistedGame::render()
{
	try {
		graphics->spriteBegin();

		// screen/game state
		switch (currentScreen) {
			case TITLE:
				renderTitleScreen();
				break;
			case GAME:
				renderMaze();
				//renderGameScreen();
				break;
		}

		graphics->spriteEnd();
	}
	catch (...) {
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::render"));
	}

}

void TwistedGame::renderTitleScreen()
{
	backgroundImage.draw();
	newGameButton.draw();

	titleImage.draw();
	console.renderLog();
}

void TwistedGame::renderMaze()
{
	int spacing = 10;
	int tileSize = 20;

	int x = spacing;
	int y = spacing;
	for (int i = 0; i < maze.height; i++) {
		// cols		
		for (int j = 0; j < maze.width; j++) {
			
			Block newBlock;

			if (!newBlock.initialize(this, tileSize, tileSize, 1, &floorTexture))
			{
				throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing block entity"));
			}

			newBlock.setPosition(x, y);
			newBlock.setVelocity(VECTOR2(0, 0));
			
			if (maze.cells.at(i).at(j).bVisited) {
				newBlock.setColorFilter(graphicsNS::BLUE);
			}

			newBlock.draw();

			x += tileSize + spacing;
		}
		x = spacing;
		y += tileSize + spacing;
	}
	console.renderLog();
}


/// <summary>
/// Preps the move to the gameplay screen
/// </summary>
void TwistedGame::setGameScreen()
{
	// shift to next sprite frame for the game bg
	backgroundImage.setX(-static_cast<int>(GAME_WIDTH));
	backgroundImage.setColorFilter(graphicsNS::WHITE);
	currentScreen = GAME;
}

/// <summary>
/// Preps to move to the Title Screen
/// </summary>
void TwistedGame::setTitleScreen()
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


void TwistedGame::renderGameScreen()
{
	backgroundImage.draw();

	// draw paths
	//for (int i = 0; i < blocks.size(); i++) {
	//	blocks.at(i).draw();
	//}

	//for (int i = 0; i < turnables.size(); i++) {
	//	turnables.at(i).draw();
	//}

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
	renderUI();
	console.renderLog();
}

void TwistedGame::renderUI()
{
	logoImage.draw();
	//RECT scoreRect;
	//scoreRect.left = 654;	// upper left X
	//scoreRect.top = 10;		// upper left Y
	//scoreRect.right = GAME_WIDTH - 8; // lower right X
	//scoreRect.bottom = scoreRect.top + 48;	// lower right Y

	//// score shadow
	//dxScoreFont.setFontColor(graphicsNS::GRAY & graphicsNS::ALPHA50);
	//dxScoreFont.print(std::to_string(score), scoreRect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	//// score main font, adjust rect position up and left
	//scoreRect.left = 649;	// upper left X
	//scoreRect.top = 7;		// upper left Y
	//scoreRect.right = GAME_WIDTH - 11;
	//scoreRect.bottom = scoreRect.top + 48;
	//dxScoreFont.setFontColor(graphicsNS::WHITE);
	//dxScoreFont.print(std::to_string(score), scoreRect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	//// ball count number
	//scoreRect.left = GAME_WIDTH - 24;	// upper left X
	//scoreRect.top += 51;				// upper left Y
	//scoreRect.right = GAME_WIDTH;		// lower right X
	//scoreRect.bottom = scoreRect.top + 32;	// lower right Y
	//dxBallCount.setFontColor(getBallCountColor());
	//dxBallCount.print(std::to_string(ballCount), scoreRect, DT_LEFT | DT_SINGLELINE);

	//// ball count icon and x
	//ballCountIcon.draw(ballCountIcon.getColorFilter(), true);
	//ballCountXImage.draw();

	//// power ups
	//if (hasPowerUp) {
	//	uiCurrentPowerUpDiamond.draw(currentPowerUpColor, true);
	//	uiCurrentPowerUpIcon.draw();
	//}
}


/// <summary>
/// Checks a given x, y location for an existing floor tile
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns>true if no tile is found, false if a tile is found</returns>
bool TwistedGame::isValidLocation(int x, int y)
{
	// go thru all blocks and check for this x, y position
	for (int j = 0; j < blocks.size(); j++) {
		Block thisBlock = blocks.at(j);

		if (thisBlock.getX() == x && thisBlock.getY() == y) {
			return false;
		}
	}

	// @TODO add turnables

	return true;
}

Vec2Int TwistedGame::makeStraightaway(int distance, ePlayerMoveDirection direction, int startX, int startY)
{
	int x = startX;
	int y = startY;

	for (int i = 0; i < distance; i++) {
		// check if we can add this block
		if (!isValidLocation(x, y)) {
			return { x, y };
		}

		Block newBlock;

		if (!newBlock.initialize(this, blockNS::WIDTH, blockNS::HEIGHT, blockNS::TEXTURE_COLS, &floorTexture))
		{
			throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing block entity"));
		}

		newBlock.setPosition(x, y);
		newBlock.setVelocity(VECTOR2(0, 0));

		if (i == 0) {
			// first
			newBlock.setColorFilter(graphicsNS::BLUE);
		}
		if (i == distance - 1) {
			// last
			newBlock.setColorFilter(graphicsNS::PURPLE);
		}

		// add to vector
		blocks.push_back(newBlock);

		// set next position
		switch (direction) {
			case UP:
				y -= blockNS::HEIGHT;
				break;
			case DOWN:
				y += blockNS::HEIGHT;
				break;
			case RIGHT:
				x += blockNS::WIDTH;
				break;
			case LEFT:
				x -= blockNS::WIDTH;
				break;
		}
	}

	return { x, y };
}

Vec2Int TwistedGame::makeTurnable(ePlayerMoveDirection lastDirection, int x, int y)
{
	Turnable turner;
	int nextX = x;
	int nextY = y;

	if (!turner.initialize(this, turnableNS::WIDTH, turnableNS::HEIGHT, turnableNS::TEXTURE_COLS, &turnableTexture))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing block entity"));
	}

	switch (lastDirection) {
		case UP:
			y -= blockNS::HEIGHT * 2; // x,y is the next block space, so move up again
			nextY -= turnableNS::HEIGHT;
		case DOWN:
			//turner.rotate();
			break;

	}

	turner.setPosition(x, y);
	turner.setVelocity(VECTOR2(0, 0));

	turnables.push_back(turner);

	return { nextX, nextY };


}

//=============================================================================
// ESC key quits the game
//=============================================================================
void TwistedGame::checkForExit() {
	// ESC key always quits
	if (input->wasKeyPressed(ESC_KEY)) {
		switch (currentScreen) {
			case TITLE:
				PostQuitMessage(0); // quits app
				break;
			case GAME:
				exitGame();
				// faster for testing
				//PostQuitMessage(0);
				break;
		}
	}
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void TwistedGame::releaseAll()
{
	backgroundTexture.onLostDevice();
	titleTexture.onLostDevice();
	iconTexture.onLostDevice();
	playerTexture.onLostDevice();
	floorTexture.onLostDevice();
	turnableTexture.onLostDevice();
	buttonTexture.onLostDevice();
	gameOverTexture.onLostDevice();
	detailsTexture.onLostDevice();
	logoTexture.onLostDevice();

	dxScoreFont.onLostDevice();
	console.onLostDevice();

	Game::releaseAll();
	return;
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void TwistedGame::resetAll()
{
	backgroundTexture.onResetDevice();
	titleTexture.onResetDevice();
	iconTexture.onResetDevice();
	playerTexture.onResetDevice();
	detailsTexture.onResetDevice();
	floorTexture.onResetDevice();
	turnableTexture.onResetDevice();
	buttonTexture.onResetDevice();
	gameOverTexture.onResetDevice();
	logoTexture.onResetDevice();

	dxScoreFont.onResetDevice();
	console.onResetDevice();

	Game::resetAll();
	return;
}
