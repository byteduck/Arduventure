#include "entity.h"

//Tests if two Entities collide.
boolean collides(Entity a, Entity b) {
  return a.x < b.x + EntityType(b).width && a.x + EntityType(a).width > b.x && a.y < b.y + EntityType(b).height && a.y + EntityType(a).height > b.y;
}

//Entity data is used for projectile timer
void tickGhost(Entity* ghost){
	if(ghost->data == 0) {
		Entity toShoot = {
			.id = ENTITY_GHOST_PROJECTILE,
			.x = ghost->x,
			.y = ghost->y,
			.data = 0
		};

		if(player.x == ghost->x) {
			if(player.y > ghost->y) {
				toShoot.data = GHOST_PROJECTILE_DIRECTION_PY;
			} else {
				toShoot.data = GHOST_PROJECTILE_DIRECTION_NY;
			}
			addEntity(toShoot);
			ghost->data = 0xFF;
		} else if(player.y == ghost->y) {
			if(player.x > ghost->x) {
				toShoot.data = GHOST_PROJECTILE_DIRECTION_PX;
			} else {
				toShoot.data = GHOST_PROJECTILE_DIRECTION_NX;
			}
			addEntity(toShoot);
			ghost->data = 0xFF;
		}
	} else {
		ghost->data--;
	}
}

void tickGhostProjectile(Entity* projectile) {
	if(projectile->data & GHOST_PROJECTILE_DIRECTION_PX) projectile->x++;
	if(projectile->data & GHOST_PROJECTILE_DIRECTION_NX) projectile->x--;
	if(projectile->data & GHOST_PROJECTILE_DIRECTION_PY) projectile->y++;
	if(projectile->data & GHOST_PROJECTILE_DIRECTION_NY) projectile->y--;
	if(projectile->x > PIXELS_WIDTH || projectile->y > PIXELS_HEIGHT) {
		projectile->id = ENTITY_NONE;
	}
	if(collides(*projectile, player)) {
		data.health--;
		projectile->id = ENTITY_NONE;
	}
}