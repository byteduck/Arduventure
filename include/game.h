#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "entity.h"
class Entity; //Forward declaration
#include "Arduventure.h"
#include "color.h"
#include "sprite.h"
#include "room.h"

#define INFOBAR_HEIGHT 8
#define GAME_HEIGHT (PIXELS_HEIGHT - INFOBAR_HEIGHT)
#define GAME_WIDTH PIXELS_WIDTH
#define NUM_ENTITIES 8

//These define which bit in the controller port reading is each button
union Controller {
  byte portReading : 5;
  struct Buttons {
    boolean up : 1;
    boolean down : 1;
    boolean left : 1;
    boolean right : 1;
    boolean action : 1;
    boolean pressedAction : 1; //Stores if action was pressed previous frame
  } buttons;
};

extern bool game_loading;
extern Controller controller;
extern Entity player;
extern Entity entities[NUM_ENTITIES];
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
void clearEntity(Entity* e);
int addEntity(Entity e);

#endif
