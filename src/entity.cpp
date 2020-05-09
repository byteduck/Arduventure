#include "entity.h"

//Tests if two Entities collide.
boolean collides(Entity a, Entity b) {
  return a.x < b.x + EntityType(b).width && a.x + EntityType(a).width > b.x && a.y < b.y + EntityType(b).height && a.y + EntityType(a).height > b.y;
}

//Test if an Entity is within a box.
boolean withinBox(Entity a, int x, int y, int width, int height) {
	return a.x < x + width && a.x + EntityType(a).width > x && a.y < y + height && a.y + EntityType(a).height > y;
}

//Test if an entity is colliding with a non-passable tile.
boolean collidesWithTile(Entity entity) {
	EntityType type = EntityType(entity);
	double x = entity.x;
	double y = entity.y;
	double eX = entity.x + type.width - 1;
	double eY = entity.y + type.height - 1;
	while(1) {
		while(1) {
			if(!getTile(x/4, y/4)->passable) return true;
			if(y == eY) break;
			y = min(y + 4, eY);
		}
		if(x == eX) break;
		x = min(x + 4, eX);
	}
	return false;
}

//Entity data is used for projectile timer
void tickGhost(Entity* ghost){
	if(ghost->data.ghostData.shootTime == 0) {
		Entity toShoot = {
			.id = ENTITY_GHOST_PROJECTILE,
			.x = ghost->x + 2,
			.y = ghost->y + 2,
			.data = {.data = 0}
		};

		if(player.x + 4 > ghost->x - 1 && player.x < ghost->x + 7) {
			if(player.y > ghost->y) {
				toShoot.data.projectileData.PY = true;
			} else {
				toShoot.data.projectileData.NY = true;
			}
			addEntity(toShoot);
			ghost->data.ghostData.shootTime = 50;
		} else if(player.y + 4 > ghost->y - 1 && player.y < ghost->y + 7) {
			if(player.x > ghost->x) {
				toShoot.data.projectileData.PX = true;
			} else {
				toShoot.data.projectileData.NX = true;
			}
			addEntity(toShoot);
			ghost->data.ghostData.shootTime = 50;
		}
	} else {
		ghost->data.ghostData.shootTime--;
	}
}

void tickGhostProjectile(Entity* projectile) {
	if(projectile->data.projectileData.PX) projectile->x++;
	if(projectile->data.projectileData.NX) projectile->x--;
	if(projectile->data.projectileData.PY) projectile->y++;
	if(projectile->data.projectileData.NY) projectile->y--;
	if(projectile->x > PIXELS_WIDTH - 2 || projectile->y > GAME_HEIGHT - 2 || collidesWithTile(*projectile)) {
		projectile->id = ENTITY_NONE;
	}
	if(collides(*projectile, player)) {
		player.data.playerData.health--;
		projectile->id = ENTITY_NONE;
	}
}