#ifndef ENTITY_H
#define ENTITY_H

#include <Arduino.h>
#include "game.h"
#include "sprite.h"

#define EntityType(entity) ENTITY_TYPES[(entity).id]

//A struct that holds a an ID, position, and arbitrary data.
struct Entity {
  byte id; //0 == no entity
  byte  x;
  byte  y;
  byte data;
};

//A struct that holds information about a type of entity and a tick function
struct EntityType {
  const byte* sprite;
  const byte* palette;
  const byte width;
  const byte height;
  void (*tick)(Entity*);
};

//Ghost stuff
#define GHOST_PROJECTILE_DIRECTION_PX bit(0)
#define GHOST_PROJECTILE_DIRECTION_NX bit(1)
#define GHOST_PROJECTILE_DIRECTION_PY bit(2)
#define GHOST_PROJECTILE_DIRECTION_NY bit(3)
void tickGhost(Entity* ghost);
void tickGhostProjectile(Entity* projectile);

/**
 * Entity type table
**/

#define ENTITY_NONE 0
#define ENTITY_PLAYER 1
#define ENTITY_KEY 2
#define ENTITY_SWORD 3
#define ENTITY_GHOST 4
#define ENTITY_GHOST_PROJECTILE 5
const EntityType ENTITY_TYPES[] = {
	{}, //No entity
	{   //Player
		.sprite = PLAYER,
		.palette = UI_PALETTE,
		.width = 4,
		.height = 4,
		.tick = nullptr
	},
	{   //Key
		.sprite = KEY,
		.palette = UI_PALETTE,
		.width = 8,
		.height = 4,
		.tick = nullptr
	},
	{	//Sword
		.sprite = SWORD,
		.palette = UI_PALETTE,
		.width = 8,
		.height = 4,
		.tick = nullptr
	},
	{   //Ghost
		.sprite = GHOST,
		.palette = ENEMY_PALETTE,
		.width = 6,
		.height = 6,
		.tick = &tickGhost
	},
	{   //Ghost Projectile
		.sprite = GHOST_PROJECTILE,
		.palette = ENEMY_PALETTE,
		.width = 2,
		.height = 2,
		.tick = &tickGhostProjectile
	}
};

boolean collides(Entity a, Entity b);

//Shortcut to draw Entity
inline void drawEntity(Entity* e) {
	drawSprite(EntityType(*e).sprite, EntityType(*e).width, EntityType(*e).height, EntityType(*e).palette, e->x, e->y);
}

#endif