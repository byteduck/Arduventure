#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "Arduventure.h"
#include "color.h"
#include "sprite.h"
#include "room.h"

#define INFOBAR_HEIGHT 8
#define NUM_OBJECTS 4

//These define which bit in the controller port reading is each button
#define UP bit(0)
#define DOWN bit(1)
#define LEFT bit(2)
#define RIGHT bit(3)

struct Data {
  byte health : 3;
  bool hasKey : 1;
  bool gotKey1 : 1;
  bool unlockedDoor1 : 1;
};
extern Data data;

extern bool game_loading;
extern byte controller;
extern SpriteObject player;
extern SpriteObject objects[NUM_OBJECTS];
extern Room room;

// Convenient method to set a pixel at an x and y position in the display buffer.
inline void setPixel(byte x, byte y, byte color) {
  displayBuffer[x + y * PIXELS_WIDTH] = color;
}

void initGame();
void gameTick();
void drawTile(byte x, byte y);
void setTile(byte x, byte y, byte tileID);
Tile *getTile(byte x, byte y);
void loadRoom(const Room *roomToLoad);
void fillArea(byte x, byte y, byte width, byte height, byte color);
void clearSpriteObject(SpriteObject obj);

/**
 *  Object Functions
**/
void keyUpdate();

#endif
