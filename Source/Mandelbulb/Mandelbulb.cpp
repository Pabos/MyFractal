#include <vector>
#include <windows.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <AntTweakBar.h>
#include <common/shader.hpp>
#include <common/keyinput.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <ctime>
using namespace glm;
using namespace std;
#define STEPONCE 1
#define TW true
#define NOTW false
#define Axes true
#define NOaxes false
#define ROTPOINTER true
#define ROTCENTRE false
void loadsetup();
void startsave();
void fileload(int);
void filesave(int);
void fnameprint(string, string, int);
void defaultsetup();
void guisetup();
void navigate();
void recalc(int);
void display(int);
void reflexdisplay();
void dispaxes();
void reset(int);
void MouseButtonCB(GLFWwindow*, int, int, int);
void MousePosCB(GLFWwindow*, double, double);
void MouseWheelCB(GLFWwindow*, double, double);
void KeybCB(GLFWwindow*, int, int, int, int);
void SizeCB(GLFWwindow*, int, int);
void callplay();
void calllerpall(float);
void callshow();
void callstepin(float);
void callrandomize(int);
void callfade();
void callpause(float);
void defrot(); 
void calldisplay();
void callreflex();
void callrecalc(int, bool, bool);
bool findrotpoint(bool);
void shutdown_();
void gpushaders();
void gpubuffers();
void environment();

///////////////////////////////////////////

struct Datastruct
{
	int steps;		int slice;		float zoom;		int dum0;
	vec3 camera;									float dmax;
	vec3 centerrot;									int numsteps;
	vec3 raycam;									float accuracy;
	vec3 wx;										float df;
	vec3 wy;										float order;
	vec3 wz;										float phase;
	float mr;		float fr;		float sc;		float focal;
	float foldx;	float foldy;	float foldz;	float depth;
	vec3 juliav;									bool julia;
	float boxsizex; float boxsizey; float boxsizez; float focus;
	float KleinR;	float KleinI;	float kleinf;	float losScale;
	float maxiter;	float maxiter0;	float maxiter1;	float maxiter2;
	float miniter0;	float miniter1;	float miniter2;	int span;
	float bailout;	float minbail;	float dum2;		int spacemode;
	float bail0;	float bail1;	float bail2;	float bail3;
	vec3 LightDir;									float AmbLight;
	vec3 col1;										float LightCol;
	vec3 col2;										float LightSpc;
	float EmisCol;	float SpecPwr; float sine;		int	  ind;
	vec3 FogColor;									float FogEn;
	int ccol;		float occrange;	int frames;		float  sincos;
	int b0;			int b1;			int b2;			float radius;
	vec3 dispang;									float Otrap;
	vec3 space;										int Rspeed;
	vec3 colcentre;									int Tspeed;
	uint minmaxs[4];
	vec3 centrespace;								float dum1;
	vec4 dum12;
};
Datastruct locals;
GLuint	Positionbuffer, Coordbuffer, Filterbuffer, Computebuffer, Fragposbuffer, Fragcolbuffer, Sparebuffer, Axesbuffer;
GLuint	ShaderID[12], ReflexID, DrawID, ReflexDrawID, FilterID, RenderID, AxesID, spanID, FracmatID, AxesmatID, axcurID, rotangID, alfaID;
static vec4 fixedaxes[6]{ vec4(-8.0f * vec3(1, 0, 0), 1), vec4(8.0f * vec3(1, 0, 0), 1), vec4(-8.0f * vec3(0, 1, 0), 2),
						vec4(8.0f * vec3(0, 1, 0), 2), vec4(-8.0f * vec3(0, 0, 1), 3), vec4(8.0f * vec3(0, 0, 1), 3) }; 
vec4 storedaxes[6];
vec4 axesvertices[1033]{ vec4(-8.0f * vec3(1, 0, 0), 1), vec4(8.0f * vec3(1, 0, 0), 1), vec4(-8.0f * vec3(0, 1, 0), 2),
						vec4(8.0f * vec3(0, 1, 0), 2), vec4(-8.0f * vec3(0, 0, 1), 3), vec4(8.0f * vec3(0, 0, 1), 3) };
vec3 centreaxes;
int wgroupsize = 640, row = 40, col = 32;	// group size on gpu sizex = 32, sizey = 20, 
int numpixels = wgroupsize * row * col;
int width = 1024, height = 800, neww = 1024, newh = 800;
float screenw = 1024 + 400, screenh = 800;
float screenratio = 1;
string savepath = getenv("APPDATA") + string("\\Fractal");
GLFWwindow *mainwindow;
float counter;
struct Newstore
{
	Datastruct	locals[100];
	int ind;
	float focus;
	int dum0;
	int dum1;
	int dum2;
	float dum3;
	float loaded;
	float dum4;
	float dum5;
	float dum6;
	float dum7;
	mat4 dum8;
	mat4 dum9;
};
Newstore newstore;
struct Globs
{
	int numsteps;
	int showpause;
	int march;
	int fadepause;
	bool showfog;
	int dum2;
	int type;
	int ind;
	int dum4;
	int dum5;
	int dum6;
	bool consolehidden;
	bool autohide;
	bool menudisp;
	bool savesetup;
	bool saturate;
	int detected[12];
	vec4 dum9;
};
Globs globals;
/////////////////////////////////////////// main
int main(void)
{
	/////////////////////////////////////////// create environment
	environment();
	/////////////////////////////////////////// create GPU shaders
	gpushaders();
	/////////////////////////////////////////// create GPU buffers
	gpubuffers();

	if (GetFileAttributesA(savepath.c_str()) != 16)  defaultsetup();  //Da sistemare
	loadsetup();
	glClear(GL_DEPTH_BUFFER_BIT);
	callrecalc(STEPONCE, NOaxes, TW);
	/////////////////////////////////////////// loop
	do	glfwWaitEvents();
	while ((glfwGetKey(mainwindow, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(mainwindow) == 0));
	/////////////////////////////////////////// closing
	shutdown_();
	return 0;
}
//
void gpushaders()
{
	ShaderID[0] = LoadComputes("Bulb.shader");
	ShaderID[1] = LoadComputes("Bulb2.shader");
	ShaderID[2] = LoadComputes("Bulbs.shader");
	ShaderID[3] = LoadComputes("BulbDE.shader");
	ShaderID[4] = LoadComputes("Bulb2DE.shader");
	ShaderID[5] = LoadComputes("Box.shader");
	ShaderID[6] = LoadComputes("BoxDE.shader");
	ShaderID[7] = LoadComputes("Bspudd.shader");
	ShaderID[8] = LoadComputes("Hybrid0.shader");
	ShaderID[9] = LoadComputes("Hybrid1.shader");
	ReflexID    = LoadComputes("Reflex0.shader");
	ReflexDrawID= LoadComputes("Reflex1.shader");
	DrawID		= LoadComputes("Draw.shader");
	FilterID	= LoadComputes("Filter.shader");
	RenderID	= LoadShaders("vertex.shader", "fragment.shader");
	alfaID		= glGetUniformLocation(RenderID, "alfa");
	FracmatID	= glGetUniformLocation(RenderID, "MVPfrac");
	AxesID		= LoadShaders("vertex1.shader", "fragment1.shader");
	AxesmatID	= glGetUniformLocation(AxesID, "MVP");
	axcurID		= glGetUniformLocation(AxesID, "axcurr");
	rotangID	= glGetUniformLocation(AxesID, "rotang");
	return;
}
//
void environment()
{
	fprintf(stdout, "Getting started\n");
	if (!glfwInit()) { fprintf(stdout, "Failed to initialize GLFW\n");/* return -1; */}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, FALSE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /// ???

	/////////////////////////////////////////
	mainwindow = glfwCreateWindow(width + 400, height, "Main", NULL, NULL);
	if (mainwindow == NULL) { fprintf(stdout, "Failed to open GLFW window"); glfwTerminate(); /*return -1;*/ }
	glfwSetWindowPos(mainwindow, 320, 30);
	glfwMakeContextCurrent(mainwindow);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH);
	glLineWidth(1);
	/////////////////////////////////////////// input handling
	glfwSetMouseButtonCallback(mainwindow, MouseButtonCB);
	glfwSetScrollCallback(mainwindow, MouseWheelCB);
	glfwSetCursorPosCallback(mainwindow, MousePosCB);
	glfwSetKeyCallback(mainwindow, KeybCB);
	glfwSetWindowSizeCallback(mainwindow, SizeCB);
	glfwSetInputMode(mainwindow, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	glfwSetInputMode(mainwindow, GLFW_STICKY_KEYS, GL_TRUE);

	/////////////////////////////////////////// Tweakbar setup
	if (TwInit(TW_OPENGL_CORE, NULL) == NULL) { fprintf(stdout, "Failed to initialize Tw\n");  glfwTerminate(); glfwDestroyWindow(mainwindow); /*return -1;*/ }
	TwWindowSize(width + 400, height);
	guisetup();

	glewExperimental = true;		// Needed for core profile?
	if (glewInit() != GLEW_OK) { fprintf(stdout, "Failed to initialize GLEW\n"); glfwTerminate(); glfwDestroyWindow(mainwindow); /*return -1;*/ }

	return;
}
//
void gpubuffers()
{
	glGenBuffers(1, &Positionbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numpixels * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Positionbuffer);
	///////////////////////////////////////////
	glGenBuffers(1, &Coordbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Coordbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numpixels * sizeof(vec2), NULL, GL_DYNAMIC_COPY);
	for (int y = 0; y < 800; y++) for (int x = 0; x < 1024; x++)
	{
		vec2 coord = (4.0f / 1024.0f) * vec2(x, y) - vec2(2.000f, 1.5625f);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, (y * 1024 + x) * sizeof(vec2), sizeof(vec4), &coord);
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Coordbuffer);
	///////////////////////////////////////////
	glGenBuffers(1, &Computebuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(locals), &locals, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, Computebuffer);
	///////////////////////////////////////////
	glGenBuffers(1, &Filterbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Filterbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numpixels * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, Filterbuffer);
	///////////////////////////////////////////
	glGenBuffers(1, &Fragposbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Fragposbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numpixels * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, Fragposbuffer);
	//
	glBindBuffer(GL_ARRAY_BUFFER, Fragposbuffer);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, 0, NULL);
	glEnableVertexAttribArray(3);
	///////////////////////////////////////////
	glGenBuffers(1, &Fragcolbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Fragcolbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numpixels * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, Fragcolbuffer);
	//
	glBindBuffer(GL_ARRAY_BUFFER, Fragcolbuffer);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, 0, NULL);
	glEnableVertexAttribArray(4);
	///////////////////////////////////////////
	glGenBuffers(1, &Sparebuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Sparebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numpixels * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Sparebuffer);
	///////////////////////////////////////////
	glGenBuffers(1, &Axesbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, Axesbuffer);
	glBufferData(GL_ARRAY_BUFFER, (8 + 1024) * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	for (int i = 0; i < 1025; i++)
	{
		vec4 temp = vec4(150.0f * cos(i * 6.28f / 1024.0f), 150.0f * sin(i * 6.28f / 1024.0f), i * 6.28f / 1024.0f, 5);
		if (i % 256 == 0) temp = vec4(0, 0, 0, 5);
		glBufferSubData(GL_ARRAY_BUFFER, (i + 8) * sizeof(vec4), sizeof(vec4), &temp);
	}
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(5);
	return;
}
//
void shutdown_()
{
	if (globals.savesetup == true)
	{
		newstore.locals[globals.ind] = locals;
		startsave();
		filesave(globals.type);
	}
	_fcloseall();
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDeleteBuffers(1, &Positionbuffer);
	glDeleteBuffers(1, &Computebuffer);
	glDeleteBuffers(1, &Filterbuffer);
	glDeleteBuffers(1, &Fragposbuffer);
	glDeleteBuffers(1, &Fragcolbuffer);
	glDeleteBuffers(1, &Axesbuffer);
	glDeleteProgram(ShaderID[0]);
	glDeleteProgram(ShaderID[1]);
	glDeleteProgram(ShaderID[2]);
	glDeleteProgram(ShaderID[3]);
	glDeleteProgram(ShaderID[4]);
	glDeleteProgram(ShaderID[5]);
	glDeleteProgram(ShaderID[6]);
	glDeleteProgram(ShaderID[7]);
	glDeleteProgram(ShaderID[8]);
	glDeleteProgram(ShaderID[9]);
	glDeleteProgram(DrawID);
	glDeleteProgram(FilterID);
	glDeleteProgram(RenderID);
	glDeleteProgram(AxesID);
	TwTerminate();
	glfwDestroyWindow(mainwindow);
	glfwTerminate();
	return;
}
///////////////////////////////////////////
TwBar *RotGUI, *TransGUI, *DBGGUI, *PaletteGUI, *MenuGUI, *SetGUI, *HybridGUI;
bool play, show, record, printj, copied = false, twenabled = true;
bool LEFT_PRESSED, RIGHT_PRESSED, BOTH_PRESSED, BUTTON_PRESSED = false;
int  axcurr, fader = true, wheel = 0, ind = -1, oldcol;
bool pivot = false, sysmod = true, dispax = true, rotpointer = false, mouserot = true, muosetrans = false;
vec2 mousepos, oldpos, posonclick;
vec4 finder[2];
vec3 newrot, disprot, eye, cpos, newup, newcam = vec3(0, 0, 4);
float rotang, startang, dispang;
mat3 axesmat2{ vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };
mat3 axesmat1{ vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };
mat3 axesmat0{ vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };
float calctime, disptime, pointsize = 1.5f;
float rand_max = float(RAND_MAX);
bool reflex = false;
void MouseButtonCB(GLFWwindow* window, int button, int action, int mods)
{
	TwEventMouseButtonGLFW(mainwindow, button, action, mods);

	oldpos = mousepos;
	if ((action == 1))
	{
		if (mousepos.x > 1023) return;
		switch (button)
		{
			case GLFW_MOUSE_BUTTON_MIDDLE:	defrot(); 	pivot = false; 		return;		break;
			case GLFW_MOUSE_BUTTON_LEFT:	LEFT_PRESSED = true;	axcurr = wheel;		break;
			case GLFW_MOUSE_BUTTON_RIGHT:	RIGHT_PRESSED = true;	axcurr = !wheel;	break;
		}
		if (LEFT_PRESSED && RIGHT_PRESSED)	{	 LEFT_PRESSED = false;	RIGHT_PRESSED = false;	
												 BOTH_PRESSED = true;	axcurr = 2;
												 dispaxes();	glfwSwapBuffers(mainwindow);}


		if (!BUTTON_PRESSED) //passa solo la prima volta
		{
			posonclick	= mousepos;
			finder[0]	= vec4(2.0f * (mousepos.x - 512), -2.0f * (mousepos.y - 400), 0, 4);
			findrotpoint(ROTPOINTER);
			BUTTON_PRESSED = true;
			startang = 0; rotang = 0;
			dispaxes();	glfwSwapBuffers(mainwindow);
		}
		return;
	}
	else
	{
		if (reflex)
		{
			callreflex();
			reflexdisplay();
		}
		else
		{
			recalc(STEPONCE);
			glFinish();
			glClear(GL_COLOR_BUFFER_BIT);
			display(STEPONCE);
		}
		TwDraw();
		glfwSwapBuffers(mainwindow);

		RIGHT_PRESSED = false;
		LEFT_PRESSED = false;
		BOTH_PRESSED = false;
		BUTTON_PRESSED = false;
		return;
	}
}
void MousePosCB(GLFWwindow* window, double x, double y)
{
	TwEventMousePosGLFW(mainwindow, x, y);
	mousepos = vec2(int(x * screenratio), int(y * screenratio));
	if (play || show) return;
	if (BUTTON_PRESSED && (mousepos.y > 3) && (mousepos.x < 1023))
	{
		navigate();
		callrecalc(locals.numsteps, Axes, TW);
		return;
	}
	else
	{
		TwDraw();
		glfwSwapBuffers(mainwindow);
		Sleep(50);
		return;
	}
}
void MouseWheelCB(GLFWwindow* window, double xoffset, double yoffset)
{
	wheel = (wheel == 0) ? 1 : 0;
	int wheel_ = !wheel;
	TwSetParam(RotGUI, NULL, "visible", TW_PARAM_INT32, 1, &wheel_);
	TwSetParam(TransGUI, NULL, "visible", TW_PARAM_INT32, 1, &wheel);
	TwDraw();
	glfwSwapBuffers(mainwindow);
	switch (wheel)
	{
		case 0: mouserot = true;	muosetrans = false; return;
		case 1: mouserot = false;	muosetrans = true; return;
		default: mouserot = true;	muosetrans = false; return;
	}
	return;
}
void SizeCB(GLFWwindow* window, int width, int height)
{
	screenw = width;
	screenh = height;
	if (width == 1424)	pointsize = 1.5f;	else	pointsize = 3.0f;
	static float standard = 1024.0f / 800.0f; 
	if (screenratio > standard)
	{	neww = 1024; newh = 800; }
	else
	{	neww = height * standard; newh = height; }
	screenratio = 1024.0f / neww;
	if (twenabled)	glViewport(0, 0, neww, newh);
	else			glViewport((screenw - neww) / 2, 0, neww, newh);
	TwWindowSize(width, height);
//	
	string 
	fname = " Rotate position='"	+ to_string(neww + 200) + " 0'";
	TwDefine(fname.c_str());
	fname = " Translate position='"	+ to_string(neww + 200) + " 0'";
	TwDefine(fname.c_str());
	fname = " Menu position='"		+ to_string(neww) + " 0'";
	TwDefine(fname.c_str());
	fname = " Palette position='"	+ to_string(neww) + " 380'";
	TwDefine(fname.c_str());
	fname = " Set position='"		+ to_string(neww + 200) + " 100'";
	TwDefine(fname.c_str());
	fname = " Hybrid position='"	+ to_string(neww + 200) + " 100'";
	TwDefine(fname.c_str());
	fname = " Debug position='"		+ to_string(neww + 200) + " 560'";
	TwDefine(fname.c_str());
//
	display(STEPONCE);	
	TwDraw();
	glfwSwapBuffers(mainwindow);
	return;
}
void KeybCB(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (action == 0) return;
	if ((glfwGetKey(mainwindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) && (show || play))
	{
		if (play || show)
		{
			record = false; play = false; show = false;
			twenabled = true;
			glClear(GL_COLOR_BUFFER_BIT);
			if (twenabled)	glViewport(0, 0, neww, newh);
			return;
		}
		return;
	}
	if ((glfwGetKey(mainwindow, GLFW_KEY_T) == GLFW_PRESS) && (show || play))
	{
		twenabled = !twenabled;
		glClear(GL_COLOR_BUFFER_BIT);
		if (twenabled)	glViewport(0, 0, neww, newh);
		else			glViewport((screenw - neww) / 2, 0, neww, newh);
		return;
	}
	if (glfwGetKey(mainwindow, GLFW_KEY_C) == GLFW_PRESS)
	{
		vec4 temp;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, ((800 - mousepos.y) * width + mousepos.x) * sizeof(vec4), sizeof(vec4), &temp);
		locals.centrespace = vec3(temp.x, -temp.y, temp.z);
		callrecalc(STEPONCE, NOaxes, TW);
		return;
	}
	if (glfwGetKey(mainwindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
   		if (play || show)
		{
			record = false; play = false; show = false;
			twenabled = true;
			glClear(GL_COLOR_BUFFER_BIT);
			if (twenabled)	glViewport(0, 0, neww, newh);
			return;
		}
		else
		{
			vec4 temp;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, ((800 - mousepos.y) * width + mousepos.x) * sizeof(vec4), sizeof(vec4), &temp);
			locals.focal = length(locals.camera - vec3(temp.x, temp.y, temp.z));
			display(STEPONCE);
			TwDraw();
			glfwSwapBuffers(mainwindow);
			return;
		}
	}	
	if (glfwGetKey(mainwindow, GLFW_KEY_M) == GLFW_PRESS)
	{
		TwSetParam(DBGGUI, NULL, "iconified", TW_PARAM_INT32, 1, &globals.menudisp);
		globals.menudisp = !globals.menudisp;
		glClear(GL_COLOR_BUFFER_BIT);
		display(STEPONCE);
		TwDraw();
		glfwSwapBuffers(mainwindow);
		return;
	}
	return;
}
///////////////////////////////////////////
quat qinc, qinc_, newquat;
float callrotang(vec2 oldpos, vec3 ax, int ax_)
{ 
	vec2 mousetocentre = mousepos - posonclick;
	if (length(mousetocentre) < 75)
	{
		qinc = newquat;
		qinc_ = newquat;
		return 0;
	}
	else
	{
		vec3 tip = (vec3(oldpos - posonclick, 1));
		if (startang == 0)
		{
			startang = asin((cross(tip, vec3(1, 0, 0)) / (length(tip))).z);
			if (tip.y < 0 && tip.x < 0) startang = 3.1415f - startang; else
			if (tip.y > 0 && tip.x < 0) startang = 3.1415f - startang; else
			rotang = 0;
			dispang = 0;
			return  0;
		}
		else
		{
			float incang = asin((cross(vec3(mousetocentre, 1), tip) / (length(tip) * length(mousetocentre))).z);
			rotang += incang;
			dispang = rotang * 180 / 3.1415f;
			float ret = -incang * powf(2.0f, float(locals.Rspeed));
			qinc = angleAxis(ret, ax);
			qinc_ = angleAxis(-ret, axesmat2[ax_]);
			return  -incang * powf(2.0f, float(locals.Rspeed));
		}
	}
}
struct ds_
{ float ds; int axe; };
ds_ callds()
{
	float incpos = 0.005f * powf(2.0f, float(locals.Tspeed));
	float ang = 0;
	vec3 tip = (vec3(oldpos - posonclick, 1));
	if (mouserot)
	{
		if (LEFT_PRESSED)	return ds_{ (oldpos.x - mousepos.x) * incpos, 0 }; 
		if (RIGHT_PRESSED)	return ds_{ (-oldpos.y + mousepos.y) * incpos, 1 }; 
	}
	else
	{
		if (RIGHT_PRESSED)	return ds_{ (oldpos.x - mousepos.x) * incpos, 0 }; 
		if (LEFT_PRESSED)	return ds_{ (-oldpos.y + mousepos.y) * incpos, 1 }; 
	}
		if (BOTH_PRESSED)	{ return ds_{ (-oldpos.x + mousepos.x + oldpos.y - mousepos.y) * incpos, 2 }; }	
}
mat3 rotupdat(ds_ ds, mat3 axesmat)
{
	mat3 axesmat_;
	callrotang(oldpos, axesmat[ds.axe], ds.axe);
	if (!(sysmod || pivot)) for (int i = 0; i < 3; i++)
	{	storedaxes[2 * i]		= vec4(-4.0f * axesmat2[i], i + 1); 
		storedaxes[2 * i + 1]	= vec4(4.0f * axesmat2[i], i + 1); }
	for (int i = 0; i < 3; i++) axesmat2[i] = qinc_ * axesmat2[i];
	for (int i = 0; i < 3; i++) axesmat_[i] = qinc * axesmat0[i];
	locals.wx	= axesmat_[0];
	locals.wy	= axesmat_[1];
	locals.wz	= axesmat_[2];
	locals.raycam = qinc * locals.raycam;
	newcam		= qinc * newcam;
	if (!pivot)
	{
		locals.centerrot	= qinc * locals.centerrot;
		locals.LightDir		= qinc * locals.LightDir;
	}
	return axesmat_;
}
void centreupdt(vec3 centreinc)
{
		if (!pivot)
		{
			newrot += centreinc;
			locals.centerrot += centreinc;
		}
}
void camupdate()
{
	if (rotpointer)
	{
		locals.camera = newcam + newrot;
		locals.centerrot = locals.camera - locals.raycam;
	}
	else
	{
		locals.camera = normalize(locals.raycam) * newstore.focus + locals.centerrot;
	}
}
ds_ ds;
void navigate()
{
	mat3 axesmat;
	if (sysmod || pivot)
		axesmat = axesmat0;
	else
		axesmat = axesmat1;
	ds = callds();

	if (mouserot)
	{
		eye = locals.raycam + locals.centerrot - disprot;
		cpos = locals.centerrot - disprot;
		newup = locals.wy;
		if (rotpointer || sysmod) for (int i = 0; i < 3; i++)
		{	storedaxes[2 * i]		= vec4(-4.0f * axesmat0[i], i + 1); 
			storedaxes[2 * i + 1]	= vec4(4.0f * axesmat0[i], i + 1); }
		axesmat0 = rotupdat(ds, axesmat);
	}
	else 
	if (!pivot)
	{
		vec3 centreinc = ds.ds * axesmat[ds.axe];
		locals.centerrot += centreinc;
		newrot += centreinc;
		eye = eye + centreinc;
		cpos = cpos + centreinc;
	}
	if (!pivot) //camupdate();
	{
		if (rotpointer)
		{
			locals.camera = newcam + newrot;
			locals.centerrot = locals.camera - locals.raycam;
		}
		else
		{
			locals.camera = normalize(locals.raycam) * newstore.focus + locals.centerrot;
		}
	}
	oldpos = mousepos;
	return;
}
bool findrotpoint(bool ifpointer)
{
	vec4 newrot_;
	if (rotpointer)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
		if(ifpointer)
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, GLintptr((800 - mousepos.y) * width + mousepos.x) * sizeof(vec4), sizeof(vec4), &newrot_);
		else
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, (400 * width + 512) * sizeof(vec4), sizeof(vec4), &newrot_);
		if (newrot_.w)
		{
			newrot = vec3(newrot_.x, newrot_.y, newrot_.z);
			disprot = newrot;
			eye = locals.raycam + locals.centerrot - disprot;
			cpos = locals.centerrot - disprot;
			newup = locals.wy;
			if (sysmod || pivot) for (int i = 0; i < 3; i++)
			{	storedaxes[2 * i]		= vec4(-4.0f * axesmat0[i], i + 1);
				storedaxes[2 * i + 1]	= vec4(4.0f * axesmat0[i], i + 1); } 
			newcam = locals.camera - newrot;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (!sysmod) for (int i = 0; i < 3; i++)
		{	storedaxes[2 * i]		= vec4(-4.0f * axesmat2[i], i + 1);
			storedaxes[2 * i + 1]	= vec4(4.0f * axesmat2[i], i + 1); }
		locals.centerrot = locals.camera - normalize(locals.raycam) * newstore.focus;
		return true;
	}
}
static string colortypes[3] = { "Centre Col' ", "Eye Col' ", "Pointer Col' " };
void defrot()
{
	pivot = false;
	rotpointer = !rotpointer;
	if (rotpointer)
	{
		TwDefine("Rotate/Rotptr label='Rotpointer'");	TwDefine("Translate/Rotptr label='Rotpointer'");
	}
	else
	{
		locals.ccol = oldcol;
		TwDefine((" Palette/ColCentre label='" + colortypes[oldcol]).c_str());
		TwDefine("Rotate/Rotptr label='Rotcentre'");	TwDefine("Translate/Rotptr label='Rotcentre'");
	}
	return;
}
//////////////////////////////////////////
void recalc(int steps)
{
float calctime_ = (float)glfwGetTime();
	locals.steps = steps;
	locals.numsteps = globals.numsteps;
	switch (locals.ccol) {
	case 0:	locals.colcentre = vec3(0, 0, 0);	break;
	case 1: locals.colcentre = locals.camera;	break;
	case 2:	locals.colcentre = newrot;			break; }
	locals.minmaxs[0] = 0; locals.minmaxs[1] = 10000000;
	locals.minmaxs[2] = 0; locals.minmaxs[3] = 10000000;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(locals), &locals, GL_DYNAMIC_COPY);

	glUseProgram(ShaderID[globals.type]);
	GLintptr offs = GLintptr(&locals.slice) - GLintptr(&locals);
	if (steps == 1)
		for (int slice = 0; slice < (locals.numsteps / steps); slice++)
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offs, sizeof(int), &slice);
			glDispatchCompute(col, row / locals.numsteps, 1);
		}
	else
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offs, sizeof(int), NULL);
			glDispatchCompute(col / locals.numsteps, row / locals.numsteps, 1);
		}
	glFinish();
calctime = (float)glfwGetTime() - calctime_;
	return;
}
void display(int steps)
{
float disptime_ = (float)glfwGetTime();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, GLintptr(&locals.minmaxs) - GLintptr(&locals), sizeof(locals.minmaxs), &locals.minmaxs);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(locals), &locals, GL_DYNAMIC_COPY);
	glUseProgram(DrawID);
	glDispatchCompute(col / steps, row / steps, 1);
	if (!reflex)
	{
		if (locals.span != 0)
		{
			glUseProgram(FilterID);
			glDispatchCompute(col / steps, row / steps, 1);
		}
		if (steps == 1) glPointSize(pointsize); else glPointSize(globals.numsteps * 2);
		glUseProgram(RenderID);
		static mat4
			MVPfrac = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.001f, 100.0f) * lookAt(vec3(0, 0, 24), vec3(0, 0, 0), vec3(0, 1, 0));
		glUniformMatrix4fv(FracmatID, 1, GL_FALSE, &MVPfrac[0][0]);
		if (steps == 1)
			glDrawArrays(GL_POINTS, 0, 800 * 1024);
		else for (int i = 0; i < 800 / steps; i++)
			glDrawArrays(GL_POINTS, i * 1024, 1024 / steps);
	}

	glFinish();
disptime = (float)glfwGetTime() - disptime_;
	glDisable(GL_DEPTH_TEST);
	return;
}
void reflexdisplay()
{
	glUseProgram(ReflexDrawID);
	glDispatchCompute(col, row, 1);

	glUseProgram(RenderID);
	glDrawArrays(GL_POINTS, 0, 800 * 1024);
	glFinish();
}
void dispaxes()
{
	//glEnable(GL_DEPTH_TEST);
	//glClear(GL_DEPTH_BUFFER_BIT);
	if (!dispax) return;
	glUseProgram(AxesID);
	glBindBuffer(GL_ARRAY_BUFFER, Axesbuffer);
	mat4 MVP = perspective(2.0f * atan((locals.zoom * 1.6f / 1.024f) / 4.0f), 1024.0f / 800.0f, 0.001f, 100.0f);
	if (rotpointer)
	{
		if (sysmod || pivot)	glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(vec4), &storedaxes);
		else					glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(vec4), &fixedaxes);	
		MVP *= lookAt(eye, cpos, newup);
	}
	else
	{
		if (sysmod || pivot)	MVP *= lookAt(vec3(0, 0, 4), vec3(0, 0, 0), vec3(0, 1, 0));
		else					MVP *= lookAt(locals.raycam, vec3(0, 0, 0), axesmat0[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(vec4), &fixedaxes);
	}
	glUniformMatrix4fv(AxesmatID, 1, GL_FALSE, &MVP[0][0]);
	glUniform1iv(axcurID, 1, &axcurr);
	glDrawArrays(GL_LINES, 0, 6);

	if ((mouserot || pivot))
	{
		finder[1] = vec4(2.0f * (mousepos.x - 512), -2.0f * (mousepos.y - 400), 0, 0);
		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(vec4), 2 * sizeof(vec4), &finder);
		MVP = ortho(-1024.0f, 1024.0f, -800.0f, 800.0f, 0.0f, 4.90f) * lookAt(vec3(0, 0, 4), vec3(0, 0, 0), vec3(0, 1, 0));
		glUniformMatrix4fv(AxesmatID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_LINES, 6, 2);

		MVP *= translate(mat4(1), vec3(2.0f * posonclick.x - 1024, -2.0f * posonclick.y + 800, 0)) * eulerAngleZ(startang);
		glUniformMatrix4fv(AxesmatID, 1, GL_FALSE, &MVP[0][0]);
		float temp = powf(2.0f, float(locals.Rspeed)) * rotang;
		glUniform1fv(rotangID, 1, &temp);
		glDrawArrays(GL_LINE_STRIP, 8, 1025);
	}
	return;
}
///////////////////////////////////////////
void calldisplay()
{
	display(STEPONCE);
	TwDraw();
	glfwSwapBuffers(mainwindow);
	return;
}
void callreflex()
{
	float calctime_ = (float)glfwGetTime();
	glUseProgram(ReflexID);
	GLintptr offs = GLintptr(&locals.slice) - GLintptr(&locals);
	for (int slice = 0; slice < (locals.numsteps); slice++)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offs, sizeof(int), &slice);
		glDispatchCompute(col, row / locals.numsteps, 1);
	}
	glFinish();
	calctime = (float)glfwGetTime() - calctime_;
	return;
}
void callrecalc(int steps, bool axes, bool tw)
{
	recalc(steps); 
	display(steps);
	if (axes && dispax) dispaxes();
	if (tw) TwDraw();
	glfwSwapBuffers(mainwindow);
	return;
}
///////////////////////////////////////////
void cleantwdraw()
{
	
	if (twenabled)	
		glScissor(neww, 0, 400, newh);
	else			
		glScissor((screenw + neww) / 2, 0, 400, newh);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	if (twenabled) TwDraw();
	glDisable(GL_SCISSOR_TEST);
	return;
}
void callplay()
{
	int run = 0;
	float amount = 0;
	ind++;
	while ((glfwWindowShouldClose(mainwindow) == 0) && play == true)
	{
		if (ind == newstore.loaded) ind = 0;
		glfwPollEvents();
		if (play == false) break;
		if (amount >= 1.0f)
		{
			callpause(globals.showpause);
			amount = 0;
			run = 0;
			ind++;
			TwDefine(("Menu/play label='Playing " + to_string(ind) + "'").c_str());
		}
		else
		{
			calllerpall(amount);
			amount += 1.0f / newstore.locals[ind].frames;
			run++;
			TwDefine(("Menu/play label='Framing " + to_string(run) + " of " + to_string(newstore.locals[ind].frames) + "'").c_str());
		}
		recalc(STEPONCE);
		callfade();
		if (record && !fader) snapshot(1, 800, 1024, savepath);
		cleantwdraw();
		glfwSwapBuffers(mainwindow);
	}
	TwDefine("Menu/play label='Play' "); TwDefine("Menu/record label='Record' ");
	TwDraw();
	glfwSwapBuffers(mainwindow);
	return;
}
void callshow()
{
	int run = 0;
	srand(time(0));
	while ((glfwWindowShouldClose(mainwindow) == 0) && show == true)
	{
		int temp = int(rand() * 5.0f / rand_max);
		globals.type = globals.detected[int(rand() * globals.detected[11] / rand_max)];
		fileload(globals.type);
		callrandomize(globals.type);
		recalc(STEPONCE); 
		for (int i = 0; i < globals.march && (glfwWindowShouldClose(mainwindow) == 0) && show == true; i++)
		{
			float temp = (float)glfwGetTime();
			glfwPollEvents();
			if (show == false) break;
			TwDefine(("Menu/show  label='Showing " + to_string(i) + " of " + to_string(run) + "'").c_str());
			callstepin(1.0f / (1.0f + globals.march));
			recalc(STEPONCE);
			callfade();
			glFinish();
			cleantwdraw();
			glfwSwapBuffers(mainwindow);
			temp = ((float)glfwGetTime() - temp);
			callpause(glm::max((float)globals.showpause - temp * 1000.0f, 0.0f));
			if (record)	snapshot(1, 800, 1024, savepath);
		}
		if (show == false)	break;
		run++;
	}
	TwDefine("Menu/show  label='Random Show'");
	return;
}
void callpause(float pause)
{
	for (int pausecounter = 0; pausecounter < pause; pausecounter += 200)
	{
		glfwPollEvents();
		if (twenabled) TwDraw();
		glfwSwapBuffers(mainwindow);
		if ((play == false) && (show == false) || (glfwWindowShouldClose(mainwindow) == 1))	return;
		Sleep((DWORD)200.0);
	}
	return;
}
void calllerpall(float amount)
{
	int nextind;
	if (ind == newstore.loaded - 1)	nextind = 0;
	else					nextind = ind + 1;
	locals.raycam		= normalize(lerp(newstore.locals[ind].raycam, newstore.locals[nextind].raycam, amount)) * 4.0f;
	locals.centerrot	= lerp(newstore.locals[ind].centerrot, newstore.locals[nextind].centerrot, amount);
	locals.camera		= normalize(locals.raycam) * newstore.focus + locals.centerrot;
	locals.wx			= normalize(lerp(newstore.locals[ind].wx, newstore.locals[nextind].wx, amount));
	locals.wy			= normalize(lerp(newstore.locals[ind].wy, newstore.locals[nextind].wy, amount));
	locals.wz			= normalize(lerp(newstore.locals[ind].wz, newstore.locals[nextind].wz, amount));
	locals.julia		= newstore.locals[nextind].julia;
	locals.maxiter		= newstore.locals[nextind].maxiter;
	locals.LightDir		= lerp(newstore.locals[ind].LightDir, newstore.locals[nextind].LightDir, amount);
	locals.juliav.x		= lerp(newstore.locals[ind].juliav.x, newstore.locals[nextind].juliav.x, amount);
	locals.juliav.y		= lerp(newstore.locals[ind].juliav.y, newstore.locals[nextind].juliav.y, amount);
	locals.juliav.z		= lerp(newstore.locals[ind].juliav.z, newstore.locals[nextind].juliav.z, amount);
	locals.phase		= lerp(newstore.locals[ind].phase, newstore.locals[nextind].phase, amount);
	locals.order		= lerp(newstore.locals[ind].order, newstore.locals[nextind].order, amount);
	locals.FogEn		= lerp(newstore.locals[ind].FogEn, newstore.locals[nextind].FogEn, amount);
	locals.FogColor		= lerp(newstore.locals[ind].FogColor, newstore.locals[nextind].FogColor, amount);
	locals.col1			= lerp(newstore.locals[ind].col1, newstore.locals[nextind].col1, amount);
	locals.col2			= lerp(newstore.locals[ind].col2, newstore.locals[nextind].col2, amount);
	return;
}
void callfade()
{
	glUseProgram(DrawID);
	glDispatchCompute(col, row, 1);
	glUseProgram(RenderID);
	glPointSize(pointsize);
	float alfa, noloop, counterinc = 2.0f / 50.0f;
	if (fader) noloop = 2.0f; else noloop = 0;
	for (float counter = 0; counter <= noloop; counter += counterinc)
	{
float disptime_ = (float)glfwGetTime();
		glfwPollEvents();
		if ((play == false) && (show == false))	break;
		if (fader)	alfa = pow(10, counter) / 100.0f;
		else		alfa = 1;
		glUniform1fv(alfaID, 1, &alfa);
		glDrawArrays(GL_POINTS, 0, 800 * 1024);	
		glfwSwapBuffers(mainwindow);
		glFinish();
disptime = ((float)glfwGetTime() - disptime_);
		Sleep(glm::max((float)globals.fadepause - disptime * 1000.0f, 0.0f));
	}
	alfa = 1;
	glUniform1fv(alfaID, 1, &alfa);
	return;
}
void callstepin(float march)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, GLintptr(&locals.minmaxs) - GLintptr(&locals), sizeof(locals.minmaxs), &locals.minmaxs);
	float distance = march * float(locals.minmaxs[2]) / 100000.0f;
	locals.camera = locals.camera - normalize(locals.raycam) * distance;
	locals.sine = float(distance * 5.0f + 1.0f);
	return;
}
float maxv(vec3 v)
{
	float temp = (v.x > v.y)	? v.x : v.y;
	return (temp > v.z)			? temp : v.z;
}
void callrandomize(int type)
{
	quat qinc;
	recalc(STEPONCE);
	if (type == 8)
	{
		rotpointer = true;
		TwDefine("Rotate/Rotptr label='Rotpointer'");
		TwDefine("Translate/Rotptr label='Rotpointer'");
		findrotpoint(ROTCENTRE);
		locals.b0 = 2.0 * rand() / rand_max;
		locals.b1 = 2.0 * rand() / rand_max;
		locals.b2 = 2.0 * rand() / rand_max;
	}
	else
	{
		locals.juliav.x += 0.15 * (rand() / rand_max - 0.5f);
		locals.juliav.y += 0.15 * (rand() / rand_max - 0.5f);
		locals.juliav.z += 0.15 * (rand() / rand_max - 0.5f);

		locals.mr		+= 0.30 * (rand() / rand_max - 0.5f);
		locals.fr		+= 0.30 * (rand() / rand_max - 0.5f);
		locals.sc		+= 0.60 * (rand() / rand_max - 0.5f);
	}
	qinc = normalize(quat{ rand() / rand_max, rand() / rand_max, rand() / rand_max, 0.5f * rand() / rand_max });
	axesmat0[0] = qinc * axesmat0[0];
	axesmat0[1] = qinc * axesmat0[1];
	axesmat0[2] = qinc * axesmat0[2];
	locals.wx = axesmat0[0];
	locals.wy = axesmat0[1];
	locals.wz = axesmat0[2];
	locals.raycam = qinc * locals.raycam;
	newcam = qinc * newcam;
	if (!pivot)
	{
		locals.centerrot = qinc * locals.centerrot;
		locals.LightDir = qinc * locals.LightDir;
	}
	camupdate();

	locals.zoom = 0.5f * (1.0f + (rand() / rand_max));

	locals.col1 = vec3(rand() / rand_max, rand() / rand_max, rand() / rand_max);
	if (globals.saturate)
		locals.col1 *= 1.0f / maxv(locals.col1);
	//locals.col2 = vec3(1, 1, 1) - locals.col1;
	locals.col2 = vec3(rand() / rand_max, rand() / rand_max, rand() / rand_max);
	if (globals.saturate)
		locals.col2 *= 1.0f / maxv(locals.col2);
	if (globals.showfog)
	{
		locals.FogEn = 0.5f * float(rand()) / rand_max;
		locals.FogColor = locals.FogEn * vec3(rand() / rand_max, rand() / rand_max, rand() / rand_max);
	}
	return;
}
///////////////////////////////////////////
void reset(int type)
{

	locals.camera = vec3(0.0f, 0.0f, newstore.focus);
	locals.raycam = vec3(0, 0, 4);
	locals.centerrot = vec3(0.0f);
	centreaxes = vec3(0.0f);
	locals.wx = vec3(1.0f, 0.0f, 0.0f);
	locals.wy = vec3(0.0f, 1.0f, 0.0f);
	locals.wz = vec3(0.0f, 0.0f, 1.0f);
	axesmat0[0] = vec3(1.0f, 0.0f, 0.0f);
	axesmat0[1] = vec3(0.0f, 1.0f, 0.0f);
	axesmat0[2] = vec3(0.0f, 0.0f, 1.0f);
	axesmat2[0] = vec3(1.0f, 0.0f, 0.0f);
	axesmat2[1] = vec3(0.0f, 1.0f, 0.0f);
	axesmat2[2] = vec3(0.0f, 0.0f, 1.0f);
	locals.LightDir = vec3(1, -1, -1);
	locals.dispang = vec3(0);
	dispax = true;
	locals.zoom = 1;
	return;
}
void startsave()
{
	FILE* fp;
	string fname = savepath + "\\start";
	if (fopen_s(&fp, fname.c_str(), "wb") == 0)
	{
		int dum;
		TwGetParam(DBGGUI, NULL, "iconified", TW_PARAM_INT32, 1, &dum);
		if (dum == 1) globals.menudisp = false; else globals.menudisp = true;
		fnameprint("   and ", fname, ind);
		fwrite(&globals, sizeof(Globs), 1, fp);
		fclose(fp);
	}
	else
		fnameprint("Error saving ", fname, ind);
	return;
}
static string typenames[10] = { "F_Bulb' ", "F_Bulb2' ", "F_Bulbs' ", "F_BulbDE' ", "F_Bulb2DE' ", 
								"F_Box' ", "F_BoxDE' ", "F_Spudsville' ",  "F_Pseudoklein' ", "F_Hybrid1' " };
void loadsetup()
{
	string fname = savepath + "\\start";
	FILE* fp = fopen(fname.c_str(), "r");
	if (fp != 0)
	{
		fnameprint("Loading", fname, 0);
		fread(&globals, sizeof(Globs), 1, fp);
		for (int i = 0; i < 10; i++)
		TwDefine((" Menu/Show" + to_string(i) + " label = '" + typenames[globals.detected[i]]).c_str());
		int temp = !globals.menudisp;
		TwSetParam(DBGGUI, NULL, "iconified", TW_PARAM_INT32, 1, &temp);
		ShowWindow(FindWindow("ConsoleWindowClass", NULL), globals.consolehidden);
		fclose(fp);
	}
	else
	{
		// if no start in app data load default start file
		fprintf(stdout, " No start file found\n");
		fname = "start";
		fp = fopen(fname.c_str(), "rb");
		fread(&globals, sizeof(Globs), 1, fp);
		for (int i = 0; i < 10; i++)
			TwDefine((" Menu/Show" + to_string(i) + " label = '" + typenames[globals.detected[i]]).c_str());
		int temp = !globals.menudisp;
		TwSetParam(DBGGUI, NULL, "iconified", TW_PARAM_INT32, 1, &temp);
		ShowWindow(FindWindow("ConsoleWindowClass", NULL), globals.consolehidden);
		fclose(fp);
		fprintf(stdout, " Loading default start file\n");
		startsave();
	}
	fileload(globals.type);
	return;
}
void fileload(int type)
{
	//reset(0);
	rotpointer = false;
	TwDefine("Rotate/Rotptr label='Rotcentre'");
	TwDefine("Translate/Rotptr label='Rotcentre'");
	string fname = savepath + "\\newsave" + to_string(type);
	FILE* fp = fopen(fname.c_str(), "rb");
	if (fp != NULL) fnameprint("Loading", fname, ind);
	if (fp == NULL)
	{
		// if no newsave in app data load default from program directory
		// can be saved from save button
		fnameprint("Error loading", fname, ind);
		fname = "newsave" + to_string(type);
		fnameprint("Loading default", fname, ind);
		fprintf(stdout, "Can be saved pressing save button or enabling saveonexit\n");
		fp = fopen(fname.c_str(), "rb");
		if (fp == NULL)	{ fnameprint("Error loading default", fname, ind); return; }
	}
	fread(&newstore, sizeof(newstore), 1, fp);
	fclose(fp);
	ind = newstore.ind;
	pivot = false;
	if (sysmod)		{ TwDefine("Rotate/Navmod label='System'");		TwDefine("Translate/Navmod label='System'"); }
	else			{ TwDefine("Rotate/Navmod label='Fractal'");	TwDefine("Translate/Navmod label='Fractal'"); }
	locals = newstore.locals[ind];
	axesmat0[0] = locals.wx;
	axesmat0[1] = locals.wy;
	axesmat0[2] = locals.wz;
	axesmat2[0] = vec3(1.0f, 0.0f, 0.0f);
	axesmat2[1] = vec3(0.0f, 1.0f, 0.0f);
	axesmat2[2] = vec3(0.0f, 0.0f, 1.0f);
	TwDefine((" Menu/Formulae  label='" + typenames[type]).c_str());
	if (type < 7)	{ TwDefine(" Set visible=true ");	TwDefine(" Hybrid visible=false"); }
	else			{ TwDefine(" Set visible=false ");	TwDefine(" Hybrid visible=true"); }	
	
	if (globals.type != 8)
	{
		TwDefine(" Menu/Reflex visible=false ");
		reflex = false;
	}
	else
		TwDefine(" Menu/Reflex visible=true ");

	if (type == 9)
	{
		TwDefine("Hybrid/Maxiter0	label=Boxiter");
		TwDefine("Hybrid/Maxiter1	label=Rotiter");
		TwDefine("Hybrid/Maxiter2	label=Rotiter0");
		TwDefine("Hybrid/b0			label=BoxC");
		TwDefine("Hybrid/b1			label=RotC");
		TwDefine("Hybrid/b2			label=RotC0");
		TwDefine("Hybrid/Foldz		label=V0div");
		TwDefine("Hybrid/KleinR		label=Phase step=1");
		TwDefine("Hybrid/KleinI		label=Order step=1");
		newstore.focus = 4;

	}
	else
	{
		TwDefine("Hybrid/Maxiter0	label=Maxiter0");
		TwDefine("Hybrid/Maxiter1	label=Maxiter1");
		TwDefine("Hybrid/Maxiter2	label=Maxiter2");
		TwDefine("Hybrid/b0			label=b0");
		TwDefine("Hybrid/b1			label=b1");
		TwDefine("Hybrid/b2			label=b2");
		TwDefine("Hybrid/Foldz		label=Foldz");
		TwDefine("Hybrid/KleinR		label=KleinR step=0.01");
		TwDefine("Hybrid/KleinI		label=KleinI step=0.001");
	}

	if (locals.spacemode)	TwDefine("Menu/Space.mode label='Spaceround'");
	else					TwDefine("Menu/Space.mode label='Spacesquare'");
	TwDefine((" Palette/ColCentre label='" + colortypes[locals.ccol]).c_str());
	if (locals.julia)	TwDefine("Set/julia  label='Julia Set' ");
	else				TwDefine("Set/julia  label='Mandel Set' ");
	string labelload = "Menu/Load  label='Preset " + to_string(ind) + " of ";
						TwDefine((labelload + to_string(int(newstore.loaded) - 1) + "'").c_str());

	return;
}
void filesave(int type)
{
	newstore.ind = ind;
	float temp = newstore.focus;
	string fname = savepath + "\\newsave" + to_string(type);
	FILE* fp = fopen(fname.c_str(), "wb");
	if (fp != 0)
	{
		fnameprint("Saving ", fname, ind);
		fwrite(&newstore, sizeof(newstore), 1, fp);
		fclose(fp);
	}
	else
		fnameprint("Error saving ", fname, ind);
	return;
}
void defaultsetup()
{
#define BUFZIZE 1024
	fprintf(stdout, "\n");
	CreateDirectoryA(savepath.c_str(), NULL);
	fnameprint("Creating setup directory (first run?) ", savepath.c_str(), ind);
	string  filedata = savepath + "\\start";
	char buf[BUFZIZE];
	size_t size;
	FILE* source;
	FILE* dest;
	if (fopen_s(&source, "start", "rb") == 0)
	{
		fnameprint("Saving ", "start", ind);
		if (fopen_s(&dest, filedata.c_str(), "wb") == 0)
		{
			fnameprint("    to ", filedata.c_str(), ind);
			while (size = fread(buf, 1, BUFZIZE, source))	fwrite(buf, 1, size, dest);
			fclose(dest);
		}
		else
		{	fnameprint("Error saving ", "start", ind);	return;	}
		fclose(source);
	}
	else
	{	fnameprint("Error opening ", "start", ind);	return;	}
	int num = 0;
	string  fname;
	do
	{
		fname = "save" + to_string(num);
		if (fopen_s(&source, fname.c_str(), "rb") == NULL)
		{
			fnameprint("Saving ", fname, ind);
			filedata = savepath + "\\" + fname;
			fopen_s(&dest, filedata.c_str(), "wb");
			fnameprint("    to ", filedata.c_str(), ind);
			while (size = fread(buf, 1, BUFSIZ, source))	fwrite(buf, 1, size, dest);
			fclose(source);
			fclose(dest);
			num++;
		}
		else
			break;
	} 
	while (true);
	return;
}
void fnameprint(string action, string fname, int num)
{
	TCHAR  buffer[1024] = TEXT("");
	if (GetFullPathName(fname.c_str(), 1024, buffer, NULL) != 0)
		fprintf(stdout, " %s %s\n", action.c_str(), buffer);
	else
		fprintf(stdout, " File not found %s\n", fname.c_str());
	return;
}
///////////////////////////////////////////		Navigation	functions

void TW_CALL Rptr(void* clientData)
{
	defrot();
	return;
}
void TW_CALL Dispaxes(void* clientData)
{
	dispax = !dispax;
	if (dispax) { TwDefine("Rotate/Dis.axes label='Axes On'");		TwDefine("Translate/Dis.axes label='Axes On'"); }
	else		{ TwDefine("Rotate/Dis.axes label='Axes Off'");		TwDefine("Translate/Dis.axes label='Axes Off'"); }
	return;
}
void TW_CALL Navmod(void* clientData)
{
	sysmod = !sysmod;
	if (sysmod)	{TwDefine("Rotate/Navmod label='System'");	TwDefine("Translate/Navmod label='System'");}
	else		{TwDefine("Rotate/Navmod label='Fractal'");	TwDefine("Translate/Navmod label='Fractal'");}
	return;
}
//////////////////////////////////////////		/MENU		functions

void TW_CALL formulabutton(void* clientData)
{
	globals.type = *(int*)(&clientData);
	fileload(globals.type);
	if(globals.autohide) TwDefine("Menu/Formulae opened=false");
	return;
}
void TW_CALL loadbutton(void* clientData)
{
	fileload(globals.type);
	ind = newstore.ind;
	return;
}
void TW_CALL indexget(void* value, void* clientData)
{ *(int*)value			= ind; }
void TW_CALL indexset(const void* value, void* clientData)
{
	if (newstore.loaded == 0)
	{
		ind = -1;
		return;
	}
	else
	{
		ind = (*(int*)value);
		globals.ind = ind;
		if (ind > newstore.loaded - 1)	ind = 0; 
		else if (ind < 0)				ind = newstore.loaded - 1;
		locals = newstore.locals[ind];
		axesmat0[0] = locals.wx;
		axesmat0[1] = locals.wy;
		axesmat0[2] = locals.wz;
		if (locals.julia)		TwDefine("Set/julia  label='Julia Set' ");
		else					TwDefine("Set/julia  label='Mandel Set' ");
		if (locals.spacemode)	TwDefine("Menu/Space.mode label='Spaceround'");
		else					TwDefine("Menu/Space.mode label='Spacesquare'");
	}
	return;
}
void TW_CALL saveset(void* clientData)
{
	//newstore.locals[ind] = locals;
	startsave();
	filesave(globals.type);
	return;
}
void TW_CALL modifyset(void* clientData)
{
	newstore.locals[ind] = locals;
	if (globals.autohide) TwDefine("Menu/Edit opened=false");
	return;
}
void TW_CALL copyset(void* clientData)
{
	newstore.locals[99] = locals;
	TwDefine("Menu/insert label='Insert'");		TwDefine("Menu/paste label='Paste'");
	copied = true;
	return;
}
void TW_CALL pasteset(void* clientData)
{
	if (copied)
	{
		newstore.locals[ind] = newstore.locals[99];
		locals = newstore.locals[99];
		TwDefine("Menu/insert label='null'");	TwDefine("Menu/paste label='null'");
		copied = false;
		if (globals.autohide) TwDefine("Menu/Edit opened=false");
	}
	return;
}
void TW_CALL insertset(void* clientData)
{
	if (copied)
	{
		uint size = (newstore.loaded - ind) * sizeof(locals);
		memcpy(&newstore.locals[ind + 1], &newstore.locals[ind], size);
		newstore.locals[ind] = newstore.locals[99];
		locals = newstore.locals[99];
		newstore.loaded++;
		ind++;
		string labelload = "Menu/Load  label='Load " + to_string(ind) + " of ";
		TwDefine((labelload + to_string(int(newstore.loaded) - 1) + "'").c_str());
		TwDefine("Menu/insert label='null'");	TwDefine("Menu/paste label='null'");
		copied = false;
		if (globals.autohide) TwDefine("Menu/Edit opened=false");
	}
	return;
}
void TW_CALL deleteset(void* clientData)
{
	if (newstore.loaded > ind)
	{
		uint size = (newstore.loaded - ind) * sizeof(locals);
		memcpy(&newstore.locals[ind], &newstore.locals[ind +  1], size);
		locals = newstore.locals[ind];
		axesmat0[0] = locals.wx;
		axesmat0[1] = locals.wy;
		axesmat0[2] = locals.wz;
		newstore.loaded--;
		string labelload = "Menu/Load  label='Load " + to_string(ind) + " of ";
		TwDefine((labelload + to_string(int(newstore.loaded) - 1) + "'").c_str());
		if (globals.autohide) TwDefine("Menu/Edit opened=false");
	}
	return;
}
void TW_CALL resetbutton(void* clientData)
{
	reset(globals.type);
	if (globals.autohide) TwDefine("Menu/Edit opened=false");
	return;
}
void TW_CALL playbutton(void* clientData)
{
	locals.span = 0;
	play = !play;
	if (!play) record = false;
	else		callplay();
	return;
}
void TW_CALL showbutton(void* clientData)
{
	locals.span = 0;
	show = !show;
	if (!show)	record = false;
	else		callshow();
	return;
}
void TW_CALL showget(void* value, void* clientData) 
{ 
	*(int*)value = globals.detected[*(int*)(&clientData)];
}
void TW_CALL showset(const void* value, void* clientData)
{
	globals.detected[*(int*)(&clientData)] = *(int*)value;
	TwDefine((" Menu/Show" + to_string(*(int*)(&clientData)) + " label = '" + typenames[*(int*)value]).c_str());
	return;
}
void TW_CALL recordbutton(void* clientData)
{
	record = !record;
	if (record)
	{	TwDefine("Menu/record  label='Recording' "); }
	else
	{
		TwDefine("Menu/record  label='Record jpg' ");
		play = false;
		show = false;
	}
	return;
}
void TW_CALL printjpgbutton(void* clientData)
{
	snapshot(record, 800, 1024, savepath);
	return;
}
void TW_CALL occget(void* value, void* clientData) 
{ *(float*)value			= locals.occrange; }
void TW_CALL occset(const void* value, void* clientData)
{
	locals.occrange = *(float*)value;
	return;
}
///////////////////////////////////////////		Palette		funcions

void TW_CALL AmbLightget(void* value, void* clientData) { *(float*)value	= locals.AmbLight; }
void TW_CALL AmbLightset(const void* value, void* clientData)
{
	locals.AmbLight = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL ColLightget(void* value, void* clientData) { *(float*)value	= locals.LightCol; }
void TW_CALL ColLightset(const void* value, void* clientData)
{
	locals.LightCol = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL Ldirget(void* value, void* clientData) { *(vec3*)value			= locals.LightDir; }
void TW_CALL Ldirset(const void* value, void* clientData) 
{ 
	locals.LightDir = *(vec3*)value;
	calldisplay();
}
void TW_CALL Emiscolget(void* value, void* clientData) { *(float*)value		= locals.EmisCol; }
void TW_CALL Emiscolset(const void* value, void* clientData)
{
	locals.EmisCol = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL Speclightget(void* value, void* clientData) { *(float*)value	= locals.LightSpc; }
void TW_CALL Speclightset(const void* value, void* clientData)
{
	locals.LightSpc = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL SpecPowget(void* value, void* clientData) { *(float*)value		= locals.SpecPwr; }
void TW_CALL SpecPowset(const void* value, void* clientData)
{
	locals.SpecPwr = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL Col1get(void* value, void* clientData) { *(vec3*)value			= locals.col1; }
void TW_CALL Col1set(const void* value, void* clientData)
{
	locals.col1 = *(vec3*)value;
	calldisplay();
	return;
}
void TW_CALL Col2get(void* value, void* clientData) { *(vec3*)value			= locals.col2; }
void TW_CALL Col2set(const void* value, void* clientData)
{
	locals.col2 = *(vec3*)value;
	calldisplay();
	return;
}
void TW_CALL Fogcget(void* value, void* clientData) { *(vec3*)value			= locals.FogColor; }
void TW_CALL Fogcset(const void* value, void* clientData)
{
	locals.FogColor = *(vec3*)value;
	calldisplay();
	return;
}
void TW_CALL FogEncget(void* value, void* clientData) { *(float*)value		= locals.FogEn; }
void TW_CALL FogEnset(const void* value, void* clientData)
{
	locals.FogEn = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL Colspinget(void* value, void* clientData) { *(float*)value		= locals.sine; }
void TW_CALL Colspinset(const void* value, void* clientData)
{
	locals.sine = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL Spanget(void* value, void* clientData) { *(int*)value			= locals.span; }
void TW_CALL Spanset(const void* value, void* clientData)
{
	locals.span = *(int*)value;
	calldisplay();
	return;
}
void TW_CALL Depthget(void* value, void* clientData) { *(float*)value		= locals.depth; }
void TW_CALL Depthset(const void* value, void* clientData)
{
	locals.depth = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL Focalget(void* value, void* clientData) { *(float*)value		= locals.focal; }
void TW_CALL Focalset(const void* value, void* clientData)
{
	locals.focal = *(float*)value;
	calldisplay();
	return;
}
void TW_CALL colorbutton(void* clientData)
{
	int newcol =  *(int*)(&clientData);
	TwDefine((" Palette/ColCentre label='" + colortypes[newcol]).c_str());
	if (newcol == 2)
	{
		rotpointer = true; 
		TwDefine("Rotate/Rotptr label='Rotpointer'"); 
		TwDefine("Translate/Rotptr label='Rotpointer'");
	}
	oldcol = locals.ccol;
	locals.ccol = newcol;
	if (globals.autohide) TwDefine("Palette/ColCentre opened=false");
	return;
}
void TW_CALL Sinphget(void* value, void* clientData) { *(float*)value		= locals.sincos; }
void TW_CALL Sinphset(const void* value, void* clientData)
{
	locals.sincos = *(float*)value;
	calldisplay();
	return;
}
///////////////////////////////////////////		Parameter set	funcions

void TW_CALL Powerget(void* value, void* clientData) { *(float*)value		= locals.order; }
void TW_CALL Powerset(const void* value, void* clientData)
{
	locals.order = *(float*)value; 
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Phaseget(void* value, void* clientData) { *(float*)value		= locals.phase; }
void TW_CALL Phaseset(const void* value, void* clientData)
{
	locals.phase = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Maxiterget(void* value, void* clientData) { *(float*)value		= locals.maxiter; }
void TW_CALL Maxiterset(const void* value, void* clientData)
{
	locals.maxiter = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Accuracyget(void* value, void* clientData) { *(float*)value	= locals.accuracy; }
void TW_CALL Accuracyset(const void* value, void* clientData)
{
	locals.accuracy = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Deget(void* value, void* clientData) { *(float*)value			= locals.df; }
void TW_CALL Deset(const void* value, void* clientData)
{
	locals.df = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Mrget(void* value, void* clientData) { *(float*)value			= locals.mr; }
void TW_CALL Mrset(const void* value, void* clientData)
{
	locals.mr = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Sphereget(void* value, void* clientData) { *(float*)value		= locals.fr; }
void TW_CALL Sphereset(const void* value, void* clientData)
{
	locals.fr = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL SCget(void* value, void* clientData) { *(float*)value			= locals.sc; }
void TW_CALL SCset(const void* value, void* clientData)
{
	locals.sc = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL juliabutton(void* clientData)
{
	locals.julia = !locals.julia;
	if (locals.julia)	TwDefine("Set/julia  label='Julia Set' ");
	else				TwDefine("Set/julia  label='Mandel Set' ");
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Juliaget(void* value, void* clientData) { *(vec3*)value		= locals.juliav;}
void TW_CALL Juliaset(const void* value, void* clientData)
{
	locals.juliav = *(vec3*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL spacebutton(void* clientData)
{
	locals.spacemode = !locals.spacemode;
	if(locals.spacemode)
		TwDefine("Menu/Space.mode label='Spaceround'");
	else
		TwDefine("Menu/Space.mode label='Spacesquare'");
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Spaceget(void* value, void* clientData) { *(vec3*)value		= locals.space; }
void TW_CALL Spaceset(const void* value, void* clientData)
{
	locals.space = *(vec3*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Roundget(void* value, void* clientData) { *(vec3*)value = locals.centrespace; }
void TW_CALL Roundset(const void* value, void* clientData)
{
	locals.centrespace = *(vec3*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Radiusget(void* value, void* clientData) { *(float*)value = locals.radius; }
void TW_CALL Radiusset(const void* value, void* clientData)
{
	locals.radius = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Reflexget(void* value, void* clientData) { *(bool*)value = reflex; }
void TW_CALL Reflexset(const void* value, void* clientData)
{

	reflex = *(bool*)value;
	if (globals.type != 8)
	{
		reflex = false;
		return;
	}

	if (reflex)
	{
		locals.FogEn = 0.5f;
		recalc(STEPONCE);
		callreflex();
		reflexdisplay();
	}
	else
	{
		locals.FogEn = 0;
		recalc(STEPONCE);
		glFinish();
		glClear(GL_COLOR_BUFFER_BIT);
		display(STEPONCE);
	}
	TwDraw();
	glfwSwapBuffers(mainwindow);
	return;
}
///////////////////////////////////////////		Hybrid	set		funcions

void TW_CALL bailoutget(void* value, void* clientData) { *(float*)value		= locals.bailout; }
void TW_CALL bailoutset(const void* value, void* clientData)
{
	locals.bailout = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL minbailget(void* value, void* clientData) { *(float*)value		= locals.minbail; }
void TW_CALL minbailset(const void* value, void* clientData)
{
	locals.minbail = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);

	return;
}
void TW_CALL bail0get(void* value, void* clientData) { *(float*)value		= locals.bail0; }
void TW_CALL bail0set(const void* value, void* clientData)
{
	locals.bail0 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL dfget(void* value, void* clientData) { *(float*)value			= locals.df; }
void TW_CALL dfset(const void* value, void* clientData)
{
	locals.df = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL miniter0get(void* value, void* clientData) { *(float*)value	= locals.miniter0; }
void TW_CALL miniter0set(const void* value, void* clientData)
{
	locals.miniter0 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL miniter1get(void* value, void* clientData) { *(float*)value	= locals.miniter1; }
void TW_CALL miniter1set(const void* value, void* clientData)
{
	locals.miniter1 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL miniter2get(void* value, void* clientData) { *(float*)value	= locals.miniter2; }
void TW_CALL miniter2set(const void* value, void* clientData)
{
	locals.miniter2 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, NOTW);
	return;
}
//
void TW_CALL maxiterget(void* value, void* clientData) { *(float*)value		= locals.maxiter; }
void TW_CALL maxiterset(const void* value, void* clientData)
{
	locals.maxiter = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL maxiter0get(void* value, void* clientData) { *(float*)value	= locals.maxiter0; }
void TW_CALL maxiter0set(const void* value, void* clientData)
{
	locals.maxiter0 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL maxiter1get(void* value, void* clientData) { *(float*)value	= locals.maxiter1; }
void TW_CALL maxiter1set(const void* value, void* clientData)
{
	locals.maxiter1 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL maxiter2get(void* value, void* clientData) { *(float*)value	= locals.maxiter2; }
void TW_CALL maxiter2set(const void* value, void* clientData)
{
	locals.maxiter2 = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL KleinRget(void* value, void* clientData) { *(float*)value		= locals.KleinR; }
void TW_CALL KleinRset(const void* value, void* clientData)
{
	locals.KleinR = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL KleinIget(void* value, void* clientData) { *(float*)value		= locals.KleinI; }
void TW_CALL KleinIset(const void* value, void* clientData)
{
	locals.KleinI = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL kleinfget(void* value, void* clientData) { *(float*)value		= locals.kleinf; }
void TW_CALL kleinfset(const void* value, void* clientData)
{
	locals.kleinf = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL foldxget(void* value, void* clientData) { *(float*)value		= locals.foldx; }
void TW_CALL foldxset(const void* value, void* clientData)
{
	locals.foldx = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL foldyget(void* value, void* clientData) { *(float*)value		= locals.foldy; }
void TW_CALL foldyset(const void* value, void* clientData)
{
	locals.foldy = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL foldzget(void* value, void* clientData) { *(float*)value		= locals.foldz; }
void TW_CALL foldzset(const void* value, void* clientData)
{
	locals.foldz = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
//
void TW_CALL boxsizexget(void* value, void* clientData) { *(float*)value	= locals.boxsizex; }
void TW_CALL boxsizexset(const void* value, void* clientData)
{
	locals.boxsizex = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL boxsizezget(void* value, void* clientData) { *(float*)value	= locals.boxsizez; }
void TW_CALL boxsizezset(const void* value, void* clientData)
{
	locals.boxsizez = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL losScaleget(void* value, void* clientData) { *(float*)value	= locals.losScale; }
void TW_CALL losScaleset(const void* value, void* clientData)
{
	locals.losScale = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL b0get(void* value, void* clientData) { *(bool*)value = locals.b0; }
void TW_CALL b0set(const void* value, void* clientData)
{
	locals.b0 = *(bool*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL b1get(void* value, void* clientData) { *(bool*)value = locals.b1; }
void TW_CALL b1set(const void* value, void* clientData)
{
	locals.b1 = *(bool*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL b2get(void* value, void* clientData) { *(bool*)value = locals.b2; }
void TW_CALL b2set(const void* value, void* clientData)
{
	locals.b2 = *(bool*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Otrapget(void* value, void* clientData) { *(float*)value		= locals.Otrap; }
void TW_CALL Otrapset(const void* value, void* clientData)
{
	locals.Otrap = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
//////////////////////////////////////////		Debug set	funcions

void TW_CALL zoomget(void* value, void* clientData) { *(float*)value		= locals.zoom; }
void TW_CALL zoomset(const void* value, void* clientData)
{
	locals.zoom = *(float*)value;
	callrecalc(locals.numsteps, NOaxes, TW);
	return;
}
void TW_CALL Pivot(void* clientData)
{
	pivot = !pivot;
	sysmod = true;
	rotpointer = false;
	{	TwDefine("Rotate/Navmod label='System'");		TwDefine("Translate/Navmod label='System'"); }
	if (pivot)
	{	TwDefine("Rotate/Rotptr label='Pivot'");		TwDefine("Translate/Rotptr label='Pivot'"); }
	else
	{	TwDefine("Rotate/Rotptr label='Rotcentre'");	TwDefine("Translate/Rotptr label='Rotcentre'"); }
	return;
}
void TW_CALL Console(void* clientData)
{
	globals.consolehidden = !globals.consolehidden;
	ShowWindow(FindWindow("ConsoleWindowClass", NULL), globals.consolehidden);
	return;
}
///////////////////////////////////////////
void guisetup()
{
	TwStructMember myStructlong[]
	{
		{	"x", TW_TYPE_FLOAT, 0, "step=0.1" },
		{	"y", TW_TYPE_FLOAT, 4, "step=0.1" },
		{	"z", TW_TYPE_FLOAT, 8, "step=0.1" }
	};
	TwType TWvec3 = TwDefineStruct("TWvec3", myStructlong, 3, sizeof(vec3), NULL, NULL);
	TwStructMember myStructlong0[]
	{
		{	"x", TW_TYPE_FLOAT, 0, "step=0.01" },
		{	"y", TW_TYPE_FLOAT, 4, "step=0.01" },
		{	"z", TW_TYPE_FLOAT, 8, "step=0.01" }
	};
	TwType TWvec30 = TwDefineStruct("TWvec30", myStructlong0, 3, sizeof(vec3), NULL, NULL);
	TwStructMember myStructlong00[]
	{
		{	"x", TW_TYPE_FLOAT, 0, "step=0.001" },
		{	"y", TW_TYPE_FLOAT, 4, "step=0.001" },
		{	"z", TW_TYPE_FLOAT, 8, "step=0.001" }
	};
	TwType TWvec300 = TwDefineStruct("TWvec300", myStructlong00, 3, sizeof(vec3), NULL, NULL);
	/////////////////////////////////////////// Rotate parameters
	RotGUI = TwNewBar("Rotate");

	TwDefine(" Rotate refresh=0.00  visible=true color='100 0 0' position='1224 0' size='200 100' ");
	TwAddButton(RotGUI, "Rotptr",		Rptr,			NULL,	"label='Rotpointer'");
	TwAddButton(RotGUI, "Dis.axes",		Dispaxes,		NULL,	NULL);
	TwAddButton(RotGUI, "Navmod",		Navmod,			NULL,	NULL);
	TwAddVarRW(RotGUI, "speed",			TW_TYPE_INT32,	&locals.Rspeed,	"label='Sensitivity' step=1.0	min=-10		max= 5");
	/////////////////////////////////////////// Translate parameters
	TransGUI = TwNewBar("Translate");

	TwDefine(" Translate refresh=0.00 visible=false color='0 100 0' position='1224 0' size='200 100' ");
	TwAddButton(TransGUI, "Rotptr",		Rptr,			NULL,	"label='Rotpointer'");
	TwAddButton(TransGUI, "Dis.axes",	Dispaxes,		NULL,	NULL);
	TwAddButton(TransGUI, "Navmod",		Navmod,			NULL,	NULL);
	TwAddVarRW(TransGUI, "speed",		TW_TYPE_INT32,	&locals.Tspeed,		"label='Sensitivity' step=1.0	min=-10	max= 10");
	/////////////////////////////////////////// Menu
	MenuGUI = TwNewBar("Menu");

	TwDefine(" Menu label=Command refresh=0.00  position='1024 0' color='255 255 255' text=dark	visible=true size='200 380' ");
	TwAddButton(MenuGUI, "Bulb",		formulabutton, (void**)0,	"group=Formulae");
	TwAddButton(MenuGUI, "Bulb2",		formulabutton, (void**)1,	"group=Formulae");
	TwAddButton(MenuGUI, "Bulbs",		formulabutton, (void**)2,	"group=Formulae");
	TwAddButton(MenuGUI, "BulbDE",		formulabutton, (void**)3,	"group=Formulae");
	TwAddButton(MenuGUI, "Bulb2DE",		formulabutton, (void**)4,	"group=Formulae");
	TwAddButton(MenuGUI, "Box",			formulabutton, (void**)5,	"group=Formulae");
	TwAddButton(MenuGUI, "BoxDE",		formulabutton, (void**)6,	"group=Formulae");
	TwAddButton(MenuGUI, "Bspudd",		formulabutton, (void**)7,	"group=Formulae");
	TwAddButton(MenuGUI, "Pseudoklein", formulabutton, (void**)8,	"group=Formulae");
	TwAddButton(MenuGUI, "Hybrid1",		formulabutton, (void**)9,	"group=Formulae");
	TwAddButton(MenuGUI, "reset",		resetbutton,	NULL,		"group=Formulae label = 'Reset'");
	TwDefine("Menu/Formulae opened=false");
	TwAddVarCB(MenuGUI,	 "Preset",		TW_TYPE_INT32,	indexset,		indexget,	NULL,	"step=1 min=-1	max= 21");
	TwAddButton(MenuGUI, "Load",		loadbutton,		NULL,						NULL);
	TwAddButton(MenuGUI, "modify",		modifyset,		NULL,						"label = 'Modify'		group = Preset_Edit");
	TwAddButton(MenuGUI, "copy",		copyset,		NULL,						"label = 'Copy'			group = Preset_Edit");
	TwAddButton(MenuGUI, "paste",		pasteset,		NULL,						"label = 'null'			group = Preset_Edit");
	TwAddButton(MenuGUI, "insert",		insertset,		NULL,						"label = 'null'			group = Preset_Edit");
	TwAddButton(MenuGUI, "delete",		deleteset,		NULL,						"label = 'Delete'		group = Preset_Edit");
	TwAddVarRW(MenuGUI, "frames",		TW_TYPE_INT32,	&locals.frames,				"label = 'Frames'		group = Preset_Edit	step=1		min=0	max= 1000");
	TwDefine("Menu/Preset_Edit opened=false");
	TwAddButton(MenuGUI, "save",		saveset,		NULL,						"label = 'Save'");
	TwAddButton(MenuGUI, "play",		playbutton,		NULL,						"label = 'Play'");
	TwAddButton(MenuGUI, "show",		showbutton,		NULL,						"label = 'Random show'");
	TwAddVarCB(MenuGUI, "Show0",		TW_TYPE_INT32, showset, showget, (void**)0,	"step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show1",		TW_TYPE_INT32, showset, showget, (void**)1,	"step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show2",		TW_TYPE_INT32, showset, showget, (void**)2, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show3",		TW_TYPE_INT32, showset, showget, (void**)3, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show4",		TW_TYPE_INT32, showset, showget, (void**)4, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show5",		TW_TYPE_INT32, showset, showget, (void**)5, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show6",		TW_TYPE_INT32, showset, showget, (void**)6, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show7",		TW_TYPE_INT32, showset, showget, (void**)7, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show8",		TW_TYPE_INT32, showset, showget, (void**)8, "step=1	min=0	max=9 group=Show");
	TwAddVarCB(MenuGUI, "Show9",		TW_TYPE_INT32, showset, showget, (void**)9, "step=1	min=0	max=9 group=Show");
	TwAddVarRW(MenuGUI, "Showcount",	TW_TYPE_INT32,	&globals.detected[11],		"step=1	min=0	max=10 group=Show");
	TwAddVarRW(MenuGUI, "Saturate",		TW_TYPE_BOOLCPP,&globals.saturate,			"group=Show");
	TwAddVarRW(MenuGUI, "Showfog",		TW_TYPE_BOOLCPP,&globals.showfog,			"group=Show");
	TwAddVarRW(MenuGUI, "Showpause",	TW_TYPE_INT32,	&globals.showpause,			"label = 'Show pause'	step=200	min=0	max= 10000	group=Show");
	TwAddVarRW(MenuGUI, "March",		TW_TYPE_INT32,	&globals.march,				"label = 'March'		step=1		min=1	max= 20		group=Show");
	TwDefine("Menu/Show opened=false");
	TwAddVarRW(MenuGUI, "Fade",			TW_TYPE_BOOLCPP,&fader,						"label = 'Fade'");
	TwAddVarRW(MenuGUI, "Fadepause",	TW_TYPE_INT32,	&globals.fadepause,			"label = 'Fade time'	step=1		min=0	max= 100	");
	TwAddButton(MenuGUI, "record",		recordbutton,	NULL,						"label = 'Record jpg'");
	TwAddButton(MenuGUI, "printjpg",	printjpgbutton, NULL,						"label = 'Print jpg'");
	TwAddVarCB(MenuGUI, "Occ.Rng",		TW_TYPE_FLOAT,	occset,			occget,		NULL,	"step=1	 min=0	max= 10 label = 'Occlusion'");
	TwAddVarCB(MenuGUI, "Space.square",	TWvec300,		Spaceset,		Spaceget,	NULL, NULL);
	TwAddVarCB(MenuGUI, "Space.centre",	TWvec300,		Roundset,		Roundget,	NULL, NULL);
	TwAddButton(MenuGUI, "Space.mode",	spacebutton,	NULL,						NULL);
	TwAddVarCB(MenuGUI, "Radius.set",	TW_TYPE_FLOAT,	Radiusset,		Radiusget,	NULL,	"step=0.001		min=0		");
	TwAddVarCB(MenuGUI, "Reflex",		TW_TYPE_BOOLCPP, Reflexset,		Reflexget,	NULL, NULL);
	/////////////////////////////////////////// Palette
	PaletteGUI = TwNewBar("Palette");

	TwDefine(" Palette refresh=0.00  visible=true color='255 255 255' text=dark position='1024 380'  size='200 420'");
	TwAddVarCB(PaletteGUI, "A.Light",	TW_TYPE_FLOAT,	AmbLightset,	AmbLightget,	NULL, "step=0.01	min=0	max= 1");
	TwAddVarCB(PaletteGUI, "Light.C.",	TW_TYPE_FLOAT,	ColLightset,	ColLightget,	NULL, "step=0.01	min=0	max= 1");
	TwAddVarCB(PaletteGUI, "Light.D.",	TWvec3,			Ldirset,		Ldirget,		NULL, NULL);
	TwAddVarCB(PaletteGUI, "Emis.C.",	TW_TYPE_FLOAT,	Emiscolset,		Emiscolget,		NULL, "step=0.01	min=0	max= 1");
	TwAddVarCB(PaletteGUI, "Light.S.",	TW_TYPE_FLOAT,	Speclightset,	Speclightget,	NULL, "step=0.01	min=0	max= 1");
	TwAddVarCB(PaletteGUI, "Pow.S.",	TW_TYPE_FLOAT,	SpecPowset,		SpecPowget,		NULL, "step=1.0		min=0	max= 30");
	TwAddVarCB(PaletteGUI, "Col.1",		TW_TYPE_COLOR3F,Col1set,		Col1get,		NULL, NULL);
	TwAddVarCB(PaletteGUI, "Col.2",		TW_TYPE_COLOR3F,Col2set,		Col2get,		NULL, NULL);
	TwAddVarCB(PaletteGUI, "Fog.C.",	TW_TYPE_COLOR3F,Fogcset,		Fogcget,		NULL, NULL);
	TwAddVarCB(PaletteGUI, "Fog.En.",	TW_TYPE_FLOAT,	FogEnset,		FogEncget,		NULL, "step=0.01	min=0	max= 1");
	int colindex[3]{ 0,1,2 };
	TwAddButton(PaletteGUI, "CenterCol", colorbutton,	(void**)0, "group=ColCentre");
	TwAddButton(PaletteGUI, "EyeCol",	colorbutton,	(void**)1, "group=ColCentre");
	TwAddButton(PaletteGUI, "PointCol", colorbutton,	(void**)2, "group=ColCentre");
	TwDefine("Palette/ColCentre opened=false");
	TwAddVarCB(PaletteGUI, "Sinphase",	TW_TYPE_FLOAT,	Sinphset,		Sinphget,		NULL, "step=0.1		min= 0 max= 6.28");
	TwAddVarCB(PaletteGUI, "Col.Spin",	TW_TYPE_FLOAT,	Colspinset,		Colspinget,		NULL, "step=0.1		min= 0		");
	TwAddVarCB(PaletteGUI, "Filter",	TW_TYPE_INT32,	Spanset,		Spanget,		NULL, "step=1		min= 0	max= 4");
	TwAddVarCB(PaletteGUI, "Focal",		TW_TYPE_FLOAT,	Focalset,		Focalget,		NULL, "step=0.1		min=0	max= 30");
	TwAddVarCB(PaletteGUI, "Depth",		TW_TYPE_FLOAT,	Depthset,		Depthget,		NULL, "step=0.1		min=0	max= 10");
	/////////////////////////////////////////// Setup parameters
	SetGUI = TwNewBar("Set");

	TwDefine(" Set label=Parameters refresh=0.00 visible=true color='255 255 255' text=dark position='1224 100' size='200 460' ");
	TwAddVarCB(SetGUI, "Power",			TW_TYPE_FLOAT,	Powerset, Powerget,			NULL, "step=0.1		min=0		max= 10");
	TwAddVarCB(SetGUI, "Phase",			TW_TYPE_FLOAT,	Phaseset, Phaseget,			NULL, "step=0.1		min=-10		max= 10");
	TwAddVarCB(SetGUI, "Maxiter",		TW_TYPE_FLOAT,	Maxiterset, Maxiterget,		NULL, "step=1		min= 0		max= 13");
	TwAddVarCB(SetGUI, "Accuracy",		TW_TYPE_FLOAT,	Accuracyset, Accuracyget,	NULL, "step=0.001	min=0.002	max= 0.01");
	TwAddVarCB(SetGUI, "De",			TW_TYPE_FLOAT,	Deset, Deget,				NULL, "step=1		min= 0		max= 5");
	TwAddVarCB(SetGUI, "Box",			TW_TYPE_FLOAT,	Mrset, Mrget,				NULL, "step=0.01	min=-100	max= 100");
	TwAddVarCB(SetGUI, "Sphere",		TW_TYPE_FLOAT,	Sphereset, Sphereget,		NULL, "step=0.01	min=-100	max= 100");
	TwAddVarCB(SetGUI, "Scale",			TW_TYPE_FLOAT,	SCset, SCget,				NULL, "step=0.01	min=-100	max= 100");
	TwAddButton(SetGUI, "julia",		juliabutton,								NULL, "label='Mandel Set' ");
	TwAddVarCB(SetGUI, "Julia.set",		TWvec30,		Juliaset, Juliaget,			NULL, NULL);
	/////////////////////////////////////////// Hybrid parameters
	HybridGUI = TwNewBar("Hybrid");

	TwDefine(" Hybrid refresh=0.00  position='1224 100' color='255 255 255' text=dark visible=true size='200 460'");
	TwDefine(" Hybrid label=Hybrid.parm ");
	TwAddVarCB(HybridGUI, "Bailbox",	TW_TYPE_FLOAT,	bailoutset,		bailoutget,		NULL, "step=0.05	min=-10		max=12");
	TwAddVarCB(HybridGUI, "Bailrot",	TW_TYPE_FLOAT,	minbailset,		minbailget,		NULL, "step=0.05	min=-10		max=12");
	TwAddVarCB(HybridGUI, "Bailrot0",	TW_TYPE_FLOAT,	bail0set,		bail0get,		NULL, "step=0.05	min=0		max=1000");
	TwAddVarCB(HybridGUI, "Accuracy",	TW_TYPE_FLOAT,  Accuracyset,	Accuracyget,	NULL, "step=0.001	min=0.002	max= 0.01");
	TwAddVarCB(HybridGUI, "De",			TW_TYPE_FLOAT,	dfset,			dfget,			NULL, "step=1		min= 0		max= 5");
	TwAddVarCB(HybridGUI, "Layer",		TW_TYPE_FLOAT,	miniter0set,	miniter0get,	NULL, "step=1		min=0		max=100");
	TwAddVarCB(HybridGUI, "Box",		TW_TYPE_FLOAT,	Mrset,			Mrget,			NULL, "step=0.01	min=-100	max= 100");
	TwAddVarCB(HybridGUI, "Sphere",		TW_TYPE_FLOAT,	Sphereset,		Sphereget,		NULL, "step=0.01	min=-100	max= 100");
	TwAddVarCB(HybridGUI, "Scale",		TW_TYPE_FLOAT,	SCset,			SCget,			NULL, "step=0.01	min=-100	max= 100");
	TwAddVarCB(HybridGUI, "Maxiter",	TW_TYPE_FLOAT,	maxiterset,		maxiterget,		NULL, "step=1		min=-1		max=13");
	TwAddVarCB(HybridGUI, "Maxiter0",	TW_TYPE_FLOAT,	maxiter0set,	maxiter0get,	NULL, "step=1		min=-1		max=100");
	TwAddVarCB(HybridGUI, "Maxiter1",	TW_TYPE_FLOAT,	maxiter1set,	 maxiter1get,	NULL, "step=1		min=-1		max=100");
	TwAddVarCB(HybridGUI, "Maxiter2",	TW_TYPE_FLOAT,	maxiter2set,	maxiter2get,	NULL, "step=1		min=-1		max=100");
	TwAddVarCB(HybridGUI, "Foldx",		TW_TYPE_FLOAT,	foldxset,		foldxget,		NULL, "step=0.01	min=-100	max=10");
	TwAddVarCB(HybridGUI, "Foldy",		TW_TYPE_FLOAT,	foldyset,		foldyget,		NULL, "step=0.01	min=-100	max=1000");
	TwAddVarCB(HybridGUI, "Foldz",		TW_TYPE_FLOAT,	foldzset,		foldzget,		NULL, "step=0.01	min=-100	max=1000");
	TwAddVarCB(HybridGUI, "KleinR",		TW_TYPE_FLOAT,	KleinRset,		KleinRget,		NULL, "step=0.01	min=-10		max=10");
	TwAddVarCB(HybridGUI, "KleinI",		TW_TYPE_FLOAT,	KleinIset,		KleinIget,		NULL, "step=0.001	min=-10		max=10");
	TwAddVarCB(HybridGUI, "Kleinf",		TW_TYPE_FLOAT,	kleinfset,		kleinfget,		NULL, "step=0.01	min=-10		max=100");
	TwAddVarCB(HybridGUI, "Boxsizex",	TW_TYPE_FLOAT,	boxsizexset,	boxsizexget,	NULL, "step=0.01	min=-10		max=10");
	TwAddVarCB(HybridGUI, "Boxsizez",	TW_TYPE_FLOAT,	boxsizezset,	boxsizezget,	NULL, "step=0.01	min=-10		max=10");
	TwAddVarCB(HybridGUI, "LosScale",	TW_TYPE_FLOAT,	losScaleset,	losScaleget,	NULL, "step=0.01	min=-10		max=10");
	TwAddVarCB(HybridGUI, "b0",			TW_TYPE_BOOLCPP,b0set,			b0get,			NULL, NULL);
	TwAddVarCB(HybridGUI, "b1",			TW_TYPE_BOOLCPP,b1set,			b1get,			NULL, NULL);
	TwAddVarCB(HybridGUI, "b2",			TW_TYPE_BOOLCPP,b2set,			b2get,			NULL, NULL);
	TwAddVarCB(HybridGUI, "Otrap",		TW_TYPE_FLOAT,	Otrapset,		Otrapget,		NULL, "step=0.1	min=0 max= 20");
	TwAddVarCB(HybridGUI, "Julia.set", TWvec30, Juliaset, Juliaget, NULL, NULL);
	/////////////////////////////////////////// Debug parameters
	DBGGUI = TwNewBar("Debug");

	TwDefine(" Debug refresh=0.00 iconifiable=true visible=true color='0 100 0' position='1224 560' size='200 240'");// iconified=true
	TwAddButton(DBGGUI,"Console",		Console,		NULL, NULL);
	TwAddVarCB(DBGGUI, "Zoom",			TW_TYPE_FLOAT,	zoomset,		zoomget,		NULL,	"step=0.01	min=0.01	max= 12");
	TwAddVarRW(DBGGUI, "Numsteps",		TW_TYPE_INT32,	&globals.numsteps,						"step=4		min=4		max= 8");
	TwAddButton(DBGGUI,"Rotpointer",	Rptr,			NULL, NULL);
	TwAddVarRW(DBGGUI, "mousex",		TW_TYPE_FLOAT,	&locals.centerrot.x,	NULL);
	TwAddVarRW(DBGGUI, "mousey",		TW_TYPE_FLOAT, &locals.centerrot.y,	NULL);
	TwAddVarRW(DBGGUI, "counter", 		TW_TYPE_FLOAT, &locals.centerrot.z, 		NULL);
	TwAddVarRW(DBGGUI, "rotang",		TW_TYPE_FLOAT,	&dispang,		NULL);
	TwAddVarRW(DBGGUI, "calctime",		TW_TYPE_FLOAT,  &calctime,		NULL);
	TwAddVarRW(DBGGUI, "disptime",		TW_TYPE_FLOAT,  &disptime,		NULL);
	TwAddButton(DBGGUI,"Pivot",			Pivot,			NULL, NULL);
	TwAddVarRW(DBGGUI, "Autohide",		TW_TYPE_BOOLCPP,&globals.autohide,		"label = 'Hideformulae' ");
	TwAddVarRW(DBGGUI, "Savesetup",		TW_TYPE_BOOLCPP, &globals.savesetup,	"label = 'Saveonexit' ");

	return;
}


//void qmul(vec4& q1, vec4& q2, vec4& ret)
//{
//	vec3 q2_ = vec3(q2.x, q2.y, q2.z); vec3 q1_ = vec3(q1.x, q1.y, q1.z);
//	ret = vec4(q2_ * q1.w + q1_ * q2.w + cross(q1_, q2_), q1.w * q2.w - dot(q1_, q2_));
//}
//void rotate_vector(vec3& v, vec4& r, vec3& rot)
//{
//	vec4 r_c = r * vec4(-1, -1, -1, 1);
//	vec4 ret_, ret;
//	qmul(vec4(v, 0), r_c, ret_);
//	qmul(r, ret_, ret);
//	rot = vec3(ret.x, ret.y, ret.z);
//}
	//qinc = angleAxis(callrotang(oldpos), axesmat[ds.axe]);	
	//qinc_ = angleAxis(callrotang_(oldpos), -axesmat2[ds.axe]);
	//mat4 mat1 = toMat4(qtot);
	//mat4 mat2 = toMat4(qinc);
	//qtot = toQuat(mat2 * mat1);
	//float temp = qtot.x * qtot.x + qtot.y * qtot.y + qtot.z * qtot.z + qtot.w * qtot.w;
	//inv.w = qtot.w/temp; inv.x = -qtot.x/temp; inv.y= -qtot.y/temp; inv.z = -qtot.z/temp;
	//axesvertices[0] = vec4(-2.0f * (inv * axesmat1[0]), 1);	axesvertices[1] = vec4(2.0f * (inv * axesmat1[0]), 1);
	//axesvertices[2] = vec4(-2.0f * (inv * axesmat1[1]), 2);	axesvertices[3] = vec4(2.0f * (inv * axesmat1[1]), 2);
	//axesvertices[4] = vec4(-2.0f * (inv * axesmat1[2]), 3);	axesvertices[5] = vec4(2.0f * (inv * axesmat1[2]), 3);
	//callrotang(oldpos, axesmat[ds.axe]);