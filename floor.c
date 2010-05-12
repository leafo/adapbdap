
#include "floor.h"

#include "common.h"
#include "ground.ply.h"
#include "game.h"

float floorScale = 6;
float floorClock = 0; // floor position
float floorLength = 10;

float scrollSpeed = 20;

void renderFloor(float dt) {
	floorClock += dt*scrollSpeed;
	if (floorClock > floorLength*floorScale) {
		floorClock = floorClock - floorLength*floorScale;
	}

	glPushMatrix();
	glTranslatef(player.pos.x/12,-floorClock,-10);
	glScalef(floorScale,floorScale,floorScale);
	glColor4f(.3,.3,.3, 1);
	renderMesh(ground_ply, ground_normals_ply, ground_ply_len/3);


	glTranslatef(0, floorLength, 0);
	renderMesh(ground_ply, ground_normals_ply, ground_ply_len/3);

	glPopMatrix();
}


