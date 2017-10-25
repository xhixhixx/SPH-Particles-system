//Jeff Chastine
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

using namespace std;

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720

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

//create a container of -10 10 in x, -5, 5 in y, -5,5 in z
void drawContainer() {
	glLineWidth(2.0f); 
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);
	glVertex3f(-10.0f, 5.0f, 5.0f);
	glVertex3f(10.0f, 5.0f, 5.0f);
	glVertex3f(-10.0f, -5.0f, 5.0f);
	glVertex3f(-10.0f, 5.0f, 5.0f);
	glVertex3f(10.0f, -5.0f, 5.0f);
	glVertex3f(10.0f, 5.0f, 5.0f);
	glVertex3f(-10.0f, -5.0f, 5.0f);
	glVertex3f(10.0f, -5.0f, 5.0f);

	glVertex3f(-10.0f, 5.0f, -5.0f);
	glVertex3f(10.0f, 5.0f, -5.0f);
	glVertex3f(-10.0f, -5.0f, -5.0f);
	glVertex3f(-10.0f, 5.0f, -5.0f);
	glVertex3f(10.0f, -5.0f, -5.0f);
	glVertex3f(10.0f, 5.0f, -5.0f);
	glVertex3f(-10.0f, -5.0f, -5.0f);
	glVertex3f(10.0f, -5.0f, -5.0f);

	glVertex3f(-10.0f, -5.0f, 5.0f);
	glVertex3f(-10.0f, -5.0f, -5.0f);
	glVertex3f(-10.0f, 5.0f, 5.0f);
	glVertex3f(-10.0f, 5.0f, -5.0f);

	glVertex3f(10.0f, -5.0f, 5.0f);
	glVertex3f(10.0f, -5.0f, -5.0f);
	glVertex3f(10.0f, 5.0f, 5.0f);
	glVertex3f(10.0f, 5.0f, -5.0f);
	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	drawContainer();
	/*glBegin(GL_TRIANGLES);
		glVertex3f(0.0f, 0.0f, -0.1f);
		glVertex3f(1.0f, 0.0f, -0.1f);
		glVertex3f(0.5f, 0.5f, -0.1f);

		glVertex3f(-1.0f, -0.5f, -4.0f);
		glVertex3f(1.0f, -0.5f, -4.0f);
		glVertex3f(0.0f, 0.5f, -4.0f);
	glEnd();*/

	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//Create window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("SPH");
	glewInit();
	//display func
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	//glutIdleFunc(idle);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}


	glutMainLoop();
	return 0;
}