
#ifndef ENEMIES_H
#define ENEMIES_H

#include "geometry.h"
#include "game.h"

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

typedef int (*EnemyBehavior)(Enemy * e, float dt);

enum FireType {
	NO_FIRE,
	SINGLE_SHOT,
	CIRCLE_SHOT,
	WIDE_SHOT,
	BOSS_SHOT,
};

typedef struct {
	float speed;
	enum FireType fireType;
	float fireChance;
	float minFirePos;
	int health;

	EnemyBehavior update;	
} EnemyType;


void pushWave(int type);
int pushEnemy(int type, float startX);
void pushBomb(Vector2d start, Vector2d dir);
void killEnemy(int id);
Box enemyHitbox(Enemy enemy);
Box bombHitbox(Bullet bomb);

void updateEnemies(float dt);
void clearEnemies();
void renderEnemies();

void updateBombs(float dt);
void renderBombs();

void fireBombs(Vector2d start, int type);

#endif
