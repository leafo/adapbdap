// view related fucntions

#ifndef RENDERER_H
#define RENDERER_H

#include "geometry.h"

struct {
	float depth;	
	float width, height; // the boundaries of field at depth
	int screenWidth, screenHeight;
} viewport;

void setupView(int width, int height);
void placeLights();
void view3d();
void view2d();
void viewSprite(); // the sprite layer
void viewOverlay();
void viewDebug();

void rect(float x1, float y1, float x2, float y2);

Vector2d project(Vector2d pos);
void renderMesh(float *verts, float *norms, int count);

#endif

