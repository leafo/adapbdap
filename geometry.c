
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
