//Jeff Chastine
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include "header\ParticleSystem.h"
#include "3rdpartylib\trackball.h"

using namespace std;

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720
#define BOX_SIZE_X 20.0
#define BOX_SIZE_Y 10.0
#define BOX_SIZE_Z 10.0

unique_ptr<ParticleSystem> pSystem;
TrackBall tb(GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON);

void reshape(int w, int h)
{
	tb.reshape(w, h);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, float(w) / float(h), 0.001, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//move the camera back
	glTranslatef(0.0f, 0.0f, -30.0f);
}

void RenderParticleSystem() {
	glPointSize(5.0f);
	glColor3f(0.2f, 0.2f, 1.0f);

	for (int i = 0; i < pSystem->getParticles().size(); ++i)
	{
		glBegin(GL_POINTS);
		glVertex3d(pSystem->getParticles()[i]->position.x,
			pSystem->getParticles()[i]->position.y,
			pSystem->getParticles()[i]->position.z);
		glEnd();
	}
}

//
void CreateParticleSystem() {
	int numX = 10;
	int numY = 10;
	int numZ = 1;
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

	glVertex3f(-boxX, -boxY, boxZ);
	glVertex3f(-boxX, -boxY, -boxZ);
	glVertex3f(-boxX, boxY, boxZ);
	glVertex3f(-boxX, boxY, -boxZ);

	glVertex3f(boxX, -boxY, boxZ);
	glVertex3f(boxX, -boxY, -boxZ);
	glVertex3f(boxX, boxY, boxZ);
	glVertex3f(boxX, boxY, -boxZ);
	glEnd();
}

void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -30.0f);
	
	glPushMatrix();

	tb.applyTransform();
	//render
	drawContainer(BOX_SIZE_X / 2, BOX_SIZE_Y / 2, BOX_SIZE_Z / 2);
	glEnable(GL_POINT_SMOOTH);
	RenderParticleSystem();

	glPopMatrix();

	glutSwapBuffers();
}

static void myMouse(int button, int state, int x, int y)
{
	tb.mouse(button, state, x, y);
	glutPostRedisplay();
}

static void myMotion(int x, int y)
{
	tb.motion(x, y);
	glutPostRedisplay();
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

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}


	glutMainLoop();
	return 0;
}