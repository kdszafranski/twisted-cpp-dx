// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provided by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

#include "block.h"

//=============================================================================
// default constructor
//=============================================================================
Block::Block() : Entity()
{
    spriteData.width = blockNS::WIDTH;           // size of Ball
    spriteData.height = blockNS::HEIGHT;
    spriteData.x = blockNS::X;                   // location on screen
    spriteData.y = blockNS::Y;

    // set bounding box for BOX collider
    collisionType = entityNS::BOX;      // override's Image default to CIRCLE
    edge.top = -blockNS::HEIGHT / 2;     // -16
    edge.right = blockNS::WIDTH / 2;     // 16
    edge.bottom = blockNS::HEIGHT / 2;   // 16
    edge.left = -blockNS::WIDTH / 2;     // -16

    velocity.x = 0;                             // velocity X
    velocity.y = 0;                             // velocity Y
    frameDelay = 1;

    startFrame = blockNS::BLOCK_START_FRAME;     // first frame of ship animation
    endFrame = blockNS::BLOCK_END_FRAME;     // last frame of ship animation
    currentFrame = startFrame;

    radius = blockNS::WIDTH / 2.0;
    mass = blockNS::MASS;
    collisionType = entityNS::BOX;

    isAnimating = false;
    originalScale = 1.0f;
    animScale = 1.0f;
}

//=============================================================================
// Initialize the Block.
// Post: returns true if successful, false if failed
//=============================================================================
bool Block::initialize(Game* gamePtr, int width, int height, int ncols,
    TextureManager* textureM)
{
    return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the block
//=============================================================================
void Block::draw()
{
    // tint this block with its color filter from the center
    Image::draw(0, true);
}

/// <summary>
/// Callback from Animation so we can unlock collisions
/// </summary>
void Block::onAnimationSuccess()
{
    isAnimating = false;
}

