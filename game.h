
#ifndef GAME_H
#define GAME_H

#include "geometry.h"
#include "image.h"

#define MAX_BULLETS 400

typedef struct {
	Vector2d pos;
	Vector2d dir;
	int dead;
} Bullet;

struct {
	Vector2d pos;
	float startx, starty;

	int bulletCount; // active array slots
	Bullet bullets[MAX_BULLETS];

	int deadCount;
	int dead[MAX_BULLETS];
} player;

void initGame(); // initialize game data
void updateState(float dt);
void renderGame(float dt);
void renderBullets(float dt); // render all the player bullets
void pushBullet(); // shoot a bullet

#endif
