
#ifndef ANIMATE_H
#define ANIMATE_H

#include "geometry.h"

typedef struct {
	Vector2d origin;
	Vector2d dir;
	float time;
	int dead;
} Animation;

void pushAnimation(Vector2d origin, int type);
void renderAnimations(float dt);

#endif
