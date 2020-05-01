#ifndef SPRITE_H
#define SPRITE_H

#include <Arduino.h>
#include "Arduventure.h"
#include "color.h"

//A struct that holds a sprite, position, and dimensions.
struct SpriteObject {
  const byte* sprite;
  byte  x;
  byte  y;
  byte  width : 4;
  byte  height : 4;
};

void drawSprite(const byte *sprite, byte spriteWidth, byte spriteHeight, const byte* palette, int x, int y);
boolean collides(SpriteObject a, SpriteObject b);

//Draws pixel at [index] in displayBuffer if rightmost bit of [color] isn't set.
//Used in drawSprite.
inline void conditionalPixelDraw(int index, byte color) {
  if(!(color & 1)) displayBuffer[index] = color;
}

//Shortcut to draw spriteObject
inline void drawSpriteObject(SpriteObject obj, byte* palette){
  drawSprite(obj.sprite, obj.width, obj.height, palette, obj.x, obj.y);
}

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

const byte INFO_PALETTE[] = {
  TRANSPARENT, DARKRED, RED, BRIGHTRED, GOLD
};

#endif
