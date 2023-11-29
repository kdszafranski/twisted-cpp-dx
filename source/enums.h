#ifndef ENUMS_H
#define ENUMS_H

// weapon types
enum WEAPON { TORPEDO, SHIP, PLANET, BALL };
// block types
enum BLOCK { WEAK, STRONG, HARD, METAL, INVINCIBLE, NONE };
// screen states
enum SCREEN { TITLE, GAME, EDITOR, CREDITS };

// player movement direction
enum ePlayerMoveDirection { NOT_MOVING, UP, RIGHT, DOWN, LEFT };
// increment moves clockwise

//#ifndef OP
//#define OP
//ePlayerMoveDirection& operator+=(ePlayerMoveDirection& orig, int) {
//	orig = static_cast<ePlayerMoveDirection>(orig + 1);
//	if (orig >= LEFT) {
//		orig = UP;
//		return orig;
//	} else {
//		return orig;
//	}
//}
//#endif // !OP


//// decrement moves counter-clockwise
//ePlayerMoveDirection& operator-=(ePlayerMoveDirection& orig, int) {
//	orig = static_cast<ePlayerMoveDirection>(orig - 1);
//	if (orig == NOT_MOVING) {
//		orig = LEFT;
//		return orig;
//	} else {
//		return orig;
//	}
//}

// Powerups
// Slow - slows ball x
// WRAP - allows ship to wrap around to the other side
// Fast - speeds up ship x
// Grow - lengthens ship along x-axis x
// Penalties
// Zoom - speeds up ball x
// Tiny - shrinks ship x
// Mystery - picks one of the others randomly x
enum POWERUP { SLOW, WARP, FAST, GROW, ZOOM, TINY, MYSTERY };


#endif