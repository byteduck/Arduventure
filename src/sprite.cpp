#include "sprite.h"

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
