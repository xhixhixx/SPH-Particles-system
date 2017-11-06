#define _CRT_SECURE_NO_WARNINGS
//#include <GL/glut.h>
#include <iostream>
#include "header/ParticleSystem.h"
#include "header/Constant.h"
#include <stdio.h>
#if defined(__APPLE__)
#  define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED 1
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

#define WIN32

#include <FL/glut.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <Fl/Fl_Slider.H>
#include <Fl/Fl_Text_Display.H>
#include <string>
#include <iomanip>
#include <sstream>


#define CAMERA_MOVEBACK_X 0.0f
#define CAMERA_MOVEBACK_Y 1.1f
#define CAMERA_MOVEBACK_Z -1.6f

void idleCallback(void* pData);
unique_ptr<Params> params;
shared_ptr<ParticleSystem> pSystem;
class SPHWindow : public Fl_Glut_Window {
public:
	
	SPHWindow(int x, int y, int w, int h, char* s, const Params& _params, shared_ptr<ParticleSystem> ps) : Fl_Glut_Window(x, y, w, h, s), params(_params), pSystem(ps) {
		mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_OPENGL3);
		p = 0;
		CreateParticleSystem();
		Fl::add_idle(idleCallback, this);
	}

	~SPHWindow() {
	}

private:
	shared_ptr<ParticleSystem> pSystem;
	const Params &params;

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
	int xOrigin = -1;//
	int particleIdInFocus = PARTICLE_IN_FOCUS;


public: 
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
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
		glEnable(GL_POINT_SPRITE_ARB);
		glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
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
			if (focusParticleId == -1) {
				if (pList[i]->isSurface && SURFACE_TRACKING) {
					glColor3d(0.0, 1.0, 0.0);
				}
				else {
					SetNormalParticleColor();
				}
			}
			else if (pList[i]->id == focusParticleId) {
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

	void printShaderInfoLog(GLint shader)
	{
		int infoLogLen = 0;
		GLchar *infoLog;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0)
		{
			infoLog = new GLchar[infoLogLen];
			// error check for fail to allocate memory omitted
			glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
			fprintf(stderr, "InfoLog:\n%s\n", infoLog);
			delete[] infoLog;
		}
	}

	void initShaders(void)
	{
		GLuint f, v;
		glClearColor(1.0, 1.0, 1.0, 0.0);

		v = glCreateShader(GL_VERTEX_SHADER);
		f = glCreateShader(GL_FRAGMENT_SHADER);
		// Read source code from files
		char *vv = textFileRead("particle.vert");
		char *ff = textFileRead("particle.frag");

		glShaderSource(v, 1, &vv, NULL);
		glShaderSource(f, 1, &ff, NULL);

		GLint compiled;

		glCompileShader(v);
		glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			fprintf(stderr, "Vertex shader not compiled.\n");
			printShaderInfoLog(v);
		}

		glCompileShader(f);
		glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			fprintf(stderr, "Fragment shader not compiled.\n");
			printShaderInfoLog(f);
		}

		p = glCreateProgram();

		glAttachShader(p, v);
		glAttachShader(p, f);
		glBindAttribLocation(p, 0, "in_Position");
		glBindAttribLocation(p, 1, "in_Color");

		glLinkProgram(p);
		glGetProgramiv(p, GL_LINK_STATUS, &compiled);
		if (compiled != GL_TRUE) {
			GLchar *infoLog; GLint length;
			glGetProgramiv(p, GL_INFO_LOG_LENGTH, &length);
			infoLog = new GLchar[length];
			glGetProgramInfoLog(p, length, NULL, infoLog);
			fprintf(stderr, "Link log=%s\n", infoLog);
			delete[] infoLog;
		}
		glUseProgram(p);
	}


	void draw(void)
	{
		if (!p) {
			initShaders();
		}
		if ((!valid())) {
			reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
		}
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
		glUseProgram(0);
		drawContainer(OFFSET_X, OFFSET_Y, OFFSET_Z);
		glEnable(GL_POINT_SMOOTH);
		glUseProgram(p);
		RenderParticleSystem();

		//update Particle System for the next timestep
		pSystem->update();

		glPopMatrix();

		//glutSwapBuffers();
		swap_buffers();
	}

	void computePos(double deltaMove) {

		x += deltaMove * lx * 0.1f;
		z += deltaMove * lz * 0.1f;
	}

	void myMouse(int button, int state, int x, int y)
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

	void myMotion(int x, int y)
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
		//DEBUG
		if (key == 'f') {
			particleIdInFocus = rand() % (NUM_PARTICLE_X * NUM_PARTICLE_Y * NUM_PARTICLE_Z);
		}
		//

	}

	virtual int handle(int event) {
		static int first = 1;
		if (first && event == FL_SHOW && shown()) {
			first = 0;
			make_current();
			GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		}
		make_current();
		int ex = Fl::event_x();
		int ey = Fl::event_y();
		int button;
		switch (event) {

		case FL_PUSH:
			button = Fl::event_button() - 1;
			myMouse(button, GLUT_DOWN, ex, ey);
			myMotion(ex, ey);
			return 1;
			break;
		case FL_RELEASE:
			myMouse(0, GLUT_UP, ex, ey);
			return 1;
			break;
		case FL_DRAG:
			myMotion(ex, ey);
			return 1;
			break;
		case FL_KEYBOARD:
			if (Fl::event_text()[0]) {
				keyPressed(Fl::event_text()[0], ex, ey); 
				return 1;
				break;
			}
		}

		return Fl_Gl_Window::handle(event);
	}
};

void idleCallback(void* pData)
{
	if (pData)
	{
		SPHWindow* pWindow = reinterpret_cast<SPHWindow*>(pData);
		pWindow->redraw();
	}
}

void startStopCb(Fl_Widget *w, void *) {
	if (pSystem->getState()) {
		pSystem->stopSystem();
	}
	else {
		pSystem->startSystem();
	}
	Fl_Button* bt = (Fl_Button*)w;
	bt->label(pSystem->getState() ? "STOP" : "START");
}

void onExitCb(Fl_Widget* widget, void*) {
	exit(0);
}

void resetCb(Fl_Widget* widget, void* data) {
	pSystem->resetSystem();
	((Fl_Button*)data)->label("START");
}

void onGravitySliderDrag(Fl_Widget* widget, void* data) {
	params->gravity = ((Fl_Slider*)widget)->value();
	stringstream ss;
	ss << fixed << setprecision(2) << params->gravity;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

void onRestDensSliderDrag(Fl_Widget* widget, void* data) {
	params->restDensity = ((Fl_Slider*)widget)->value();
	stringstream ss;
	ss << fixed << setprecision(2) << params->restDensity;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

void onCollisionDampingSliderDrag(Fl_Widget* widget, void* data) {
	params->collisionDamping = ((Fl_Slider*)widget)->value();
	stringstream ss;
	ss << fixed << setprecision(2) << params->collisionDamping;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

void onGasConstSliderDrag(Fl_Widget* widget, void* data) {
	params->gasConstant = ((Fl_Slider*)widget)->value();
	stringstream ss;
	ss << fixed << setprecision(2) << params->gasConstant;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

void onViscosSliderDrag(Fl_Widget* widget, void* data) {
	params->setViscosVarSet(((Fl_Slider*)widget)->value());
	stringstream ss;
	ss << fixed << setprecision(2) << params->viscosity;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

void onTensionCoefSliderDrag(Fl_Widget* widget, void* data) {
	params->tensionCoef = ((Fl_Slider*)widget)->value();
	stringstream ss;
	ss << fixed << setprecision(2) << params->tensionCoef;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

void onTensionThreshSliderDrag(Fl_Widget* widget, void* data) {
	params->tensionThresh = ((Fl_Slider*)widget)->value();
	stringstream ss;
	ss << fixed << setprecision(2) << params->tensionThresh;

	((Fl_Text_Buffer*)data)->text(ss.str().c_str());
}

int lastSliderPos = 60;
void createSliderForParam(char* title, double minVal, double maxVal, double& param, Fl_Callback* cb) {
	stringstream ss;
	Fl_Slider *slider = new Fl_Slider(20, lastSliderPos, 200, 30, title);
	slider->type(FL_HOR_NICE_SLIDER);
	slider->bounds(minVal, maxVal);
	slider->value(param);
	Fl_Text_Display *valTxt = new Fl_Text_Display(240, lastSliderPos, 70, 30);
	Fl_Text_Buffer *buff = new Fl_Text_Buffer();
	valTxt->buffer(buff);
	ss << fixed << setprecision(2) << param;
	buff->text(ss.str().c_str());
	valTxt->align(FL_ALIGN_CENTER);
	slider->callback(cb, buff);
	lastSliderPos += 60;
}

int main(int argc, char* argv[])
{
	Fl::use_high_res_GL(true);
	params = make_unique<Params>();
	pSystem = make_shared<ParticleSystem>(NUM_PARTICLE_X, NUM_PARTICLE_Y, NUM_PARTICLE_Z, *params);

	SPHWindow *sphWindow = new SPHWindow(50, 50, WINDOW_WIDTH, WINDOW_HEIGHT, "SPH", *params, pSystem);
	sphWindow->end();
	sphWindow->show(argc, argv);
	
	Fl_Window *window = new Fl_Window(1100, 50, 320, 600, "Controller");
	Fl_Button *startBtn = new Fl_Button(20, 20, 80, 25, "START");
	startBtn->callback(startStopCb, 0);

	Fl_Button *resetBtn = new Fl_Button(120, 20, 80, 25, "Reset");
	resetBtn->callback(resetCb, startBtn);

	//slider control + value display
	createSliderForParam("Gravity", 1.0, 20.0, params->gravity, onGravitySliderDrag);
	createSliderForParam("Rest Density", 200, 2000, params->restDensity, onRestDensSliderDrag);
	createSliderForParam("Collision Damping", 0.1, 1.0, params->collisionDamping, onCollisionDampingSliderDrag);
	createSliderForParam("Gas Constant", 1.0, 5.0, params->gasConstant, onGasConstSliderDrag);
	createSliderForParam("Viscosity", 1.0, 10.0, params->viscosity, onViscosSliderDrag);
	createSliderForParam("Surface Tension Coefficient", 1.0, 5.0, params->tensionCoef, onTensionCoefSliderDrag);
	createSliderForParam("Surface Tension Threshold", 0.0, 100.0, params->tensionThresh, onTensionThreshSliderDrag);

	

	window->end();
	window->show(argc, argv);
	sphWindow->callback(onExitCb);
	window->callback(onExitCb);

	return Fl::run();
}
