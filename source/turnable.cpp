// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provided by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

#include "turnable.h"

//=============================================================================
// default constructor
//=============================================================================
Turnable::Turnable() : Block()
{
    spriteData.width = turnableNS::WIDTH;           // size of Ball
    spriteData.height = turnableNS::HEIGHT;
    spriteData.x = blockNS::X;                   // location on screen
    spriteData.y = blockNS::Y;

    // set bounding box for BOX collider
    collisionType = entityNS::ROTATED_BOX;      // override's Image default to CIRCLE
    rotatedBoxReady = false;
    edge.top = -turnableNS::HEIGHT;     // -128
    edge.right = turnableNS::WIDTH / 2;     // 32
    edge.bottom = turnableNS::HEIGHT;   // 16
    edge.left = -turnableNS::WIDTH / 2;     // -16

    velocity.x = 0;                             // velocity X
    velocity.y = 0;                             // velocity Y
    frameDelay = 1;

    startFrame = turnableNS::TURNABLE_START_FRAME;     // first frame of ship animation
    endFrame = turnableNS::TURNABLE_END_FRAME;     // last frame of ship animation
    currentFrame = startFrame;

    isAnimating = false;
    originalScale = 1.0f;
    animScale = 1.0f;
}

//=============================================================================
// Initialize the Turnable.
// Post: returns true if successful, false if failed
//=============================================================================
bool Turnable::initialize(Game* gamePtr, int width, int height, int ncols,
    TextureManager* textureM)
{
    return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the turnable
//=============================================================================
void Turnable::draw()
{
    // tint this turnable with its color filter from the center
    Image::draw(0, true);
}

void Turnable::rotate()
{
    spriteData.angle += 1.5707;
}
