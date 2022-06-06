#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <windows.h>

#include <AntTweakBar.h>
//#define GLFW_DLL

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/keyinput.hpp>


GLuint Compile(const char*);
GLuint framebuffer, setupbuffer;
struct Setup {
	float colspin;	float colmode;	float maxiter;		float zoom;
	double centrey;					double centrex;
	double juliax;					double juliay;	
	int julia;		int polar;		float power;			float phase;	
} setup = { 10,		0,		512,	1, 
			0,				0, 
			0.3,			0.45, 
			0,		1,		2,		2}; 
Setup oldsetup = setup;
vec3 col1 = vec3(0, 1, 0);
vec3 col2 = vec3(1, 0, 0);
vec3 oldcol1 = col1;
vec3 oldcol2 = col2;
std::vector<glm::vec3>	frame_vertices;
GLuint FramebufferName = 0;
void reset();
void recalc(bool);
void trajectory();
void display();
void grid();
void programview(char*, const char*);
void programview1(char*, const char*);
void guisetup();

float x, y, z;
bool back = false, go = true, printj = false, resetflag = false;
float Maxiter = 4096.0f, Colspin = 3.0f;
float Colmode = 0;
GLint status_;
GLuint vertexposID, maxiterID, maxiter_ID, colspinID, colmodeID, col1ID, col2ID;
GLuint centrex_ID, centrey_ID, span_ID;
GLuint juliaxID, juliayID, juliaID;
GLint ViewshaderID, gridID;
GLint Viewshader1ID, frameposID, zoomdispID, mposdispID, MVPID, MVPID1;
GLuint ProgID;
GLuint Positionbuffer, Centrebuffer, Coordbuffer;

/////////////////////////////
vec4 mpos;
dvec2 fracpos;
float mposx, mposy, zoom = 0.5f;
int Posx = 512, Posy = 384;
int oldposx = 512, oldposy = 384;
int wheel = 4, xpos, ypos,	oldwheel = 1;

bool gorecalc = true, recalcstay = false;
bool godisplay = true, gogrid = true;
bool gotraj = false, gosnap = false;
int eventcount = 0, count = 0;
int fracwidth = 2048, fracheight = 1536;
int dispwidth = 1024, dispheight = 768;
bool finder = true;

bool LEFT_PRESSED = false, RIGHT_PRESSED = false, MIDDLE_PRESSED = false, RIGHT_ARM = true, dontrefresh = false;

struct instatus
{
	bool	leftpressed;	bool	rightpressed;	int		wheel = 2;	int		xpos, ypos, centrex, centrey;
	dvec2	fracpos;		vec4	mpos;			float	zoomdisp = 0.5f;	int		maxiter;
	float	colspin;	float	colmode;
};
instatus currstatus;
GLFWwindow *mainwindow;
HWND console = FindWindow("ConsoleWindowClass", NULL);
const char* szPath = getenv("APPDATA");
std::string savepath;

void MouseButtonCB(GLFWwindow* ,int ,int, int);
void MousePosCB(GLFWwindow* ,double ,double);
void MouseWheelCB(GLFWwindow*, double, double);
int main(void)
{
	// Initialise GLFW
	if (!glfwInit()) { fprintf(stderr, "Failed to initialize GLFW\n"); return -1; }
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//ShowWindow(console, SW_SHOW);
	ShowWindow(console, SW_HIDE);
	//SetWindowPos(console, 0, 500, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Open a window and create its OpenGL context
	mainwindow = glfwCreateWindow(dispwidth + 200, dispheight, "Main", NULL, NULL);
	if (mainwindow == NULL) { fprintf(stderr, "Failed to open GLFW window"); glfwTerminate(); return -1; }
	glfwSetWindowPos(mainwindow, 250, 30);
	glfwSetWindowTitle(mainwindow, "Main");
	glfwMakeContextCurrent(mainwindow);
	glViewport(0, 0, dispwidth, dispheight);
	glfwSetMouseButtonCallback(mainwindow, MouseButtonCB);
	glfwSetCursorPosCallback(mainwindow, MousePosCB);
	glfwSetScrollCallback(mainwindow, MouseWheelCB);

	if (TwInit(TW_OPENGL_CORE, NULL) == NULL) { fprintf(stdout, "Failed to initialize Tw\n");  glfwTerminate(); glfwDestroyWindow(mainwindow); return -1; }
	TwWindowSize(dispwidth + 200, 768);
	guisetup();

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) { fprintf(stderr, "Failed to initialize GLEW\n"); return -1; }
	glfwSwapInterval(0);

	glfwSetCursorPos(mainwindow, dispwidth / 2, dispheight / 2);
	ProgID = Compile("Bulb.shader");
	programview("vertex.shader", "fragment.shader");
	programview1("vertex1.shader", "fragment1.shader");
	// ---------------------------------------------
	glGenBuffers(1, &Positionbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, fracwidth * fracheight * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, Positionbuffer);
// --------------------
	for (int i = 0; i < 1000; i++) frame_vertices.push_back(vec3(0, 0, 0));
	glGenBuffers(1, &framebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, framebuffer);
	glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	frame_vertices[0] = (vec3(-1.024f, -0.768f, 256));
	frame_vertices[1] = (vec3(+1.024f, -0.768f, 256));
	frame_vertices[2] = (vec3(+1.024f, +0.768f, 256));
	frame_vertices[3] = (vec3(-1.024f, +0.768f, 256));
	frame_vertices[4] = (vec3(-1.024f, -0.768f, 256));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
// --------------------
	glGenBuffers(1, &setupbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, setupbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(setup), &setup, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, setupbuffer);
// --------------------
	glGenBuffers(1, &Centrebuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Centrebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, fracwidth * fracheight * sizeof(vec2), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, Centrebuffer);
// --------------------
	glGenBuffers(1, &Coordbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Coordbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, fracwidth * fracheight * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Coordbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, Coordbuffer);
	glBindBufferBase(GL_ARRAY_BUFFER, 0, Coordbuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);

	savepath = szPath;
	DWORD temp = GetFileAttributesA(savepath.c_str());
	savepath.append("\\Mandelbrot");
	printf(" savepath %s\n", savepath.c_str());
	temp = GetFileAttributesA(savepath.c_str());  //temp = 16 if directory exists
	if (temp != 16)
	{
		printf(" Creating directory %s\n", savepath.c_str());
		CreateDirectoryA(savepath.c_str(), NULL);
	}
	void input();
	do
	{
		input();
		if (!dontrefresh)
		{
			if (gorecalc)
				recalc(recalcstay);
			if (godisplay)
				display();
			if (gogrid)
				grid();
		}
		if (gosnap)
		{
			display();
			snapshot(false, dispheight, dispwidth, savepath);
			printj = false;
			gosnap = false;
		}
		if (gotraj)
		{
			display();
			trajectory();
		}
		gorecalc = false; recalcstay = false;
		godisplay = false; gogrid = false;

		TwDraw();
		glfwSwapBuffers(mainwindow);

		glfwWaitEvents();
		Sleep((DWORD)20.0);
	} while ((glfwGetKey(mainwindow, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(mainwindow) == 0) );

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDeleteBuffers(1, &framebuffer);
	glDeleteBuffers(1, &Positionbuffer);
	glDeleteBuffers(1, &setupbuffer);
	glDeleteBuffers(1, &Centrebuffer);
	glDeleteBuffers(1, &Coordbuffer);
	glDeleteFramebuffers(1, &FramebufferName);
	glDeleteProgram(ProgID);
	glDeleteProgram(ViewshaderID);
	glDeleteProgram(Viewshader1ID);
	TwTerminate();
	glfwDestroyWindow(mainwindow);
	glfwTerminate();
	return 0;
}

void recalc(bool stay)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, setupbuffer);
	if (!stay)
	{
		setup.centrex = fracpos.x;
		setup.centrey = fracpos.y;
	}
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(setup), &setup, GL_STATIC_DRAW);
	glUseProgram(ProgID);
	glDispatchCompute(64, 64, 1);
	back = false;
}

void trajectory()
{
	int	iteration;
	double lv0 = 0;
	dvec2 v0 = dvec2(fracpos.x, fracpos.y);
	dvec2 vc = v0;
	if (setup.julia)
		vc = dvec2(setup.juliax, setup.juliay);
	else
		vc = v0;
	if (!setup.polar)
	{
		for (iteration = 0; iteration < setup.maxiter; iteration += 1)
		{
			frame_vertices[iteration + 5] = (1.0f / setup.zoom) * vec3(v0.x - setup.centrex, v0.y - setup.centrey, min(2.0f, iteration / 10.0f)) / 2.0f;
			double x = v0.x;
			v0.x = v0.x * v0.x - v0.y * v0.y + vc.x;
			v0.y = 2.0f * x * v0.y + vc.y;
			lv0 = (v0.x * v0.x + v0.y * v0.y);
		}
	}
	else
	{
		lv0 = length(v0);
		for (iteration = 0; iteration < setup.maxiter; iteration += 1)
		{
			frame_vertices[iteration + 5] = (1.0f / setup.zoom) * vec3(v0.x - setup.centrex, v0.y - setup.centrey, min(2.0f, iteration / 10.0f)) / 2.0f;

			float theta = setup.phase * atan(v0.y, v0.x);
			v0 = (float)pow(lv0, setup.power) * vec2(cos(theta), sin(theta)) + (vec2)vc;
			lv0 = length(v0);
		}

	}
	glUseProgram(Viewshader1ID);
	glm::mat4 projectionMatrix = glm::ortho(-1024.0f, 1024.0f, 768.0f, -768.0f);
	glUniform1f(zoomdispID, 1.0f);
	glUniform4f(mposdispID, 0, 0, 0, 0);
	glUniformMatrix4fv(MVPID1, 1, GL_FALSE, &projectionMatrix[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, framebuffer);
	glBufferData(GL_ARRAY_BUFFER, frame_vertices.size() * sizeof(glm::vec3), &frame_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINE_STRIP, 5, iteration);
	glDisableVertexAttribArray(1);
	gotraj = false;
}

void display()
{
	glfwMakeContextCurrent(mainwindow);
	glPointSize(1);
	glm::mat4 projectionMatrix = glm::ortho(-1024.0f, 1024.0f, 768.0f, -768.0f);
	glUseProgram(ViewshaderID);
	glBindBuffer(GL_ARRAY_BUFFER, Coordbuffer);
	glUniform1f(maxiter_ID, setup.maxiter);
	glUniform1f(colspinID, setup.colspin);
	glUniform1f(colmodeID, setup.colmode);
	glUniform3f(col1ID, col1.x, col1.y, col1.z);
	glUniform3f(col2ID, col2.x, col2.y, col2.z);
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &projectionMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);
	glDrawArrays(GL_POINTS, 0, 4 * 1024 * 768);
	glDisableVertexAttribArray(2);
	godisplay = false;
}

void grid()
{
	if (!finder) return;
	glUseProgram(Viewshader1ID);
	glm::mat4 projectionMatrix = glm::ortho(-1024.0f, 1024.0f, 768.0f, -768.0f);
	glUniform1f(zoomdispID, zoom);
	glUniform4f(mposdispID, mpos.x, mpos.y, 0, 0);
	glUniformMatrix4fv(MVPID1, 1, GL_FALSE, &projectionMatrix[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, framebuffer);
	glBufferData(GL_ARRAY_BUFFER, frame_vertices.size() * sizeof(glm::vec3), &frame_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINE_STRIP, 0, 5);
	glDisableVertexAttribArray(1);
	gogrid = false;
}

void TW_CALL Reset(void* clientData) 
{
	reset();
	gorecalc = true; recalcstay = true;
	godisplay = true;
	return;
}

void TW_CALL Print(void* clientData) 
{ 
	gosnap = true;
}

void TW_CALL Finder(void* clientData)
{
	finder = !finder;
	gogrid = true;
	godisplay = true;
}

float resize = dispwidth / 1024.0f;
void TW_CALL Rsize(void* clientData)
{
	if (resize == 1.0f) 
	{
		resize = 1050.0f / 768.0f;
		TwDefine(" Pallette position='1400 50'  size='200 550'");
		TwDefine(" debug position='1400 600'  size='200 150'");
		glfwSetWindowPos(mainwindow, 150, 30);
	}
	else
	{
		resize = 1;

		TwDefine(" Pallette position='1025 10'  size='200 550'");
		TwDefine(" debug position='1025 600'  size='200 150'");
		glfwSetWindowPos(mainwindow, 250, 30);
	}

	dispwidth = 1024 * resize;
	dispheight = 768 * resize;
	glfwSetWindowSize(mainwindow, dispwidth + 200, dispheight);
	glViewport(0, 0, dispwidth, dispheight);
	TwWindowSize(dispwidth + 200, dispheight);

	recalc(true);
	display();
	TwDraw();
	glfwSwapBuffers(mainwindow);
	return;
}

void TW_CALL Console(void* clientData)
{
	static bool consolehidden = true;
	if (consolehidden)
		ShowWindow(console, SW_SHOW); // shows the window
	else
		ShowWindow(console, SW_HIDE); // hides the window
	consolehidden = !consolehidden;
	return;
}

void MousePosCB(GLFWwindow* auxwindow, double x, double y) {
	xpos = (int)(x / resize); ypos = (int)(y / resize);
	TwEventMousePosGLFW(mainwindow, x, y);
}

void MouseWheelCB(GLFWwindow* window, double xoffset, double yoffset) {
	wheel += (int)yoffset;
	if (wheel < 1) wheel = 1;
	godisplay = true;
	gogrid = true;
}

void MouseButtonCB(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(window, button, action, mods); //tweakbar call
	if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS)) {	dontrefresh = true;  LEFT_PRESSED = true; return; }
	if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_RELEASE))	LEFT_PRESSED = false;
	if ((button == GLFW_MOUSE_BUTTON_MIDDLE) && (action == GLFW_PRESS))		MIDDLE_PRESSED = true;
	if ((button == GLFW_MOUSE_BUTTON_MIDDLE) && (action == GLFW_RELEASE))	MIDDLE_PRESSED = false;
	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {		if (RIGHT_ARM)	RIGHT_PRESSED = true;	RIGHT_ARM = false;}
	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE)) {	RIGHT_PRESSED = false;	RIGHT_ARM = true;}
	if (action == GLFW_PRESS) dontrefresh = false;
	return;
}

void KeybCB(GLFWwindow* window, int key, int scancode, int action, int mod)/*computedata.camera - (crot - computedata.centerpos);*/
{
	if (action == GLFW_RELEASE)
		TwKeyPressed(key, TW_KMOD_NONE);
	if ((glfwGetKey(mainwindow, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) { setup.maxiter += 512; return; }
	if ((glfwGetKey(mainwindow, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) { setup.maxiter -= 512; return; }
	if ((glfwGetKey(mainwindow, GLFW_KEY_HOME) == GLFW_PRESS)) { setup.maxiter = 512; return; }

	return;
}
void TW_CALL maxiterget(void* value, void* clientData) { *(float*)value = setup.maxiter; }
void TW_CALL maxiterset(const void* value, void* clientData)
{
	setup.maxiter = *(float*)value;
	gorecalc = true;
	recalcstay = true;
	godisplay = true;
	return;
}
void TW_CALL colspinget(void* value, void* clientData) { *(float*)value = setup.colspin; }
void TW_CALL colspinset(const void* value, void* clientData)
{
	setup.colspin = *(float*)value;
	godisplay = true;
	return;
}
void TW_CALL colmodeget(void* value, void* clientData) { *(float*)value = setup.colmode; }
void TW_CALL colmodeset(const void* value, void* clientData)
{
	setup.colmode = *(float*)value;
	godisplay = true;
	return;
}
void TW_CALL col1get(void* value, void* clientData) { *(vec3*)value = col1; }
void TW_CALL col1set(const void* value, void* clientData)
{
	col1 = *(vec3*)value;
	godisplay = true;
	return;
}
void TW_CALL col2get(void* value, void* clientData) { *(vec3*)value = col2; }
void TW_CALL col2set(const void* value, void* clientData)
{
	col2 = *(vec3*)value;
	godisplay = true;
	return;
}
void TW_CALL juliaget(void* value, void* clientData) { *(bool*)value = setup.julia; }
void TW_CALL juliaset(const void* value, void* clientData)
{
	setup.julia = *(bool*)value;
	gorecalc = true;
	recalcstay = true;
	godisplay = true;
	gogrid = true;
	return;
}
void TW_CALL juliaxget(void* value, void* clientData) { *(double*)value = setup.juliax; }
void TW_CALL juliaxset(const void* value, void* clientData)
{
	setup.juliax = *(double*)value;
	gorecalc = true;
	recalcstay = true;
	godisplay = true;
	gogrid = true;
	return;
}
void TW_CALL juliayget(void* value, void* clientData) { *(double*)value = setup.juliay; }
void TW_CALL juliayset(const void* value, void* clientData)
{
	setup.juliay = *(double*)value;
	gorecalc = true;
	recalcstay = true;
	godisplay = true;
	gogrid = true;
	return;
}
void TW_CALL powerget(void* value, void* clientData) { *(float*)value = setup.power; }
void TW_CALL powerset(const void* value, void* clientData)
{
	setup.power = *(float*)value;
	gorecalc = true;
	recalcstay = true;
	godisplay = true;
	gogrid = true;
	return;
}
void TW_CALL phaseget(void* value, void* clientData) { *(float*)value = setup.phase; }
void TW_CALL phaseset(const void* value, void* clientData)
{
	setup.phase = *(float*)value;
	gorecalc = true;
	godisplay = true;
	recalcstay = true;
	gogrid = true;
	return;
}
void guisetup()
{
	//////////////////// Pallette
	TwBar* PalletteGUI = TwNewBar("Pallette");
	TwDefine(" Pallette refresh=0.005  visible=false color='128 128 0' position='1025 10'  size='200 550'");
	TwAddVarCB(PalletteGUI, "Maxiter",	TW_TYPE_FLOAT,		maxiterset,	maxiterget, NULL, "step=8	min=6.0 max= 8192.0");
	TwAddVarCB(PalletteGUI, "Col Spin", TW_TYPE_FLOAT,		colspinset,	colspinget, NULL, "step=1.0");
	TwAddVarCB(PalletteGUI, "Col Mode", TW_TYPE_FLOAT,		colmodeset,	colmodeget,	NULL, "step=1.0	min=0.0 max= 2.0");
	TwAddVarCB(PalletteGUI, "Col.1",	TW_TYPE_COLOR3F,	col1set,	col1get,	NULL, NULL);
	TwAddVarCB(PalletteGUI, "Col.2",	TW_TYPE_COLOR3F,	col2set,	col2get,	NULL, NULL);
	TwAddVarRW(PalletteGUI, "Col.2",	TW_TYPE_COLOR3F,	&col2, NULL);
	TwAddButton(PalletteGUI, "Reset",	Reset, NULL, NULL);
	TwAddButton(PalletteGUI, "Print",	Print, NULL, NULL);
	TwAddButton(PalletteGUI, "Size",	Rsize, NULL, NULL);
	TwAddButton(PalletteGUI, "Finder",	Finder, NULL, NULL);
	TwAddVarCB(PalletteGUI, "Juliaset", TW_TYPE_BOOL32,		juliaset,	juliaget,	NULL, NULL);
	TwAddVarCB(PalletteGUI, "juliax",	TW_TYPE_DOUBLE,		juliaxset, juliaxget,	NULL, "step=0.001	min=0.0 max= 2.0");
	TwAddVarCB(PalletteGUI, "juliay",	TW_TYPE_DOUBLE,		juliayset, juliayget,	NULL, "step=0.001	min=0.0 max= 2.0");
	TwAddVarRW(PalletteGUI, "Polar", TW_TYPE_BOOL32, &setup.polar, NULL);
	TwAddVarCB(PalletteGUI, "Power", TW_TYPE_FLOAT, powerset, powerget, NULL, "step=0.1	min=0.0 max= 8.0");
	TwAddVarCB(PalletteGUI, "Phase", TW_TYPE_FLOAT, phaseset, phaseget, NULL, "step=0.1	min=0.0 max= 8.0");
	TwDefine(" Pallette visible=true");
	//////////////////// Navigation
	TwBar* debugGUI = TwNewBar("debug");
	TwDefine(" debug refresh=0.005  visible=false color='0 128 0' position='1025 600'  size='200 150'");
	TwAddVarRW(debugGUI, "mousex", TW_TYPE_INT32, &xpos, "step=1");
	TwAddVarRW(debugGUI, "mousey", TW_TYPE_INT32, &ypos, "step=1");
	TwAddVarRW(debugGUI, "fract.x", TW_TYPE_DOUBLE, &fracpos.x, "step=0.00000001");
	TwAddVarRW(debugGUI, "fract.y", TW_TYPE_DOUBLE, &fracpos.y, "step=0.00000001");
	TwAddVarRW(debugGUI, "zoom", TW_TYPE_FLOAT, &zoom, "step=0.0001");
	TwAddButton(debugGUI, "Console", Console, NULL, NULL);
	TwDefine(" debug visible=true");
}

void input()
{
	currstatus.leftpressed = LEFT_PRESSED;
	currstatus.rightpressed = RIGHT_PRESSED;
	//Aggiorna cursori
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 2 * (fracwidth * ypos * sizeof(fracpos) + xpos * sizeof(fracpos)), sizeof(fracpos), &fracpos);
	//get coord from fractal
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Coordbuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 2 * (fracwidth * ypos * sizeof(mpos) + xpos * sizeof(mpos)), sizeof(mpos), &mpos);

	currstatus.maxiter = setup.maxiter;
	currstatus.colspin = setup.colspin;
	currstatus.colmode = setup.colmode;
	if (xpos < 1024.0f)
	{
		if (!MIDDLE_PRESSED) 
		{
			currstatus.wheel = wheel;
			currstatus.zoomdisp = 2.0f / (float)wheel;
		}
		else 
		{
			wheel = 2;
			currstatus.wheel = 2;
			currstatus.zoomdisp = 1;
		}
		zoom = currstatus.zoomdisp;

		if (LEFT_PRESSED) 
		{
			gotraj = true;
			return;
		}

		if (RIGHT_PRESSED) 
		{
			setup.zoom *= zoom;
			gorecalc = true; recalcstay = false;
			godisplay = true; gogrid = true;
			RIGHT_PRESSED = false;
			RIGHT_ARM = false;
			return;
		}
		godisplay = true; 
		gogrid = true;
	}
}

void reset()
{
	zoom = 0.5f;
	wheel = 4;
	Posx = 512; Posy = 384;
	glfwSetCursorPos(mainwindow, Posx, Posy);
	mpos.x = 0;	mpos.y = 0;
	fracpos.x = 0; fracpos.y = 0;
	currstatus.fracpos = dvec2(0, 0);
	setup.centrex = mpos.x;
	setup.centrey = mpos.y;
	setup.maxiter = 512;
	setup.zoom = 1.0f;
	setup.power = 2;
	setup.phase = 2;
	setup.julia = false;
	setup.juliax = 0.300;
	setup.juliay = 0.450;
	resetflag = false;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, setupbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(setup), &setup, GL_STATIC_DRAW);
	glfwSetCursorPos(mainwindow, 512, 384);
	gorecalc = true;
	godisplay = true;
	recalcstay = true;
}
GLuint Compile(const char* compute_file_path) {
	GLint status;
	GLuint retval = LoadComputes(compute_file_path);
	glGetProgramiv(retval, GL_LINK_STATUS, &status);
	juliaxID = glGetUniformLocation(retval, "juliax");
	juliayID = glGetUniformLocation(retval, "juliay");
	juliaID = glGetUniformLocation(retval, "julia");
	return retval;
}

void programview(char* vertex_file_path, const char* fragment_file_path)
{
	GLint status_;
	ViewshaderID = LoadShaders(vertex_file_path, fragment_file_path);
	glLinkProgram(ViewshaderID);
	glGetProgramiv(ViewshaderID, GL_LINK_STATUS, &status_);
	MVPID = glGetUniformLocation(ViewshaderID, "MVP");
	maxiter_ID = glGetUniformLocation(ViewshaderID, "maxiter");
	colspinID = glGetUniformLocation(ViewshaderID, "colspin");
	colmodeID = glGetUniformLocation(ViewshaderID, "colmode");
	col1ID = glGetUniformLocation(ViewshaderID, "col1");
	col2ID = glGetUniformLocation(ViewshaderID, "col2");
}

void programview1(char* vertex_file_path, const char* fragment_file_path)
{
	GLint status_;
	Viewshader1ID = LoadShaders(vertex_file_path, fragment_file_path);
	glLinkProgram(Viewshader1ID);
	glGetProgramiv(Viewshader1ID, GL_LINK_STATUS, &status_);
	MVPID1 = glGetUniformLocation(ViewshaderID, "MVP");
	frameposID = glGetAttribLocation(Viewshader1ID, "framepos");
	zoomdispID = glGetUniformLocation(Viewshader1ID, "zoomdisp");
	mposdispID = glGetUniformLocation(Viewshader1ID, "mpos");
}