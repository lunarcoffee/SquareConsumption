#include <stdio.h>
#include <nds.h>
#include "drawing.h"
#include "main.h"
#include "square.h"

#define C_WALL ARGB16(1, 20, 4, 4)
#define C_PLAYER_SQ ARGB16(1, 0, 31, 0)
#define C_ENEMY_SQ ARGB16(1, 27, 27, 4)
#define C_BLACK ARGB16(1, 0, 0, 0)

#define MAP_WIDTH 14    // Width of the collision/enemy map.
#define MAP_HEIGHT 10   // Height of the map.

#define TW 16   // Width of each map tile.
#define SW 14   // Width of other squares.

static void drawBox(void);
static void drawPlayerSquare(void);
static void drawNPC(void);
static s32 min(s32 first, s32 second);
static s32 max(s32 first, s32 second);
static s32 toPx(s32 mapCoord);
static void sleepMs(s32 ms);
static bool gameWon(void);
static void advanceTurn(bool exploded);

// Map of enemies and objects that the player square can collide in.
static int map[MAP_HEIGHT][MAP_WIDTH];

static int sx;  // The player square's x index on the map.
static int sy;  // The y index.

static int moves;   // Number of valid moves made since the start of the game.
static int cd;      // The explosion attack cooldown.

static bool game_won = false;

// Essentially starts the game.
void initSquare(void) {
    resetGame();
    drawBox();
    drawPlayerSquare();
    drawNPC();
}

void moveSquare(s32 xs, s32 ys) {
    s32 newX = sx + xs;
    s32 newY = sy + ys;

    if (newX < 0 || newX >= MAP_WIDTH || newY < 0 || newY >= MAP_HEIGHT || map[newY][newX] > 0)
        return;

    fillSquare(toPx(sx), toPx(sy), SW, C_BLACK);
    sx = newX;
    sy = newY;
    fillSquare(toPx(sx), toPx(sy), SW, C_PLAYER_SQ);

    advanceTurn(false);
}

void attackSquare(void) {
    for (s32 x = max(sx - 1, 0); x <= min(sx + 1, MAP_WIDTH - 1); x++) {
        for (s32 y = max(sy - 1, 0); y <= min(sy + 1, MAP_HEIGHT - 1); y++) {
            if (map[y][x] == 2) {
                fillSquare(toPx(x), toPx(y), SW, C_BLACK);

                map[y][x] = 0;
                advanceTurn(false);

                if (gameWon()) {
                    iprintf("\x1b[11;0HYou won the game!\n");
                    game_won = true;
                }
                return;
            }
        }
    }
}

void explodeSquare(void) {
    // Animation frame colors.
    static u16 colors[] = {
        ARGB16(1, 31, 0, 0),
        ARGB16(1, 30, 4, 4),
        ARGB16(1, 28, 6, 6),
        ARGB16(1, 26, 8, 8),
        ARGB16(1, 25, 12, 12),
    };

    if (cd > 0)
        return;

    // Set everything in a 2 tile radius to emptiness.
    for (s32 x = max(sx - 2, 0); x <= min(sx + 2, MAP_WIDTH - 1); x++)
        for (s32 y = max(sy - 2, 0); y <= min(sy + 2, MAP_HEIGHT - 1); y++)
            map[y][x] = 0;

    // Creating an explosion is always a valid move.
    advanceTurn(true);

    // Draw explosion animation and redraw containing box (the animation may have destroyed a part) and square.
    for (s32 i = 0; i < sizeof(colors) / sizeof(u16); i++) {
        fillSquare(toPx(sx - 2), toPx(sy - 2), 5 * TW, colors[i]);
        sleepMs(80);
    }
    fillSquare(toPx(sx - 2), toPx(sy - 2), 5 * TW, C_BLACK);
    drawBox();
    drawPlayerSquare();

    if (gameWon() && !game_won) {
        iprintf("\x1b[11;0HYou won the game!");
        game_won = true;
    }
}

// Resets the game to the starting configuration.
void resetGame(void) {
    static int tempMap[MAP_HEIGHT][MAP_WIDTH] = {
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0},
        {0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0}
    };

    sx = 0;
    sy = 0;
    moves = 0;
    cd = 0;
    game_won = false;

    for (s32 x = 0; x < MAP_WIDTH; x++)
        for (s32 y = 0; y < MAP_HEIGHT; y++)
            map[y][x] = tempMap[y][x];

    fillSquare(0, 0, SCREEN_WIDTH, C_BLACK);
    drawBox();
    drawPlayerSquare();
    drawNPC();

    iprintf("\x1b[0;0H");
    iprintf("Moves/CD: 0/0                   \n");
    for (s32 i = 0; i < SCREEN_WIDTH / 8; i++)
        iprintf("-");

    iprintf("\nWelcome! Controls:\n\n");
    iprintf("A: consume an adjacent square.\n");
    iprintf("B: explode! (has cooldown)\n");
    iprintf("X: exit the game.\n");
    iprintf("Y: reset the game.\n\n                 ");
}

// Draws the containing box.
static void drawBox(void) {
    lineRect(TW / 2, TW / 2, SCREEN_WIDTH - TW - 1, SCREEN_HEIGHT - TW - 1, C_WALL);
}

static void drawPlayerSquare(void) {
    fillSquare((sx + 1) * TW, (sy + 1) * TW, SW, C_PLAYER_SQ);
}

// Draws objects that can be collided with and enemies.
static void drawNPC(void) {
    for (s32 x = 0; x < MAP_WIDTH; x++)
        for (s32 y = 0; y < MAP_HEIGHT; y++)
            if (map[y][x] == 1)
                lineSquare(toPx(x), toPx(y), SW, C_WALL);
            else if (map[y][x] == 2)
                fillSquare(toPx(x), toPx(y), SW, C_ENEMY_SQ);
}

static s32 min(s32 first, s32 second) {
    return first < second ? first : second;
}

static s32 max(s32 first, s32 second) {
    return first > second ? first : second;
}

// Converts a map array index to an on-screen pixel index.
static s32 toPx(s32 mapCoord) {
    return (mapCoord + 1) * TW;
}

static void sleepMs(s32 s) {
    float64 limit = s / 16.666666;
    for (s32 i = 0; i < limit; i++)
        swiWaitForVBlank();
}

static bool gameWon(void) {
    for (s32 x = 0; x < MAP_WIDTH; x++)
        for (s32 y = 0; y < MAP_HEIGHT; y++)
            if (map[y][x] == 2)
                return false;
    return true;
}

static void advanceTurn(bool exploded) {
    moves++;

    // Decrement or set the explosion attack cooldown.
    if (cd > 0)
        cd--;
    if (exploded)
        cd = 10;

    iprintf("\x1b[0;0H");
    iprintf("Moves/CD: %d/%d ", moves, cd);
    iprintf("\x1b[11;0H");
}
