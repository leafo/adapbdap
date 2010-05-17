
#include "common.h"
#include "game.h"

#include "animate.h"

#define MAX_ANIMS 20

defList(Animation, anims, MAX_ANIMS);

static float duration = .3;

// render single anim
// return 1 on done
static int renderSingle(Animation a) {
	if (a.time > duration) return 1;

	glPushMatrix();
	glTranslatef(a.origin.x, a.origin.y-1, 0);
	float t = a.time/duration;
	glScalef(t*4, t*4, 1);
	// glRotatef(60*t, 0,0,1);
	glRotatef(180, 0,0,1);
	glColor4f(1,1,1, 1.0 - t);
	image_quad();
	glPopMatrix();

	return 0;
}

void renderAnimations(float dt) {
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	int i = 0;
	for (i = 0; i < anims_count; i++) {
		if (anims[i].dead) continue;

		anims[i].time += dt;
		if (renderSingle(anims[i])) { // if done
			anims[i].dead = 1;
			killId(anims, i);
		}
	}
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
}

void pushAnimation(Vector2d origin, int type) {
	int aid = getFreeId(anims);
	if (aid >= MAX_ANIMS) return;

	Animation a = { origin, 0.0, 0 };
	anims[aid] = a;
}


