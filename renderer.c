
#include "renderer.h"

#include "common.h"

// light properties
GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
GLfloat diffuse[] = {0.7, 0.7, 0.7, 1.0};
GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat position[] = {0.0, 0.0, 0.0, 1.0};

// set the perspective
void perspective(float fov) {
	float zNear = .1;
	float zFar = 100;

	float ratio =  (float)viewport.screenWidth/viewport.screenHeight;

	// adapted from gluPerspective
	float f = zNear * tanf(fov * 3.1415 / 180.0 / 2); // frustum size
#ifdef IPHONE
	glFrustumf(-f, f, -f/ratio, f/ratio, zNear, zFar);
#else
	glFrustum(-f, f, -f/ratio, f/ratio, zNear, zFar);
#endif

}

void placeLights() {
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}

// go from screen space to world space at depth
Vector2d project(Vector2d pos) {
	Vector2d out;

	out.x = viewport.width*2*pos.x/viewport.screenWidth - viewport.width;
	out.y = -viewport.height*2*pos.y/viewport.screenHeight + viewport.height;

	return out;
}

void setupView(int width, int height) {

	glViewport(0,0, width, height);

	viewport.screenWidth = width;
	viewport.screenHeight = height;

	glClearColor(0.1, 0.1, 0.1, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_FLAT);

	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glEnable(GL_COLOR_MATERIAL);
}


void renderMesh(float *verts, float *norms, int count) {
	glVertexPointer(3, GL_FLOAT, 0, verts);	
	glNormalPointer(GL_FLOAT, 0, norms);	

	glDrawArrays(GL_TRIANGLES, 0, count);
}

// set up the view for 3d mode
void view3d() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60, (float)viewport.screenWidth/viewport.screenHeight, 0.1, 100);
	perspective(60);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	placeLights();
}

// set up view for 2d mode
void view2d() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef IPHONE
	glOrthof(0, viewport.screenWidth, viewport.screenHeight, 0, -1, 1);
#else
	glOrtho(0, viewport.screenWidth, viewport.screenHeight, 0, -1, 1);
#endif

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);
}


// render 2d stuff in the game plane
void viewSprite() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspective(60);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // ~
	glTranslatef(0,0, -20);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);
}


void viewOverlay() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef IPHONE
	glOrthof(-1, 1, -1, 1, -1, 1);
#else
	glOrtho(-1, 1, -1, 1, -1, 1);
#endif

	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glEnableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


void viewDebug() {
	viewSprite();
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void rect(float x1, float y1, float x2, float y2) {
	glColor4f(.1,1,1, .5);

	float verts[] = { x1,y1, x2,y1, x1,y2, x2,y2 };

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glColor4f(1,1,1, 1);
}



