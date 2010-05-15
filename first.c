
#include <stdlib.h>
#include <stdio.h>

#include <GL/glfw.h>
#include <GL/glpng.h>

#include "cube.ply.h"
#include "hull.ply.h"

#include "geometry.h"
#include "renderer.h"
#include "image.h"
#include "font.h"

#include "game.h"

#define WIDTH 320
#define HEIGHT 480


bool dragging = false;


Image imageBind(const char *fname) {
	pngInfo info;
	GLuint id = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA,
			&info, GL_CLAMP, GL_NEAREST, GL_NEAREST);

	Image i = { id, info.Width, info.Height };
	return i;
}

void gameLoop(float dt) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateState(dt);
	renderGame(dt);

	glfwSwapBuffers();
	glfwSleep(0.01);
}


int main() {
	glfwInit();

	bool ok = glfwOpenWindow(WIDTH, HEIGHT,
		8,8,8,8,
		24,0,
		GLFW_WINDOW
	);

	if (!ok) {
		glfwTerminate();
		printf("failed to open window\n");
		exit(1);
	}

	glfwSetWindowTitle("game test");
	glfwEnable(GLFW_STICKY_KEYS);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float delta, last;

	initGame();

	int width, height;
	glfwGetWindowSize(&width, &height);

	setupView(width, height);
	while (glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC)) {
		float c = glfwGetTime();
		delta = c - last;	
		last = c;

		// check for input
		int mx, my;
		glfwGetMousePos(&mx, &my);
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			if (dragging == false) {
				dragging = true;
				moveStart(mx, my);
			} else {
				moveDrag(mx,my);
			}
		} else {
			if (dragging) moveEnd(mx, my);
			dragging = false;
		}

		gameLoop(delta);
	}

}
