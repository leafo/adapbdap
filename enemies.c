

#include "enemies.h"

#include "common.h"
#include "renderer.h"
#include "timer.h"
#include "enemy1.ply.h"

#include "game.h"

#define MAX_ENEMIES 50

defList(Enemy, enemies, MAX_ENEMIES);
float yLimit = 24;


int launchEnemy(void *timer) {
	pushEnemy(0);
	return 0;
}

void pushEnemy(int type) {
	int eid = getFreeId(enemies);
	if (eid >= MAX_ENEMIES) return;
	
	Enemy e = {0};
	Vector2d start = {
		mt_rand() % (2 * (int)viewport.width) - viewport.width,
		viewport.height + 2 
	};

	e.pos = start;
	switch (type) {
		case 0:
			e.dir = vec2d(0,-1);
			break;
	}

	enemies[eid] = e;
}

void pushWave(float interval, int type) {
	createTimer(interval, launchEnemy);
}

void updateEnemies(float dt) { // call before update bullets
	int i;
	for (i = 0; i < enemies_count; i++) {
		if (enemies[i].dead) continue;

		// see if they hit a bullet
		Box hitbox = enemyHitbox(enemies[i]);
		int killed = false;
		int b;
		Bullet * const bullets = player.bullets;
		for (b = 0; b < player.bulletCount; b++) {
			if (bullets[b].dead) continue;
			if (boxInBox(bulletHitbox(bullets[b]), hitbox)) {
				killBullet(b);
				killed = true;
				onKillEnemy(enemies[i].type);
				break;
			}
		}

		if (killed || enemies[i].pos.y < -yLimit) {
			killEnemy(i);
			continue;
		}

		// update pos
		enemies[i].pos.x += enemies[i].dir.x * dt * 10;
		enemies[i].pos.y += enemies[i].dir.y * dt * 10;
	}
}

void renderEnemies() {
	glColor4f(0.625,1,0.566,1);
	int i;
	for (i = 0; i < enemies_count; i++) {
		if (enemies[i].dead) continue;

		glPushMatrix();
		glTranslatef(enemies[i].pos.x, enemies[i].pos.y, 0);
		glScalef(.7,.7,.7);
		glRotatef(-45, 0,1,0);
		renderMesh(enemy1_ply, enemy1_normals_ply, enemy1_ply_len/3);
		glPopMatrix();
	}

	glColor4f(1,1,1,1);
}

Box enemyHitbox(Enemy enemy) {
	Box out = {
		enemy.pos.x-.5,
		enemy.pos.y+1,

		enemy.pos.x+.5,
		enemy.pos.y-.5
	};
	return out;
}

// render the enemy hitboxes
void renderEnemyHitbox() {
	int i = 0;
	for (i = 0; i < enemies_count; i++) {
		if (enemies[i].dead) continue;
		Box b = enemyHitbox(enemies[i]);
		rect(b.x1, b.y1, b.x2, b.y2);
	}
}

void killEnemy(int i) {
	enemies[i].dead = 1;
	killId(enemies, i);
}

