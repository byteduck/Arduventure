#include "sprite.h"

//Load a sprite from PROGMEM into memory. Sprite's width must be multiple of 2.
byte* loadSprite(const byte* PROGMEM sprite, byte width, byte height) {
  size_t spriteSize = (width/2)*height;
  byte* spriteMem = malloc(spriteSize);
  memcpy_P(spriteMem, sprite, spriteSize);
  return spriteMem;
}

//Unload a sprite from memory.
byte* unloadSprite(byte *spriteMem) {
  free(spriteMem);
}

//Sprite's width must be a multiple of 4.
//If the rightmost bit in a palette color is set, it's transparent
void drawSprite(const byte *sprite, byte spriteWidth, byte spriteHeight, const byte* palette, int x, int y) {
  int oX = x;
  int oY = y;
  for(byte y = 0; y < spriteHeight; y++) {
    for(byte x = 0; x < spriteWidth/2; x++) {
      byte spriteRow = sprite[x + y * (spriteWidth/2)];
      int startIndex = x*2 + oX + (y + oY) * PIXELS_WIDTH ;
      conditionalPixelDraw(startIndex, palette[(spriteRow >> 4) & 0b1111]);
      conditionalPixelDraw(startIndex + 1, palette[(spriteRow) & 0b1111]);
    }
  }
}

//Tests if two SpriteObjects collide.
boolean collides(SpriteObject a, SpriteObject b) {
  return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y;
}
