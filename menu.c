
#include "common.h"
#include "game.h"
#include "font.h"
#include "renderer.h"

#include "menu.h"

char title[] = "adapbdap";

extern float ship_ply[];
extern float ship_normals_ply[];
extern int ship_ply_len;

static float spinTime = -8;

static Box newGameButton = { 32, 89, 225, 145 };
static Box scoresButton = { 32, 159, 225, 215 };
static Box backButton = { 117, 416, 305, 469 };

enum MenuFrame {
	MAIN_FRAME,
	SCORE_FRAME,
};

static float scoreTime = 0;
static enum MenuFrame currentFrame = MAIN_FRAME;

void renderMenu(float dt) {
	spinTime += dt;

	if (currentFrame == SCORE_FRAME) {
		scoreTime+= dt*6;
		if (scoreTime > 1.0) scoreTime = 1.0;
	} else {
		scoreTime-= dt*6;
		if (scoreTime < 0.0) scoreTime = 0.0;
	}

	view3d();
	glLoadIdentity();


	glTranslatef(-scoreTime*5, 0,0);

	glTranslatef(1,-1.5,-5);
	glRotatef(-65, 1,0,0);
	glRotatef(spinTime*20, 0,0,1);
	glTranslatef(.6,0,0);
	renderMesh(ship_ply, ship_normals_ply, ship_ply_len/3);

	view2d();
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(-scoreTime*224, 0,0);

	bind_image(buttons);
	draw_image(buttons, 6, 50);

	
	glColor4f(1,1,1, .4);
	text(font, 26, 220, "leaf corcoran 2010");


	glScalef(2,2,0);
	glColor4f(1,1,.8, 1);
	text(font, 19,50, "new game");
	text(font, 19,85, "scores");



	glColor4f(1,1,1,1);
	glScalef(2,2,0);
	text(font, 0,4, title);

	glPopMatrix();

	if (scoreTime > 0.0) {
		glPushMatrix();	
		glTranslatef(480*(1-scoreTime), 0,0);

		bind_image(buttons);
		draw_image(buttons, 90, 375);

		static char buff[100];
		
		glScalef(2,2,0);
		text(font, 8,4, "scores");

		glColor4f(1,.8,.9, 1);
		int i;
		for (i = 0; i < SCORE_COUNT; i++) {
			sprintf(buff, "%d", scores[i]);
			text(font, 30,20+18*i, buff);
		}

		glColor4f(1,1,.8, 1);
		text(font, 70, 213, "back");
		glColor4f(1,1,1, 1);

		glPopMatrix();	
	}


}

void onClick(Vector2d point) {
	switch(currentFrame) {	
		case MAIN_FRAME:
			if (pointInBox(point, newGameButton)) {
				initGame();
				gameState = GAME_STATE;	
			} else if(pointInBox(point, scoresButton)) {
				getScores(scores, SCORE_COUNT);
				/*
				insertScore(332);
				insertScore(1232);
				insertScore(783);
				*/

				currentFrame = SCORE_FRAME;	
			}
			break;
		case SCORE_FRAME:
			if (pointInBox(point, backButton)) {
				currentFrame = MAIN_FRAME;
			}
			break;
	} 
}

int insertScore(int val) {
	getScores(scores, SCORE_COUNT);

	int i;
	int last = -1;
	int rank = -1;
	for (i = 0; i < SCORE_COUNT; i++) {
		if (last == -1) {
			if (val > scores[i]) {
				last = scores[i];
				scores[i] = val;
				rank = i+1;
			}
		} else {
			int tmp = scores[i];
			scores[i] = last;
			last = tmp;	
		}
	}

	// save it
	saveScores(scores, SCORE_COUNT);

	return rank;
}

