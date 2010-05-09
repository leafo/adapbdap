#include <stdlib.h>
#include <GL/glpng.h>

#include "image.h"

static float quad_tex[] = {
	0,0, 1,0, 0,1, 1,1 
};


// make a quad in a triangle array
void make_quad(float *where, float left, float top, float right, float bottom) {
}


Image load_image(const char *fname) {
	pngInfo info;
	GLuint id = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA,
			&info, GL_CLAMP, GL_NEAREST, GL_NEAREST);

	Image i = { id, info.Width, info.Height };
	return i;
}

void draw_image(const Image i, float x, float y) {
	float pos[] = {
		x,y,
		x+i.width,y,
		x,y+i.width,
		x+i.width,y+i.width
	};

	glVertexPointer(2, GL_FLOAT, 0, pos);	
	glTexCoordPointer(2, GL_FLOAT, 0, quad_tex);	

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void bind_image(const Image i) {
	glBindTexture(GL_TEXTURE_2D, i.texid);
}

void blit_image(const Image i) {

}

