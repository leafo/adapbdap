
#include "bullet.ply.h"
#include "ship.ply.h"

#include "renderer.h"
#include "timer.h"
#include "image.h"
#include "font.h"

#include "game.h"
#include "enemies.h"
#include "floor.h"

#include "common.h"

Image logo, font;
char status[80];
float depth = -20; // the depth of the playing field

int bulletsCreated = 0;

int fireBulletEvent(void *t) {
	float xdiff = 0;
	// float xdiff = .05 - .1 * (float)(rand() % 1000) / 1000.0;
	// printf("ydiff: %f\n", xdiff);

	pushBullet(player.pos, normalize(vec2d(xdiff,1)));
	return 0;		
}

void initGame() {
	mt_init();

	// load resources
	printf("loading resources...\n");
	logo = load_image("logo.png");
	font = load_image("font.png");

	// init game state
	player.pos.x = 0;
	player.pos.y = 0;

	player.lastKill = 0;

	viewport.depth = -20;
	viewport.width = 11.1;
	viewport.height = 16.1;


	player.bulletCount = 0; 
	player.deadCount = 0; // dead bullets

	createTimer(.3, fireBulletEvent);
	pushWave(.5, 0);
}

// update the game statte
void updateState(float dt) {
	updateTimers(dt);
	updateEnemies(dt);
	updateBullets(dt);

	player.lastKill += dt;
}

// render the game
float shipTime = 0;
void renderGame(float dt) {
	view3d();
	glTranslatef(0,0, depth);
	
	shipTime +=dt;

	renderFloor(dt);
	
	// place ship
	glColor4f(.9,.9,.9, 1);
	glPushMatrix();
	glTranslatef(player.pos.x,player.pos.y, 0);

	glRotatef(-45 + sin(shipTime*2)*5, 0,1,0);



	glRotatef(90, 0,0,1);
	renderMesh(ship_ply, ship_normals_ply, ship_ply_len/3);
	
	glPopMatrix();

	renderBullets();
	renderEnemies();



	viewDebug();
	// renderEnemyHitbox();
	// renderBulletHitbox();
	// rect(player.pos.x, player.pos.y, 0, 0);

	view2d();
	glColor4f(1,1,1,1);
	glLoadIdentity();

	// write hud
	glPushMatrix();
	glTranslatef(0,viewport.screenHeight,0);
	glRotatef(-90, 0,0,1);

	text(font, 0,0, "score");
	glScalef(2,2,1);
	sprintf(status, "%d", player.score,
			player.pos.x, player.pos.y);

	text(font, 18, 0, status);
	glPopMatrix();

}

void pushBullet(Vector2d pos, Vector2d dir) {
	int i = player.bulletCount;
	if (i >= MAX_BULLETS) {
		// printf("max bullets!\n");
		return;
	}

	if (player.deadCount) {
		// printf("pulling dead at %d\n", player.deadCount-1);
		i = player.dead[player.deadCount-1];
		player.deadCount--;
	} else {
		player.bulletCount++;
	}

	// printf("bullet count: %d\n", player.bulletCount);
	
	static bool odd = true;
	odd = !odd;

	bulletsCreated++;
	player.bullets[i].dead = 0;
	player.bullets[i].pos = sum(pos, vec2d(odd ? 1 : -.5,.5));
	player.bullets[i].dir = vec2d(dir.x + player.dp.x/2.0, dir.y);
}

Box	bulletHitbox(Bullet b) {
	Box out = {
		b.pos.x-.3,
		b.pos.y+1.6,

		b.pos.x+.3,
		b.pos.y,
	};
	return out;
}

void renderBulletHitbox() {
	int i;
	for (i = 0; i < player.bulletCount; i++) {
		if (player.bullets[i].dead) continue;

		Box b = bulletHitbox(player.bullets[i]);
		rect(b.x1, b.y1, b.x2, b.y2);
	}
}

void killBullet(int i) {
	player.bullets[i].dead = 1;
	player.dead[player.deadCount++] = i;
}


void updateBullets(float dt) {
	int b;
	Bullet * const bullets = player.bullets;
	for (b = 0; b < player.bulletCount; b++) {
		if (bullets[b].dead) continue;

		// bullet out of bounds?
		if (bullets[b].pos.y > viewport.height+2) {
			killBullet(b);
			continue;
		}
		// collision handled in update enemies

		bullets[b].pos.x += bullets[b].dir.x * dt * 30;
		bullets[b].pos.y += bullets[b].dir.y * dt * 30;
	}
}

void renderBullets() {
	glColor4f(1, .796, .341, 1);
	int i;
	Bullet * const bullets = player.bullets;
	for (i = 0; i < player.bulletCount; i++) {
		if (bullets[i].dead) continue;

		glPushMatrix();
		glTranslatef(bullets[i].pos.x, bullets[i].pos.y, 0);

		glRotatef(bullets[i].pos.y*10, 0,1,0);

		glScalef(.5,1, .5);
		glRotatef(-90, 1,0,0);
		renderMesh(bullet_ply, bullet_normals_ply, bullet_ply_len/3);
		glPopMatrix();

	}

	glColor4f(1, 1, 1, 1);
}


void onKillEnemy(int type) {
	float mult = (1.0 - player.lastKill) * 3;
	mult = (mult < 0 ? 0 : mult) + 1;
	int points = mult * (type + 1);
	printf("got points: %d\n", points);
	player.score += points;

	player.lastKill = 0;
}

