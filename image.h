
#ifndef IMAGE_H
#define IMAGE_H

#ifdef IPHONE
#include <OpenGLES/ES1/gl.h>
#else
#include <GL/glfw.h>
#endif

typedef struct {
	GLuint texid;
	int width, height;
} Image;

Image load_image(const char *fname);
void draw_image(const Image i, float x, float y);

void bind_image(const Image i);
void blit_image(const Image i);

#endif

