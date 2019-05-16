#include <stdio.h>
#include <nds.h>
#include "drawing.h"
#include "square.h"

static void initVideo(void);

u16 *vram;

int main(void) {
    initVideo();
    consoleDemoInit();
    initSquare();

    while (true) {
        swiWaitForVBlank();
        scanKeys();
        u32 keys = keysDown();

        // Exit on SELECT button press.
        if (keys & KEY_X)
            break;

        if (keys & KEY_UP)
            moveSquare(MOVE_NONE, MOVE_UP);
        else if (keys & KEY_RIGHT)
            moveSquare(MOVE_RIGHT, MOVE_NONE);
        else if (keys & KEY_DOWN)
            moveSquare(MOVE_NONE, MOVE_DOWN);
        else if (keys & KEY_LEFT)
            moveSquare(MOVE_LEFT, MOVE_NONE);
        else if (keys & KEY_A)
            attackSquare();
        else if (keys & KEY_B)
            explodeSquare();
        else if (keys & KEY_Y)
            resetGame();
    }
}

static void initVideo(void) {
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    s32 bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    vram = bgGetGfxPtr(bg);
}
