
#ifndef GEOMETRY_H
#define GEOMETRY_H

#define bool int
#define true 1
#define false 0

typedef struct {
	float x, y;
} Vector2d;

Vector2d vec2d(float x, float y);
Vector2d normalize(Vector2d v);
Vector2d scale(Vector2d v, float x);

#endif
