#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include "header\ParticleSystem.h"
#include "header\Constant.h"

unique_ptr<ParticleSystem> pSystem;

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 0.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;

//
int particleIdInFocus = 1;

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, float(w) / float(h), 0.001, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//move the camera back
	glTranslatef(0.0f, 0.0f, -30.0f);
}

void SetNormalParticleColor() {
	glColor3f(0.2f, 0.2f, 1.0f);
}

void SetParticleColorByCell(double r, double g, double b) {
	glColor3d(r, g, b);
}

void RenderParticleSystem() {
	glPointSize(PARTICLE_SIZE);
	SetNormalParticleColor();
	vector<shared_ptr<Particle>>& pList = pSystem->getParticles();

	int focusParticleId = particleIdInFocus;

	for (unsigned int i = 0; i < pList.size(); ++i)
	{
		//render each cell in a different color
		//SetParticleColorByCell(0.1f * pList[i]->cellPosition.x, 0.1f * pList[i]->cellPosition.y, 0.1f * pList[i]->cellPosition.z);

		//
		if (pList[i]->id == focusParticleId) {
			glColor3d(1.0, 0.0, 0.0);
		}
		else if (pSystem->checkIsNeighbor(focusParticleId, pList[i]->id)) {
			glColor3d(0.0, 1.0, 0.0);
		}
		else {
			SetNormalParticleColor();
		}

		glBegin(GL_POINTS);
		glVertex3d(pList[i]->position.x,
				pList[i]->position.y,
				pList[i]->position.z);
		glEnd();
	}
}

//
void CreateParticleSystem() {
	int numX = NUM_PARTICLE_X;
	int numY = NUM_PARTICLE_Y;
	int numZ = NUM_PARTICLE_Z;
	pSystem = make_unique<ParticleSystem>(numX, numY, numZ);
	pSystem->createParticleSystem();
}

//create a container of -10 10 in x, -5, 5 in y, -5,5 in z
void drawContainer(double boxX, double boxY, double boxZ) {
	glLineWidth(2.0f); 
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);
	glVertex3d(-boxX, boxY, boxZ);
	glVertex3d(boxX, boxY, boxZ);
	glVertex3d(-boxX, -boxY, boxZ);
	glVertex3d(-boxX, boxY, boxZ);
	glVertex3d(boxX, -boxY, boxZ);
	glVertex3d(boxX, boxY, boxZ);
	glVertex3d(-boxX, -boxY, boxZ);
	glVertex3d(boxX, -boxY, boxZ);

	glVertex3d(-boxX, boxY, -boxZ);
	glVertex3d(boxX, boxY, -boxZ);
	glVertex3d(-boxX, -boxY, -boxZ);
	glVertex3d(-boxX, boxY, -boxZ);
	glVertex3d(boxX, -boxY, -boxZ);
	glVertex3d(boxX, boxY, -boxZ);
	glVertex3d(-boxX, -boxY, -boxZ);
	glVertex3d(boxX, -boxY, -boxZ);

	glVertex3d(-boxX, -boxY, boxZ);
	glVertex3d(-boxX, -boxY, -boxZ);
	glVertex3d(-boxX, boxY, boxZ);
	glVertex3d(-boxX, boxY, -boxZ);

	glVertex3d(boxX, -boxY, boxZ);
	glVertex3d(boxX, -boxY, -boxZ);
	glVertex3d(boxX, boxY, boxZ);
	glVertex3d(boxX, boxY, -boxZ);
	glEnd();
}

void computePos(float deltaMove) {

	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void display()
{
	if (deltaMove)
		computePos(deltaMove);

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	glTranslatef(0.0f, 0.0f, -30.0f);
	gluLookAt(x, 1.0f, z,
		x + lx, 1.0f, z + lz,
		0.0f, 1.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	glPushMatrix();
	//render
	drawContainer(BOX_SIZE_X / 2, BOX_SIZE_Y / 2, BOX_SIZE_Z / 2);
	glEnable(GL_POINT_SMOOTH);
	RenderParticleSystem();

	glPopMatrix();

	glutSwapBuffers();
}

static void myMouse(int button, int state, int x, int y)
{
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			angle += deltaAngle;
			xOrigin = -1;
		}
		else {// state = GLUT_DOWN
			xOrigin = x;
		}
	}
}

static void myMotion(int x, int y)
{
	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
}

void keyPressed(unsigned char key, int x, int y) {
	if (key == 'f') {
		particleIdInFocus = rand() % (NUM_PARTICLE_X * NUM_PARTICLE_Y * NUM_PARTICLE_Z);
	}
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//Create window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("SPH");
	glewInit();
	//create particle system
	CreateParticleSystem();
	//display func
	glEnable(GL_DEPTH_TEST);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	glutMouseFunc(myMouse);
	glutMotionFunc(myMotion);
	glutKeyboardFunc(keyPressed);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}


	glutMainLoop();
	return 0;
}