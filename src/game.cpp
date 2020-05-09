#include "game.h"

Controller controller;
Entity player = {
  .id = ENTITY_PLAYER,
  .x = 58,
  .y = 28,
  .data = {.playerData = {
    .health = 4,
    .hasKey = false,
    .gotKey1 = false,
    .unlockedDoor1 = false,
    .hasSword = false,
  }}
};
Entity entities[NUM_ENTITIES];
Room room;

const Room* checkpointRoom = &ROOM_START;
Entity checkpointPlayer = player;

//Called in setup()
void initGame() {
  loadRoom(&ROOM_START);
}

//Called after each frame is drawn.
void gameTick() {
  //digitalRead pins 30-37 into controller
  controller.portReading = PINC;
  
  //Fill in Entities with background behind them
  clearEntity(&player);
  for(int i = 0; i < NUM_ENTITIES; i++) {
    Entity ent = entities[i];
    if(ent.id) clearEntity(&ent);
  }

  //Move player according to input
  if(controller.buttons.up){
    if(player.y == 0) {
      if(room.roomUp){
        player.y = GAME_HEIGHT - 4;
        loadRoom(room.roomUp);
      }
    } else if(getTile(player.x / 4, (player.y - 1) / 4)->passable && getTile((player.x + 3) / 4, (player.y - 1) / 4)->passable) {
      player.y--;
    }
  }
  if(controller.buttons.down) {
    if(player.y == GAME_HEIGHT - 4) {
      if(room.roomDown){
        player.y = 0;
        loadRoom(room.roomDown);
      }
    } else if(getTile(player.x / 4, (player.y + 4) / 4)->passable && getTile((player.x + 3) / 4, (player.y + 4) / 4)->passable) {
      player.y++;
    } 
  }
  if(controller.buttons.right) {
    if(player.x == GAME_WIDTH - 4) {
      if(room.roomRight){
        player.x = 0;
        loadRoom(room.roomRight);
      }
    } else if(getTile((player.x + 4) / 4, player.y / 4)->passable && getTile((player.x + 4) / 4, (player.y + 3) / 4)->passable) {
      player.x++;
    }
  }
  if(controller.buttons.left) {
    if(player.x == 0) {
      if(room.roomLeft){
        player.x = GAME_WIDTH - 4; 
        loadRoom(room.roomLeft);
      }
    } else if(getTile((player.x - 1) / 4, player.y / 4)->passable && getTile((player.x - 1) / 4, (player.y + 3) / 4)->passable) {
      player.x--;
    } 
  }

  //If the room has an update function, call it
  if(room.update) room.update();

  //If pressing the action button for the first time this frame, attack
  fillArea(PIXELS_WIDTH / 2 - 4, PIXELS_HEIGHT - 6, 8, 4, BLACK);
  if(controller.buttons.action && player.data.playerData.hasSword) {
    drawSprite(SWORD, 8, 4, UI_PALETTE, PIXELS_WIDTH / 2 - 4, PIXELS_HEIGHT - 6);
    if(!controller.buttons.pressedAction) {
      for(byte i = 0; i < NUM_ENTITIES; i++) {
        Entity* e = &entities[i];
        if(e->id == ENTITY_GHOST)  {
          if(withinBox(*e, player.x - 1, player.y - 1, 6, 6)) {
            e->id = 0;
          }
        }
      }
    }
  }
  controller.buttons.pressedAction = controller.buttons.action;
    
  //Draw & tick Entities
  for(int i = 0; i < NUM_ENTITIES; i++) {
    if(entities[i].id) {
      if(EntityType(entities[i]).tick) EntityType(entities[i]).tick(&entities[i]);
      if(entities[i].id) drawEntity(&entities[i]); //We check id again to make sure the entity wasn't removed while ticking
    }
  }

  //If we died, load from checkpoint
  if(player.data.playerData.health == 0) {
    player = checkpointPlayer;
    player.data.playerData.health = 4;
    loadRoom(checkpointRoom);
  }

  //Draw player
  drawEntity(&player);

  //Draw health
  fillArea(2, PIXELS_HEIGHT - 6, 24, 4, BLACK);
  for(byte i = 0; i < player.data.playerData.health; i++) {
    drawSprite(HEART, 4, 4, UI_PALETTE, i*6 + 2, PIXELS_HEIGHT - 6);
  }

  //If we have a key, draw it
  if(player.data.playerData.hasKey) {
    drawSprite(KEY, 8, 4, UI_PALETTE, GAME_WIDTH - 10, PIXELS_HEIGHT - 6);
  } else {
    fillArea(GAME_WIDTH - 10, PIXELS_HEIGHT - 6, 8, 4, BLACK);
  }
}

void loadRoom(const Room *roomToLoad) {
  //If the current room has an unload function, call it
  if(room.unload) room.unload();

  //Remove all of the entities
  for(int i = 0; i < NUM_ENTITIES; i++) {
    entities[i].id = ENTITY_NONE;
  }

  memcpy_P(&room, roomToLoad, sizeof(Room)); //Load Room from progmem

  //If the new room is a checkpoint, save
  if(room.isCheckpoint) {
    checkpointRoom = roomToLoad;
    checkpointPlayer = player;
  }

  //Set background
  memset(displayBuffer, room.background, GAME_WIDTH * GAME_HEIGHT);

  //Draw outline around info bar
  for(byte y = GAME_HEIGHT; y < PIXELS_HEIGHT; y++) {
    if(y == GAME_HEIGHT || y == PIXELS_HEIGHT - 1) {
      memset(displayBuffer + y * GAME_WIDTH, room.infoOutline, GAME_WIDTH);
    } else {
      setPixel(0, y, room.infoOutline);
      setPixel(GAME_WIDTH - 1, y, room.infoOutline);
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
      displayBuffer[cX + cY*GAME_WIDTH] = color;
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