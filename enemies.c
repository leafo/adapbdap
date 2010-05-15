

#include "enemies.h"

#include "common.h"
#include "renderer.h"
#include "timer.h"

#include "enemy1.ply.h"
#include "enemy2.ply.h"
#include "bomb.ply.h"

#include "game.h"

#define MAX_ENEMIES 200

int currentWave = 0;
int waves[] = {
	// 3,3,3,3, -1,
	1,0,2,3,1,2,0,3,1,0 -1
};


EnemyType enemyTypes[] = {
	{1.0, SINGLE_SHOT, .333, 0.0},
	{1.5, SINGLE_SHOT, 1.0, 0.0},
	{1.0, CIRCLE_SHOT, 1.0, 3.0},
};

defList(Bullet, bombs, 200);
defList(Enemy, enemies, MAX_ENEMIES);
float yLimit = 24;

static int waveType = 0; // current type of wave
static int waveTicks = 0; // ticks left on wave

int launchEnemy(void *timer) {
	switch(waveType) {
		case 1: {
			float w = 5;
			float r = (mt_rand() % 1000) / 1000.0;
			pushEnemy(0, r*2*w - w);
			break;
		}
		case 2:
			if (waveTicks == 1) {
				pushEnemy(1, 0);
			} else {
				pushEnemy(0, -waveTicks*2);
				pushEnemy(0, waveTicks*2);
			}
			break;
		case 3: // drop bomb enemies;
			if (waveTicks > 2) {
				pushEnemy(2, -5);
			} else {
				pushEnemy(2, 5);
			}
			break;
	}

	waveTicks--;	
	if (waveTicks <= 0) {
		int next = waves[currentWave];
		if (next == -1) next = waves[currentWave = 0];
		currentWave++;

		pushWave(next);
		return true;
	}

	return false;
}

void pushEnemy(int type, float startX) {
	int eid = getFreeId(enemies);
	if (eid >= MAX_ENEMIES) return;
	
	Enemy e = {0};
	Vector2d start = {
		startX,
		viewport.height + 2 
	};

	e.type = type;
	e.pos = start;

	EnemyType t = enemyTypes[e.type];
	e.dir = scale(vec2d(0,-1), t.speed);

	if (t.fireChance == 1.0 || rand_float() <= t.fireChance) {
		e.loaded = true;
		e.release = (mt_rand() % 1000) / 1000.0 * viewport.height + t.minFirePos;
	}

	enemies[eid] = e;
}

void pushWave(int type) {
	waveType = type;
	float interval = 1;
	switch(waveType) {
		case 0: // rest for 1 seconds
			interval = 1;
			waveTicks = 1;
			break;
		case 1: // random enemies
			interval = .5;
			waveTicks = 10;
			break;
		case 2:
			interval = .2;
			waveTicks = 5;
			break;
		case 3:
			interval = .4;
			waveTicks = 4;
			break;
	}
	// printf("starting wave type %d\n", type);
	// launchEnemy(0);
	createTimer(interval, launchEnemy);
}


void updateBombs(float dt) {
	int i;
	for (i = 0; i < bombs_count; i++) {
		if (bombs[i].dead) continue;

		Vector2d p = bombs[i].pos;
		// if outside of map, kill
		if (p.x < -viewport.width || p.x > viewport.width ||
				p.y > viewport.height || p.y < -viewport.height) {
			bombs[i].dead = 1;
			killId(bombs, i);
			continue;
		}

		// update pos
		bombs[i].pos.x += bombs[i].dir.x * dt * 20;
		bombs[i].pos.y += bombs[i].dir.y * dt * 20;
	}
}

void renderBombs() {
	int i;
	glColor4f(.3,.3,1, .8);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	for (i = 0; i < bombs_count; i++) {
		if (bombs[i].dead) continue;

		glPushMatrix();
		glTranslatef(bombs[i].pos.x, bombs[i].pos.y, 0);

		glRotatef(bombs[i].pos.x*15, 0,1,0);
		glRotatef(bombs[i].pos.y*15, 0,0,1);
		glScalef(.5,.5,.5);

		renderMesh(bomb_ply, bomb_normals_ply, bomb_ply_len/3);

		glPopMatrix();

	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
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

		// shoot a bomb?
		if (enemies[i].loaded && enemies[i].pos.y < enemies[i].release) {
			enemies[i].loaded = false;
			
			fireBombs(enemies[i].pos, enemyTypes[enemies[i].type].fireType);
		}

	}
}

void renderEnemies() {
	int i;
	for (i = 0; i < enemies_count; i++) {
		if (enemies[i].dead) continue;

		glPushMatrix();
		glTranslatef(enemies[i].pos.x, enemies[i].pos.y, 0);

		switch(enemies[i].type) {
			case 0:
				glColor4f(0.625,1,0.566,1); // green
				break;
			case 1:
				glColor4f(1, .639, .625, 1); // red
				glRotatef(enemies[i].pos.y*15, 0,1,0);
				break;
			case 2:
				glColor4f(.949, .615, 1, 1); // red
				break;
		}

		glScalef(.7,.7,.7);
		glRotatef(-45, 0,1,0);
		
		switch(enemies[i].type) {
			case 0:
			case 1:
				renderMesh(enemy1_ply, enemy1_normals_ply, enemy1_ply_len/3);
				break;
			case 2:
				renderMesh(enemy2_ply, enemy2_normals_ply, enemy2_ply_len/3);
				break;
		}


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

void fireBombs(Vector2d start, int type) {
	int i;
	int circleMax = 10;

	int wideMax = 5;

	switch(type) {
		case SINGLE_SHOT:
			pushBomb(start, normalize(diff(player.pos, start)));
			break;
		case CIRCLE_SHOT:
			for (i = 0; i < circleMax; i++) {
				float a = 2.0*M_PI*i/circleMax;
				pushBomb(start, vec2d(cos(a), sin(a) - .5));
			}
			break;
		case WIDE_SHOT:
			for (i = 0; i < wideMax; i++) {
				float a = .25*M_PI*i/wideMax;
				pushBomb(start, vec2d(cos(a), sin(a) - .5));
			}
			break;
	}
}

void pushBomb(Vector2d start, Vector2d dir) {
	int bid = getFreeId(bombs);
	if (bid >= 200) return;

	Bullet b = {0};
	b.pos = start;
	b.dir = dir;
	// b.dir = normalize(diff(player.pos, start));

	bombs[bid] = b;
}

