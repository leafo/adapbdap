
#include <math.h>
#include <stdio.h>

#include "geometry.h"

float length(Vector2d v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

Vector2d vec2d(float x, float y) {
	Vector2d out = {x, y};
	return out;
}

Vector2d normalize(Vector2d v) {
	float len = length(v);
	if (len == 0) {
		Vector2d out = {0};
		return out;
	}
	Vector2d out = { v.x/len, v.y/len };
	return out;
}

Vector2d scale(Vector2d v, float x) {
	Vector2d out = { x*v.x, x*v.y };
	return out;
}

Vector2d diff(Vector2d a, Vector2d b) {
	return vec2d(a.x - b.x, a.y - b.y);
}

Vector2d sum(Vector2d a, Vector2d b) {
	return vec2d(a.x + b.x, a.y + b.y);
}


bool pointInBox(Vector2d p, Box b) {
	if (p.x < b.x1 || p.x > b.x2 || p.y > b.y1 || p.y < b.y2) return false; 
	return true;
}

bool boxInBox(Box a, Box b) {
	if (a.x2 < b.x1 || a.x1 > b.x2 || a.y2 > b.y1 || a.y1 < b.y2) return false;
	return true;
}



