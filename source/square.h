#ifndef SQUARE_CONSUMPTION_SQUARE_H
#define SQUARE_CONSUMPTION_SQUARE_H

// Movement directions for [moveSquare].
#define MOVE_UP -1
#define MOVE_RIGHT 1
#define MOVE_DOWN 1
#define MOVE_LEFT -1
#define MOVE_NONE 0

void initSquare(void);
void moveSquare(s32 xs, s32 ys);
void attackSquare(void);
void explodeSquare(void);
void resetGame(void);

#endif
