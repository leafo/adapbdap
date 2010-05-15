
#ifndef ENEMIES_H
#define ENEMIES_H

#include "geometry.h"

typedef struct {
	Vector2d pos;
	Vector2d dir;
	int dead;

	int type;
	int health;

	int loaded; // ready to shoot?
	float release; // y coordinate when bomb will be released

	float time;
} Enemy;


enum FireType {
	NO_FIRE,
	SINGLE_SHOT,
	CIRCLE_SHOT,
	WIDE_SHOT,
};

typedef struct {
	float speed;
	enum FireType fireType;
	float fireChance;
	float minFirePos;
} EnemyType;


void pushWave(int type);
void pushEnemy(int type, float startX);
void pushBomb(Vector2d start, Vector2d dir);
void killEnemy(int id);
Box enemyHitbox(Enemy enemy);


void updateEnemies(float dt);
void renderEnemies();

void updateBombs(float dt);
void renderBombs();

void fireBombs(Vector2d start, int type);

#endif
