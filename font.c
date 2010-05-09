
#include <stdio.h>
#include <string.h>

#include "font.h"

#define NUM_LETTER_X 16
#define NUM_LETTER_Y 16

// actual letter width
#define REAL_WIDTH 7 

typedef struct {
	float x1,y1, x2,y2, x3,y3;
} Triangle2d;


// set a quod in a triangle array
static void quod_set(Triangle2d *tri,
		float x1, float y1, float x2, float y2)
{
	Triangle2d *left = tri;
	Triangle2d *right = tri+1;

	left->x1 = x1;
	left->y1 = y1;

	left->x2 = right->x2 = x2;
	left->y2 = right->y2 = y1;

	left->x3 = right->x1 = x1;
	left->y3 = right->y1 = y2;

	right->x3 = x2;
	right->y3 = y2;
}

// render some text on the screen
void text(Image i, float origin_x, float origin_y, const char *string) {
	int cwidth = i.width / NUM_LETTER_X;
	int cheight = i.height / NUM_LETTER_Y;

	float ss = (float)cwidth/i.width;
	float tt = (float)cheight/i.height;

	int len = strlen(string);
	int num_triangles = 2*len;

	Triangle2d verts[num_triangles];
	Triangle2d texcoords[num_triangles];

	int k;
	Triangle2d *current_tri = verts;
	Triangle2d *current_tex = texcoords;
	for (k = 0; k < len; k++) {
		char c = string[k];
		// printf("%c (%f, %f)\n", c,s,t);

		// find tex coords
		int lx = cwidth * (c % NUM_LETTER_X);
		int ly = cwidth * (c / NUM_LETTER_X);
		float s = (float)lx/i.width;
		float t = (float)ly/i.height;

		quod_set(current_tri, 
			origin_x, origin_y,
			origin_x + 16, origin_y + 16
		);

		quod_set(current_tex, s,t, s+ss, t+tt);

		// move 2 triangles forward
		current_tri+=2;
		current_tex+=2;

		origin_x += REAL_WIDTH;
	}

	// draw it
	bind_image(i);

	glVertexPointer(2, GL_FLOAT, 0, verts);	
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);	
	glDrawArrays(GL_TRIANGLES, 0, num_triangles*3);
}

