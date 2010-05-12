
#ifndef GEOMETRY_H
#define GEOMETRY_H

#define bool int
#define true 1
#define false 0

typedef struct {
	float x, y;
} Vector2d;

typedef struct {
	float x1, y1, x2, y2;
} Box;

Vector2d vec2d(float x, float y);
Vector2d normalize(Vector2d v);
Vector2d scale(Vector2d v, float x);
Vector2d diff(Vector2d a, Vector2d b);
Vector2d sum(Vector2d a, Vector2d b);

bool pointInBox(Vector2d p, Box b);
bool boxInBox(Box a, Box b);

#endif
