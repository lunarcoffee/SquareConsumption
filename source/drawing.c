#include <nds.h>
#include "main.h"

void drawHLine(s32 x, s32 y, s32 length, u16 color) {
    s32 len = x + length;
    for (; x < len; x++)
        vram[SCREEN_WIDTH * y + x] = color;
}

void drawVLine(s32 x, s32 y, s32 length, u16 color) {
    s32 len = y + length;
    for (; y < len; y++)
        vram[SCREEN_WIDTH * y + x] = color;
}

void lineRect(s32 x, s32 y, s32 width, s32 height, u16 color) {
    drawHLine(x, y, width - 1, color);
    drawHLine(x, y + height - 1, width, color);
    drawVLine(x, y, height, color);
    drawVLine(x + width - 1, y, height, color);
}

void lineSquare(s32 x, s32 y, s32 side, u16 color) {
    lineRect(x, y, side, side, color);
}

void fillSquare(s32 x, s32 y, s32 side, u16 color) {
    s32 dx = x + side;
    s32 dy = y + side;
    s32 oy = y;

    for (; x < dx; x++) {
        for (; y < dy; y++)
            // Don't allow drawing outside the proper bounds.
            if (x > 0 && x < SCREEN_WIDTH)
                vram[SCREEN_WIDTH * y + x] = color;
        y = oy;
    }
}
