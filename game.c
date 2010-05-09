
#include <stdio.h>

#include "bullet.ply.h"
#include "ship.ply.h"

#include "renderer.h"
#include "timer.h"
#include "image.h"
#include "font.h"

#include "game.h"

Image logo, font;
char status[80];
float depth = -20; // the depth of the playing field

int bulletsCreated = 0;

int fireBulletEvent(float dt) {
	float xdiff = .1 - .2 * (float)(rand() % 1000) / 1000.0;
	printf("ydiff: %f\n", xdiff);

	pushBullet(player.pos, normalize(vec2d(xdiff,1)));
	return 0;		
}

void initGame() {
	// load resources
	printf("loading resources...\n");
	logo = load_image("logo.png");
	font = load_image("font.png");

	// init game state
	player.pos.x = 0;
	player.pos.y = 0;

	viewport.depth = -20;
	viewport.width = 7.6;
	viewport.height = 11.7;


	player.bulletCount = 0; 
	player.deadCount = 0; // dead bullets

	int t = createTimer(.1, fireBulletEvent);
}

// update the game statte
void updateState(float dt) {
	updateTimers(dt);
}

// render the game
void renderGame(float dt) {
	view3d();
	glTranslatef(0,0, depth);
	
	// place ship
	glPushMatrix();
	glTranslatef(player.pos.x,player.pos.y, 0);

	glRotatef(-45, 0,1,0);
	glRotatef(90, 0,0,1);
	renderMesh(ship_ply, ship_normals_ply, ship_ply_len/3);
	
	glPopMatrix();


	viewDebug();
	// rect(player.pos.x, player.pos.y, 0, 0);
	renderBullets(dt);

	view2d();
	glLoadIdentity();

	// write hud
	glPushMatrix();
	glTranslatef(0,viewport.screenHeight,0);
	glRotatef(-90, 0,0,1);

	glScalef(2,2,1);
	sprintf(status, "c: %d l: %f, %f", bulletsCreated, player.pos.x, player.pos.y);

	text(font, 0, 0, status);
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

	bulletsCreated++;
	player.bullets[i].dead = 0;
	player.bullets[i].pos = pos;
	player.bullets[i].dir = dir;
}

void renderBullets(float dt) {
	int i;
	
	for (i = 0; i < player.bulletCount; i++) {
		if (player.bullets[i].dead) continue;

		if (player.bullets[i].pos.y > 17.0) {
			// printf("killing bullet\n");
			player.bullets[i].dead = 1;
			player.dead[player.deadCount++] = i;
			continue;
		}

		/*
		glPushMatrix();
		glTranslatef(player.bullets[i].pos.x, player.bullets[i].pos.y, 0);


		renderMesh(bullet_ply, bullet_normals_ply, bullet_ply_len/3);
		glPopMatrix();
		*/

		float x = player.bullets[i].pos.x;
		float y = player.bullets[i].pos.y;
		rect(x-.25,y-.25, x+.25, y+.25);


		player.bullets[i].pos.x += player.bullets[i].dir.x * dt * 30;
		player.bullets[i].pos.y += player.bullets[i].dir.y * dt * 30;
	}
}



