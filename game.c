
#include "bullet.ply.h"
#include "ship.ply.h"

#include "renderer.h"
#include "timer.h"
#include "image.h"
#include "font.h"

#include "game.h"
#include "menu.h"
#include "enemies.h"
#include "floor.h"
#include "animate.h"

#include "common.h"

enum GameState gameState = MENU_STATE;
int fireTimer = -1;

Image logo, font, explosion;
char status[80];
float depth = -20; // the depth of the playing field

int bulletsCreated = 0;

void moveStart(int x, int y) {
	Vector2d target = project(vec2d(x, y));

	player.posOffset = diff(target, player.pos);
}

void moveDrag(int x, int y) {
	Vector2d target = project(vec2d(x, y));

	// player.dp = diff(target, player.pos);	
	player.pos = diff(target, player.posOffset);
}

void moveEnd(int x, int y) {
	switch(gameState) {
		case MENU_STATE:
			onClick(vec2d(x,y));
		case GAME_STATE:
			if (player.deathTime > .9) {
				gameState = MENU_STATE;	
			}
	}
}

void renderAll(float dt) {
	switch(gameState) {
		case MENU_STATE: 
			renderMenu(dt);
			break;
		case GAME_STATE:
			updateState(dt);
			renderGame(dt);
			break;
	}
}

int fireBulletEvent(void *t) {
	float xdiff = 0;
	// float xdiff = .05 - .1 * (float)(rand() % 1000) / 1000.0;
	// printf("ydiff: %f\n", xdiff);

	pushBullet(player.pos, normalize(vec2d(xdiff,1)));
	return 0;		
}

void loadResources() {
	mt_init();

	// load resources
	printf("loading resources...\n");
	buttons = load_image("buttons.png");
	font = load_image("font.png");
	tick = load_image("life.png");

	explosion = load_image("explosion.png");

}

void initGame() {
	// init game state
	player.pos.x = 0;
	player.pos.y = -6;

	player.health = 3;
	player.lastKill = 0;
	player.lastHit = -1;
	player.deathTime = -1;

	player.scoreSaved = 0;

	viewport.depth = -20;
	viewport.width = 11.1;
	viewport.height = 16.1;

	player.score = 0;
	player.displayedScore = 0;

	player.bulletCount = 0; 
	player.deadCount = 0; // dead bullets

	clearBullets();
	clearTimers();
	clearEnemies();
	clearBombs();

	fireTimer = createTimer(.3, fireBulletEvent);
	pushWave(0);
}

// update the game statte
void updateState(float dt) {
	updateTimers(dt);
	updateEnemies(dt);
	updateBullets(dt);
	updateBombs(dt);

	// move ship towards drag point
	player.posOffset = scale(player.posOffset, .9);

	player.lastKill += dt;

	if (player.displayedScore < player.score) {
		int dif = player.score - player.displayedScore;
		int up = dt*dif;
		if (up < 3) up = 3;
		player.displayedScore += up;
		if (player.displayedScore > player.score)
			player.displayedScore = player.score;
	}

}

// render the game
float shipTime = 0;
void renderGame(float dt) {
	view3d();
	glTranslatef(0,0, depth);
	
	shipTime += dt;

	renderFloor(dt);
	
	// place ship
	
	if (player.health > 0) {
		glColor4f(.9,.9,.9, 1);
		glPushMatrix();
		glTranslatef(player.pos.x,player.pos.y, 0);

		glRotatef(-45 + sin(shipTime*2)*5, 0,1,0);

		glRotatef(90, 0,0,1);
		renderMesh(ship_ply, ship_normals_ply, ship_ply_len/3);
		
		glPopMatrix();
	}

	renderBullets();
	renderEnemies();
	renderBombs();

	viewSprite();
	bind_image(explosion);
	renderAnimations(dt);

	// viewDebug();
	// renderEnemyHitbox();
	// renderBulletHitbox();
	// rect(player.pos.x, player.pos.y, 0, 0);

	if (player.lastHit >= 0 && player.deathTime != 1.0) {
		viewOverlay();
		glLoadIdentity();
		glColor4f(1,.1,.1, 1.0 - player.lastHit);
		image_quad();
		player.lastHit += dt*8;
		if (player.lastHit > 1.0) player.lastHit = -1;
	}

	view2d();
	glColor4f(1,1,1,1);
	glLoadIdentity();

	// draw live
	bind_image(tick);
	int i;
	for (i = 0; i < player.health; i++)
		draw_image(tick, i*16,0);

	// write hud
	glPushMatrix();
	glTranslatef(0,viewport.screenHeight,0);
	glRotatef(-90, 0,0,1);

	text(font, 0,0, "score");
	glScalef(2,2,1);
	sprintf(status, "%d", player.displayedScore,
			player.pos.x, player.pos.y);

	text(font, 18, 0, status);
	glPopMatrix();


	if (player.deathTime >= 0) {
		viewOverlay();
		glLoadIdentity();


		if (player.deathTime > 1.0) player.deathTime = 1.0;

		glColor4f(0,0,0, player.deathTime);
		image_quad();
		player.deathTime += dt;


		if (player.deathTime > 0.9) {
			view2d();
			glColor4f(1,1,1,1);
			glScalef(2,2,2);
			text(font, 0,0, "You have died");

			sprintf(status, "Your score was %d", player.score);
			text(font, 0,18, status);

			if (!player.scoreSaved) {
				player.scoreSaved = insertScore(player.score);	
			}

			if (player.scoreSaved > 0) {
				sprintf(status, "Your rank is %d", player.scoreSaved);
				text(font, 0,36, status);
			}
		}

	}

}

void pushBullet(Vector2d pos, Vector2d dir) {
	int i = player.bulletCount;
	if (i >= MAX_BULLETS) {
		//printf("max bullets!\n");
		return;
	}

	if (player.deadCount) {
		// printf("pulling dead at %d\n", player.deadCount-1);
		i = player.dead[player.deadCount-1];
		player.deadCount--;
	} else {
		player.bulletCount++;
	}

	
	static bool odd = true;
	odd = !odd;

	bulletsCreated++;
	player.bullets[i].dead = 0;
	player.bullets[i].pos = sum(pos, vec2d(0,.5));
	player.bullets[i].dir = vec2d(dir.x + player.dp.x/8.0, dir.y);
}

Box playerHitbox() {
	Box out = {
		player.pos.x-.4,
		player.pos.y+.5,

		player.pos.x+.4,
		player.pos.y-1,
	};
	return out;
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

		bullets[b].pos.x += bullets[b].dir.x * dt * 40;
		bullets[b].pos.y += bullets[b].dir.y * dt * 40;
	}
}

// remove all bullets
void clearBullets() {
	int b;
	Bullet * const bullets = player.bullets;
	for (b = 0; b < player.bulletCount; b++) {
		if (bullets[b].dead) continue;
		killBullet(b);
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

		glScalef(.5,.5,.5);
		glRotatef(-90, 1,0,0);
		renderMesh(bullet_ply, bullet_normals_ply, bullet_ply_len/3);
		glPopMatrix();

	}

	glColor4f(1, 1, 1, 1);
}

void onPlayerHit() {
	player.lastHit = 0;
	player.health--;

	if (player.health == 0) {
		pushAnimation(player.pos, 0);
		removeTimer(fireTimer);
		player.deathTime = 0;
	}
}

void onKillEnemy(int type) {
	float mult = (1.0 - player.lastKill) * 3;
	mult = (mult < 0 ? 0 : mult) + 1;
	int points = mult * pow(type + 1, 2);
	points *= 19;
	player.score += points;

	player.lastKill = 0;
}

