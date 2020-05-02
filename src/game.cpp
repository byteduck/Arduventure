#include "game.h"

byte controller;
Entity player = {
  .id = ENTITY_PLAYER,
  .x = 58,
  .y = 28
};
Entity entities[NUM_ENTITIES];
Room room;
Data data = {
  .health = 4,
  .hasKey = false,
  .gotKey1 = false,
  .unlockedDoor1 = false,
  .hasSword = false,
};

//Called in setup()
void initGame() {
  loadRoom(&ROOM_START);
}

//Called after each frame is drawn.
void gameTick() {
  //digitalRead pins 30-37 into controller
  controller = PINC;
  
  //Fill in Entities with background behind them
  clearEntity(&player);
  for(int i = 0; i < NUM_ENTITIES; i++) {
    Entity ent = entities[i];
    if(ent.id) clearEntity(&ent);
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
    
  //Draw & tick Entities
  for(int i = 0; i < NUM_ENTITIES; i++) {
    if(entities[i].id) {
      if(EntityType(entities[i]).tick) EntityType(entities[i]).tick(&entities[i]);
      if(entities[i].id) drawEntity(&entities[i]); //We check id again to make sure the entity wasn't removed while ticking
    }
  }
  drawEntity(&player);

  //Draw health
  for(byte i = 0; i < data.health; i++) {
    drawSprite(HEART, 4, 4, UI_PALETTE, i*6 + 2, PIXELS_HEIGHT - 6);
  }

  //If we have a key, draw it
  if(data.hasKey) {
    drawSprite(KEY, 8, 4, UI_PALETTE, PIXELS_WIDTH - 10, PIXELS_HEIGHT - 6);
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

//Set tile at position
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

//Fill area with color
void fillArea(byte x, byte y, byte width, byte height, byte color) {
  byte eX = x + width;
  byte eY = y + height;
  for(int cX = x; cX < eX; cX++) {
    for(int cY = y; cY < eY; cY++) {
      displayBuffer[cX + cY*PIXELS_WIDTH] = color;
    }
  }
}

//Redraws background around entity
void clearEntity(Entity* e) {
  fillArea(e->x, e->y, EntityType(*e).width, EntityType(*e).height, room.background);
}

//Adds an entity to the first available slot and returns the slot it was added to. -1 if no space.
int addEntity(Entity e) {
  int slot = 0;
  while(slot < NUM_ENTITIES) {
    if(!entities[slot].id) {
      entities[slot] = e;
      return slot;
    }
    slot++;
  }
  return -1;
}