#include "game.h"

byte controller;
SpriteObject player;
byte *heartSprite;
byte *keySprite;
SpriteObject objects[NUM_OBJECTS];
Room room;
Data data = {
  .health = 4,
  .hasKey = false,
  .gotKey1 = false,
  .unlockedDoor1 = false
};

//Called in setup()
void initGame() {
  byte* playerSprite = loadSprite(PLAYER, 4, 4);
  player.sprite = playerSprite;
  player.x = 58;
  player.y = 28;
  player.width = 4;
  player.height = 4;

  heartSprite = loadSprite(HEART, 4, 4);
  keySprite = loadSprite(KEY, 8, 4);

  loadRoom(&ROOM_START);
}

//Caled on vsync
void gameTick() {
  //digitalRead pins 30-37 into controller
  controller = PINC;

  
  //Fill in spriteObjects with tiles behind them
  clearSpriteObject(player);
  for(int i = 0; i < NUM_OBJECTS; i++) {
    SpriteObject obj = objects[i];
    if(obj.sprite) clearSpriteObject(obj);
  }

  //Move player according to input
  if(controller & UP){
    if(player.y == 0) {
      loadRoom(room.roomUp);
      player.y = PIXELS_HEIGHT - INFOBAR_HEIGHT - 4;
    } else if(getTile(player.x / 4, (player.y - 1) / 4)->passable && getTile((player.x + 3) / 4, (player.y - 1) / 4)->passable) {
      player.y--;
    }
  }
  if(controller & DOWN) {
    if(player.y == PIXELS_HEIGHT - INFOBAR_HEIGHT - 4) {
      loadRoom(room.roomDown);
      player.y = 0;
    } else if(getTile(player.x / 4, (player.y + 4) / 4)->passable && getTile((player.x + 3) / 4, (player.y + 4) / 4)->passable) {
      player.y++;
    } 
  }
  if(controller & RIGHT) {
    if(player.x == PIXELS_WIDTH - 4) {
      loadRoom(room.roomRight);
      player.x = 0;
    } else if(getTile((player.x + 4) / 4, player.y / 4)->passable && getTile((player.x + 4) / 4, (player.y + 3) / 4)->passable) {
      player.x++;
    }
  }
  if(controller & LEFT) {
    if(player.x == 0) {
      loadRoom(room.roomLeft);
      player.x = PIXELS_WIDTH - 4;
    } else if(getTile((player.x - 1) / 4, player.y / 4)->passable && getTile((player.x - 1) / 4, (player.y + 3) / 4)->passable) {
      player.x--;
    } 
  }

  //If the room has an update function, call it
  if(room.update) room.update();
    
  //Draw SpriteObjects
  for(int i = 0; i < NUM_OBJECTS; i++) {
    SpriteObject obj = objects[i];
    if(obj.sprite) drawSpriteObject(obj, room.palette);
  }
  drawSpriteObject(player, room.palette);

  //Draw bottom info bar
  for(byte i = 0; i < data.health; i++) {
    drawSprite(heartSprite, 4, 4, INFO_PALETTE, i*6 + 2, PIXELS_HEIGHT - 6);
  }
  if(data.hasKey) {
    drawSprite(keySprite, 8, 4, INFO_PALETTE, PIXELS_WIDTH - 10, PIXELS_HEIGHT - 6);
  } else {
    fillArea(PIXELS_WIDTH - 10, PIXELS_HEIGHT - 6, 8, 4, BLACK);
  }
}

void loadRoom(const Room *roomToLoad) {
  //If the current room has an uload function, call it
  if(room.unload) room.unload();

  memcpy_P(&room, roomToLoad, sizeof(Room)); //Load Room from progmem

  //Set background
  memset(displayBuffer, room.background, PIXELS_WIDTH * (PIXELS_HEIGHT - INFOBAR_HEIGHT));

  //Draw outline around info bar
  for(byte y = PIXELS_HEIGHT - INFOBAR_HEIGHT; y < PIXELS_HEIGHT; y++) {
    if(y == PIXELS_HEIGHT - INFOBAR_HEIGHT || y == PIXELS_HEIGHT - 1) {
      memset(displayBuffer + y * PIXELS_WIDTH, room.infoOutline, PIXELS_WIDTH);
    } else {
      setPixel(0, y, room.infoOutline);
      setPixel(PIXELS_WIDTH - 1, y, room.infoOutline);
    }
  }

  //Draw tiles
  for(byte x = 0; x < 30; x++) {
    for(byte y = 0; y < 13; y++) {
      drawTile(x,y);
    }
  }
  
  //If the room has a load function, call it
  if(room.load) room.load();
}

//Draws the tile at the corresponding tile coordinates (regular coords / 4)
void drawTile(byte x, byte y) {
  byte *sprite;
  if(x % 2 == 0) {
    sprite = room.tileset[(room.tiles[x/2 + y*15] >> 4) & 0xF].sprite;
  } else {
    sprite = room.tileset[room.tiles[x/2 + y*15] & 0xF].sprite; 
  }
  drawSprite(sprite, 4, 4, room.palette, x*4, y*4);
}

void setTile(byte x, byte y, byte tileID) {
  if(x % 2 == 0) {
    room.tiles[x/2 + y*15] = (tileID & 0xF) << 4;
  } else {
    room.tiles[x/2 + y*15] = tileID & 0xF;
  }
  fillArea(x*4, y*4, 4, 4, room.background);
  drawTile(x, y);
}

//Get the tile at sprite coordinates
Tile *getTile(byte x, byte y) {
  if(x % 2 == 0) {
    return &room.tileset[(room.tiles[x/2 + y*15] >> 4) & 0xF];
  } else {
    return &room.tileset[room.tiles[x/2 + y*15] & 0xF]; 
  }
}

void fillArea(byte x, byte y, byte width, byte height, byte color) {
  byte eX = x + width;
  byte eY = y + height;
  for(int cX = x; cX < eX; cX++) {
    for(int cY = y; cY < eY; cY++) {
      displayBuffer[cX + cY*PIXELS_WIDTH] = color;
    }
  }
}

//Redraws background around spriteObject
void clearSpriteObject(SpriteObject obj) {
  fillArea(obj.x, obj.y, obj.width, obj.height, room.background);
}

/**
 * Object Methods
*/
void keyUpdate() {

}