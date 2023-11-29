#ifndef _TURNABLE_H                 // Prevent multiple definitions if this 
#define _TURNABLE_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "block.h"
#include "constants.h"

namespace turnableNS
{
    const int WIDTH = 64;                   // image width
    const int HEIGHT = 128;                  // image height
    const float SPEED = 0;                // Rotation speed?
    const int   TEXTURE_COLS = 1;           // texture has 2 columns
    const int   TURNABLE_START_FRAME = 0;      // ball starts at frame 0
    const int   TURNABLE_END_FRAME = 0;        // ball animation 
}

// inherits from Block class
// Image > Entity > Block > Turnable
class Turnable : public Block
{
private:
    bool isAnimating = false;
    float originalScale;
    float animScale;

public:
    // constructors
    Turnable();

    // inherited member functions
    virtual void draw();
    void rotate();
    virtual bool initialize(Game* gamePtr, int width, int height, int ncols,
        TextureManager* textureM);

};
#endif

