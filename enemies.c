

#include "enemies.h"

#include "common.h"
#include "renderer.h"
#include "timer.h"

#include "enemy1.ply.h"
#include "enemy2.ply.h"
#include "boss1.ply.h"
#include "bomb.ply.h"

#include "animate.h"

#define MAX_ENEMIES 200
#define MAX_BOMBS 200

static int waveType = 0; // current type of wave
static int waveTicks = 0; // ticks left on wave

struct {
	int id; // the enemy id of boss sprite
	float rate; // how fast it is firing
	int counter;
	int phase;
} boss;

int currentWave = 0;
int waves[] = {
	// 3,3,3,3, -1,
	1,0,2,3,1,2,0,3,1,4,0, -1
	// 4, -1
};

EnemyType enemyTypes[];

int behavior_moveDown(Enemy *e, float dt) {
	// e->pos.x += e->dir.x * dt * 10;
	e->pos.y += e->dir.y * dt * 10;

	// shoot a bomb?
	if (e->loaded && e->pos.y < e->release) {
		e->loaded = false;
		fireBombs(e->pos, enemyTypes[e->type].fireType);
	}

	return 0;
}

int behavior_boss(Enemy *e, float dt) {
	if (e->pos.y > 9.0) {
		e->pos.y += e->dir.y * dt * 10;
		return 0;
	}

	// phase one		
	e->time += dt;
	if (e->time > boss.rate) {
		e->time -= boss.rate;
		switch (boss.phase) {
			case 0: 
				boss.phase = 1;
				boss.counter = 0;
				break;
			case 1:
				boss.rate = 0.5;
				fireBombs(sum(e->pos, vec2d(4, 0)), SINGLE_SHOT);
				fireBombs(sum(e->pos, vec2d(-4, 0)), SINGLE_SHOT);

				boss.counter++;
				if (boss.counter > 5) {
					boss.counter = 0;
					boss.phase = 2;
					boss.rate = 0.5;
				}

				break;
			case 2:
				boss.rate = .1;
				fireBombs(sum(e->pos, vec2d(1, -4)), SINGLE_SHOT);
				fireBombs(sum(e->pos, vec2d(-1, -4)), SINGLE_SHOT);

				boss.counter++;
				if (boss.counter > 10) {
					boss.rate = 0.5;
					boss.phase = 4;
					boss.counter = 0;
				}

				break;
			case 3: 
				boss.rate = .3;
				if (boss.counter % 2) {
					fireBombs(sum(e->pos, vec2d(5, 0)), WIDE_SHOT);
				} else {
					fireBombs(sum(e->pos, vec2d(-5, 0)), WIDE_SHOT);
				}

				boss.counter++;
				if (boss.counter > 10) {
					boss.rate = .5;
					boss.phase = 0;
					boss.counter = 0;
				}
				break;
			case 4: // slow spin
				boss.rate = .1;
				float a = boss.counter*20*M_PI/180.0;
				pushBomb(sum(e->pos, vec2d(0, 0)), vec2d(cos(a), sin(a)));
				pushBomb(sum(e->pos, vec2d(0, 0)), vec2d(cos(a+M_PI), sin(a+M_PI)));
				
				boss.counter++;
				if (boss.counter > 20) {
					boss.rate = .5;
					boss.phase = 3;
					boss.counter = 0;
				}
		}
	}

	return 0;
}


EnemyType enemyTypes[] = {
	{1.0, SINGLE_SHOT, .333, 0.0, 1, behavior_moveDown},
	{1.5, SINGLE_SHOT, 1.0, 0.0, 1, behavior_moveDown},
	{1.0, CIRCLE_SHOT, 1.0, 3.0, 2, behavior_moveDown},
	{.8, BOSS_SHOT, 1.0, 3.0, 30, behavior_boss}, // the boss
};

defList(Bullet, bombs, MAX_BOMBS);
defList(Enemy, enemies, MAX_ENEMIES);
float yLimit = 24;



// get the id of the next wave
static int nextWave() {
	int next = waves[currentWave];
	if (next == -1) next = waves[currentWave = 0];
	currentWave++;
	return next;
}

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
		case 4: // a boss
			if (boss.id == -1) pushBoss();
			// don't let wave end until boss is dead
			if (enemies[boss.id].health > 0) waveTicks++;
			break;

	}

	waveTicks--;	
	if (waveTicks <= 0) {
		pushWave(nextWave());
		return true;
	}

	return false;
}

int pushBoss() {
	boss.id = pushEnemy(3, 0);
	boss.phase = 0;
	printf("pushed boss: %d\n", boss.id);
}

int pushEnemy(int type, float startX) {
	int eid = getFreeId(enemies);
	if (eid >= MAX_ENEMIES) return;
	
	Enemy e = {0};
	Vector2d start = {
		startX,
		viewport.height + 4 
	};

	e.type = type;
	e.pos = start;

	EnemyType t = enemyTypes[e.type];
	e.dir = scale(vec2d(0,-1), t.speed);

	if (t.fireChance == 1.0 || rand_float() <= t.fireChance) {
		e.loaded = true;
		e.release = (mt_rand() % 1000) / 1000.0 * viewport.height + t.minFirePos;
	}

	e.health = t.health;

	enemies[eid] = e;
	return eid;
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
		case 4:
			boss.id = -1;
			interval = 1.0;
			waveTicks = 1;
			break;
	}
	// printf("starting wave type %d\n", type);
	// launchEnemy(0);
	createTimer(interval, launchEnemy);
}


void updateBombs(float dt) {
	Box playerBox = playerHitbox();
	int i;
	for (i = 0; i < bombs_count; i++) {
		if (bombs[i].dead) continue;

		int dead = 0;
		if (boxInBox(playerBox, bombHitbox(bombs[i]))) {
			onPlayerHit();
			dead = 1;
		}

		Vector2d p = bombs[i].pos;
		// if outside of map, kill
		if (dead || p.x < -viewport.width || p.x > viewport.width ||
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

void clearBombs() {
	int i;
	for (i = 0; i < bombs_count; i++) {
		if (bombs[i].dead) continue;
		bombs[i].dead = 1;
		killId(bombs, i);
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
	Box playerBox = playerHitbox();
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
				enemies[i].health--;
				if (enemies[i].health == 0) {
					killed = true;
					onKillEnemy(enemies[i].type);
					pushAnimation(enemies[i].pos, 0);
				}
				break;
			}
		}
		
		// see if they hit the player
		if (boxInBox(hitbox, playerBox)) {
			killBullet(b);
			killed = true;
			pushAnimation(enemies[i].pos, 0);
			onPlayerHit();
		}

		if (killed || enemies[i].pos.y < -yLimit) {
			killEnemy(i);
			continue;
		}

		// update state
		EnemyBehavior update = enemyTypes[enemies[i].type].update;
		update(enemies+i, dt);
	}
}

void clearEnemies() {
	currentWave = 0;
	boss.id = -1;
	int i;
	for (i = 0; i < enemies_count; i++) {
		if (enemies[i].dead) continue;
		killEnemy(i);
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
			case 3:
				glColor4f(.819, 1, .784, 1); // red
				break;
		}
		
		switch(enemies[i].type) {
			case 0:
			case 1:
				glScalef(.7,.7,.7);
				glRotatef(-45, 0,1,0);
				renderMesh(enemy1_ply, enemy1_normals_ply, enemy1_ply_len/3);
				break;
			case 2:
				glScalef(.7,.7,.7);
				glRotatef(-45, 0,1,0);
				renderMesh(enemy2_ply, enemy2_normals_ply, enemy2_ply_len/3);
				break;
			case 3:
				glScalef(1.8, 1.8, 1.8);
				renderMesh(boss1_ply, boss1_normals_ply, boss1_ply_len/3);

		}


		glPopMatrix();
	}

	glColor4f(1,1,1,1);
}

Box enemyHitbox(Enemy enemy) {
	if (enemy.type == 3) {
		Box out = {
			enemy.pos.x-4.5,
			enemy.pos.y+1,

			enemy.pos.x+4.5,
			enemy.pos.y-2.0
		};
		return out;
	}

	Box out = {
		enemy.pos.x-.5,
		enemy.pos.y+1,

		enemy.pos.x+.5,
		enemy.pos.y-.5
	};
	return out;
}

Box bombHitbox(Bullet bomb) {
	Box out = {
		bomb.pos.x-.3,
		bomb.pos.y+.2,

		bomb.pos.x+.3,
		bomb.pos.y-.2
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


	for (i = 0; i < bombs_count; i++) {
		if (bombs[i].dead) continue;
		Box b = bombHitbox(bombs[i]);
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
		case WIDE_SHOT: {
			float spread = M_PI*50.0/180.0;
			float a = angle(diff(player.pos, start)) - spread/2;

			for (i = 0; i < wideMax; i++) {
				pushBomb(start, vec2d(cos(a), sin(a) - .5));
				a += spread/wideMax;
			}
			break;
		}
	}
}

void pushBomb(Vector2d start, Vector2d dir) {
	int bid = getFreeId(bombs);
	if (bid >= MAX_BOMBS) return;

	Bullet b = {0};
	b.pos = start;
	b.dir = dir;
	// b.dir = normalize(diff(player.pos, start));

	bombs[bid] = b;
}

