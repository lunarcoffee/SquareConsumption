#ifndef SQUARE_CONSUMPTION_DRAWING_H
#define SQUARE_CONSUMPTION_DRAWING_H

#include <nds.h>

void drawHLine(s32 x, s32 y, s32 length, u16 color);
void drawVLine(s32 x, s32 y, s32 length, u16 color);

void lineRect(s32 x, s32 y, s32 width, s32 height, u16 color);
void lineSquare(s32 x, s32 y, s32 side, u16 color);

void fillSquare(s32 x, s32 y, s32 side, u16 color);

#endif
