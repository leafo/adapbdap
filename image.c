#include <stdlib.h>

#include "image.h"

static float quad_tex[] = {
	0,0, 1,0, 0,1, 1,1 
};

extern Image imageBind(const char *fname);

Image load_image(const char *fname) {
	return imageBind(fname);
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

