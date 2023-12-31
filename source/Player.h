// Chapter 6 project by Kris Szafranski
// ====================================
// Builds on engine code provded by
// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly

#ifndef _SHIP_H                 // Prevent multiple definitions if this 
#define _SHIP_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
//#include "constants.h"

namespace playerNS
{
    const int WIDTH = 54;                   // image width
    const int HEIGHT = 32;                  // image height
    const int X = 0;   // location on screen
    const int Y = 0;
    const float SPEED = 310;                // 100 pixels per second
    const float MASS = 0;// 1.0e14f;              // mass
    const int   TEXTURE_COLS = 1;           // texture has 8 columns
    const int   SHIP_START_FRAME = 0;      // ship1 starts at frame 0
    const int   SHIP_END_FRAME = 0;        // ship1 animation frames 0,1,2,3
    const float SHIP_ANIMATION_DELAY = 0.2f;    // time between frames
}

// inherits from Entity class
class Player : public Entity
{
private:
    float currentSpeed;
    bool hasPowerUp;
    bool canWrapEdge;

public:

    ePlayerMoveDirection moveDirection;
    // constructor
    Player();

    // inherited member functions
    virtual void draw();
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);
    // reset new game state
    void Reset();
    void update(float frameTime);

    // power ups
    void applyPowerUp(POWERUP type) override;
    void removePowerUp();
    void removeWrapAround();
    void setHasPowerUp(bool v) { hasPowerUp = v; }
    bool getHasPowerUp() { return hasPowerUp; }
    bool canWrapAround() { return canWrapEdge; }
    void grow();
    void shrink();
    void resetSize();
    void resetSpeed();

    // call back interface
    void onAnimationSuccess() override;

};
#endif

