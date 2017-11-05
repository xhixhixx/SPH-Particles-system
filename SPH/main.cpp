#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "header/ParticleSystem.h"
#include "header/Constant.h"

unique_ptr<ParticleSystem> pSystem;

GLuint p;

// angle of rotation for the camera direction
double angle = 0.0;
// actual vector representing the camera's direction
double lx = 0.0, lz = -1.0;
// XZ position of the camera
double x = 0.0, z = 0.0;
// the key states. These variables will be zero
//when no key is being presses
double deltaAngle = 0.0;
double deltaMove = 0;
int xOrigin = -1;

//
int particleIdInFocus = PARTICLE_IN_FOCUS;

#define CAMERA_MOVEBACK_X 0.0f
#define CAMERA_MOVEBACK_Y 1.0f
#define CAMERA_MOVEBACK_Z -2.0f

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, double(w) / double(h), 0.001, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//move the camera back
	glTranslatef(CAMERA_MOVEBACK_X, CAMERA_MOVEBACK_Y, CAMERA_MOVEBACK_Z);
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

void computePos(double deltaMove) {

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
	glTranslatef(CAMERA_MOVEBACK_X, CAMERA_MOVEBACK_Y, CAMERA_MOVEBACK_Z);
	gluLookAt(x, 1.0f, z,
		x + lx, 1.0f, z + lz,
		0.0f, 1.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	glPushMatrix();
	//render
	drawContainer(OFFSET_X, OFFSET_Y, OFFSET_Z);
	glEnable(GL_POINT_SMOOTH);
	glUseProgram(p);
	RenderParticleSystem();
	glUseProgram(0);
	//update Particle System for the next timestep
	pSystem->update();

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
		deltaAngle = (x - xOrigin) * 0.004f;

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

char *textFileRead(char *fn) {

	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void setShaders() {

	GLuint v, f;
	char *vs, *fs;

	// Create shader handlers
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from files
	vs = textFileRead("particle.vert");
	fs = textFileRead("particle.frag");

	const char * vv = vs;
	const char * ff = fs;

	// Set shader source
	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	// Compile all shaders
	glCompileShader(v);
	glCompileShader(f);

	// Create the program
	p = glCreateProgram();

	// Attach shaders to program
	glAttachShader(p, v);
	glAttachShader(p, f);

	// Link and set program to use
	glLinkProgram(p);
	glUseProgram(p);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//Create window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("SPH");
	glewInit();
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	setShaders();
	//create particle system
	CreateParticleSystem();
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