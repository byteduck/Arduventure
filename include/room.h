/**
 * A room is 30x13 tiles. Each tile is 4 bits.
 * Each room also loads a certain tileset, dictating which 4-bit values correspond to which 4x4 sprite.
 * Doing things this way means we can load just 195 bytes + tile/sprite data for each room
 * instead of a whole 7.2kb of framebuffer :)
 * 
 * Palettes of rooms generally follow the same structure:
 * TRANSPARENT, BLACK, darkcolorA, mediumColorA, brightColorA, darkColorB, ...., WHITE
 */

#ifndef ROOM_H
#define ROOM_H

#include <Arduino.h>
#include "tile.h"
#include "sprite.h"

struct Room {
  byte background;
  byte infoOutline;
  byte tiles[195];
  Tile tileset[16];
  byte palette[16];
  const Room* roomUp;
  const Room* roomDown;
  const Room* roomLeft;
  const Room* roomRight;
  void (*load)(void);
  void (*unload)(void);
  void (*update)(void);
};

extern const Room PROGMEM ROOM_2;
extern const Room PROGMEM ROOM_START;
extern const Room PROGMEM ROOM_KEY1;
extern const Room PROGMEM ROOM_DOOR1;
extern const Room PROGMEM ROOM_END;

#endif
