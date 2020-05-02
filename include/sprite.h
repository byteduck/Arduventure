#ifndef SPRITE_H
#define SPRITE_H

#include <Arduino.h>
#include "Arduventure.h"
#include "color.h"

void drawSprite(const byte *sprite, byte spriteWidth, byte spriteHeight, const byte* palette, int x, int y);

//Draws pixel at [index] in displayBuffer if rightmost bit of [color] isn't set.
//Used in drawSprite.
inline void conditionalPixelDraw(int index, byte color) {
  if(!(color & 1)) displayBuffer[index] = color;
}

/*
* SPRITES
*/
const byte PLAYER[] = {
  0x0F, 0xF0,
  0xFF, 0xFF,
  0x0F, 0xF0,
  0xF0, 0x0F
};

const byte HEART[] = {
  0x10, 0x01,
  0x23, 0x32,
  0x12, 0x21,
  0x01, 0x10
};

const byte KEY[] = {
  0x00, 0x00, 0x04, 0x44,
  0x44, 0x44, 0x44, 0x04,
  0x40, 0x40, 0x04, 0x44,
  0x40, 0x00, 0x00, 0x00
};

const byte SWORD[] = {
  0x0D, 0x00, 0x00, 0x00,
  0x5D, 0xEE, 0xEE, 0xEF,
  0x4E, 0xFF, 0xFF, 0xF0,
  0x0E, 0x00, 0x00, 0x00
};

const byte GHOST[] = {
  0x00, 0x11, 0x00,
  0x02, 0x21, 0x10,
  0x2C, 0x21, 0xC1,
  0x22, 0xCC, 0x11,
  0x22, 0x21, 0x11,
  0x20, 0x22, 0x01
};

const byte GHOST_PROJECTILE[] = {
  0x12,
  0x21
};


/*
* PALETTES
*/
const byte UI_PALETTE[16] = {TRANSPARENT, DARKRED, RED, BRIGHTRED, GOLD, BROWN, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, BLACK, DARKGRAY, LIGHTGRAY, WHITE};
const byte ENEMY_PALETTE[16] = {TRANSPARENT, LIGHTPURPLE, PURPLE, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT, BLACK, DARKGRAY, LIGHTGRAY, WHITE};

#endif
