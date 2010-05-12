
// shmup
// leaf corcoran
// 2010

#ifndef COMMON_H
#define COMMON_H


#ifdef IPHONE
#include <OpenGLES/ES1/gl.h>
#else
#include <GL/glfw.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rand.h"

// create a new list with dead stack
#define defList(type, name, max) \
	int name##_count = 0; \
	type name[max]; \
	int dead_##name##_count = 0; \
	int dead_##name##_list[max]
	

// get a free id from the list/dead list
#define getFreeId(name) \
	dead_##name##_count ? \
	(dead_##name##_count--, dead_##name##_list[dead_##name##_count]) : \
	name##_count++

// mark something as dead
#define killId(name, id) \
	dead_##name##_list[dead_##name##_count++] = id

#endif

