
#ifndef ENEMIES_H
#define ENEMIES_H

#include "geometry.h"

typedef struct {
	Vector2d pos;
	Vector2d dir;
	int dead;
	int type;
	int health;
	float time;
} Enemy;

void pushWave(float duration, int type);
void pushEnemy(int type);
void killEnemy(int id);
Box enemyHitbox(Enemy enemy);


void updateEnemies(float dt);
void renderEnemies();


#endif
