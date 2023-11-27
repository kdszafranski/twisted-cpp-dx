// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provded by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

#include "Player.h"

//=============================================================================
// default constructor
//=============================================================================
Player::Player() : Entity()
{
	currentSpeed = playerNS::SPEED;
	hasPowerUp = false;
	canWrapEdge = false;

	spriteData.width = playerNS::WIDTH;           // size of player
	spriteData.height = playerNS::HEIGHT;
	spriteData.x = playerNS::X;                   // location on screen
	spriteData.y = playerNS::Y;
	spriteData.rect.bottom = playerNS::HEIGHT;    // rectangle to select parts of an image
	spriteData.rect.right = playerNS::WIDTH;

	// set bounding box for BOX colliders
	collisionType = entityNS::BOX;
	edge.top = -playerNS::HEIGHT / 2;    // -16
	edge.right = playerNS::WIDTH / 2;    // 64
	edge.bottom = playerNS::HEIGHT / 2;  // 16
	edge.left = -playerNS::WIDTH / 2;    // -64

	velocity.x = 0;                             // velocity X
	velocity.y = 0;                             // velocity Y
	frameDelay = playerNS::SHIP_ANIMATION_DELAY;
	startFrame = playerNS::SHIP_START_FRAME;     // first frame of player animation
	endFrame = playerNS::SHIP_END_FRAME;     // last frame of player animation
	currentFrame = startFrame;
	radius = playerNS::WIDTH / 2.0;
	mass = playerNS::MASS;
}

//=============================================================================
// Initialize the Player.
// Post: returns true if successful, false if failed
//=============================================================================
bool Player::initialize(Game* gamePtr, int width, int height, int ncols,
	TextureManager* textureM)
{
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the ship
//=============================================================================
void Player::draw()
{
	if (hasPowerUp) {
		Image::draw(graphicsNS::FB_METAL);
	} else {
		Image::draw(); // white
	}
}

//=============================================================================
// apply the given power up
//=============================================================================
void Player::applyPowerUp(POWERUP type)
{
	// apply relevant power up
	if (type == FAST) {
		// bump our speed
		currentSpeed *= 1.5;
	}
	if (type == WARP) {
		currentSpeed += .35f;
		canWrapEdge = true;
	}

	// change our color, covers SLOW
	hasPowerUp = true;
}

void Player::removePowerUp()
{
	hasPowerUp = false;
}

void Player::removeWrapAround() {
	canWrapEdge = false;
	resetSpeed();
};


/// <summary>
/// Callback from animation process when animation is complete
/// </summary>
void Player::onAnimationSuccess()
{
	if (spriteData.xScale > 1) {
		// is now big
		grow();
	} else if (spriteData.xScale < 1) {
		// now small
		shrink();
	} else {
		// is equal
		resetSize();
	}

}

//=============================================================================
// adjust spriteData and collider bounds
//=============================================================================
void Player::grow()
{
	spriteData.width = 192;
	edge.right = spriteData.width / 2;    // 96
	edge.left = -spriteData.width / 2;    // -96
	hasPowerUp = true;
}

//=============================================================================
// Adjust spriteData and collider bounds
//=============================================================================
void Player::shrink()
{
	spriteData.width = 64;
	edge.right = spriteData.width / 2;    // 32
	edge.left = -spriteData.width / 2;    // -32
	hasPowerUp = true;
}

void Player::resetSize()
{
	spriteData.width = playerNS::WIDTH;
	edge.right = spriteData.width / 2;    // 64
	edge.left = -spriteData.width / 2;
	removePowerUp();
}

/// <summary>
/// Resets ship speed and turns power ups to false
/// </summary>
void Player::resetSpeed()
{
	currentSpeed = playerNS::SPEED;
	removePowerUp();
}


//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Player::update(float frameTime)
{
	// move right
	if (input->isKeyDown(SHIP_RIGHT_KEY)) {
		spriteData.x += frameTime * currentSpeed;
	}

	// move left
	if (input->isKeyDown(SHIP_LEFT_KEY)) {
		spriteData.x -= frameTime * currentSpeed;
	}

	if (canWrapEdge) {
		const float halfWidth = spriteData.width / 2;
		// right
		if (spriteData.x > GAME_WIDTH) {
			spriteData.x = 0;
		} else if (spriteData.x < 0) {
			// left
			spriteData.x = GAME_WIDTH;
		}
	} else {
		// keep it on the screen
		if (spriteData.x > GAME_WIDTH - spriteData.width) {
			spriteData.x = GAME_WIDTH - spriteData.width;    // position at right screen edge
		} else if (spriteData.x < 0) {
			spriteData.x = 0;                             // position at left screen edge
		}
	}

}
