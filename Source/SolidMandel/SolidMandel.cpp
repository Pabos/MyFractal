
#include <sstream>   // std::cout
#include <iostream>   // std::cout
#include <fstream>
#include <vector>
#include <windows.h>
#include <time.h>

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
#include <AntTweakBar.h>
 
#include <common/shader.hpp>
#include <common/keyinput.hpp>
#include <iostream>
using namespace std;

GLuint Compile(const char *);
GLuint programview(char * , const char *);
void guisetup();
void recalc(bool);
void display();
int type		= 0;
struct Computestructure
{ 
	float order;	float xpoints;	float cam;		int f2; 
	float maxd;		float mind;		int maxite;		float dum1;
};
Computestructure computestructure= { 2.0f,	1.0f,	0,	0, -200, 300, 8 ,0 };		

////////////////
GLuint Prog[8], ViewshaderID;
GLuint MVPID, MROTID, maxdID, mindID, alightID, acolorID, ecolorID, scolorID, slightID, spowerID, sineID;
float alight = 0.5f;
float acolor = 0.5f;
float ecolor = 0.5f;
float spower = 16;
float slight = 0.4f;
float sine = 10;
float order = 8;
int iteration = 4;

bool running = true;
bool LEFT_PRESSED = false, RIGHT_PRESSED = false, MIDDLE_PRESSED = false;
GLuint  Computebuffer, Framebuffer, Normalbuffer;

float xmul	= 1;
int sw		= 0;
int cols	= 32 * xmul;			//numx	
int rows	= cols;			//numy
int sizex	= 32;
int sizey	= 20;
float camrange = 1.024f + 0.510f;
glm::mat4 MVP;
glm::mat4 RotationMatrix;
glm::mat4 TranslationMatrix, View, Projection;
float r0, r1, r2;
float psize = 1.0f;
bool printj = false;
GLFWwindow* mainwindow;
int wheel, xmpos, ympos;
///////////////////////////////////////////////////////////////////////////
int main( void )
{
// Initialise GLFW
	if (!glfwInit()) { fprintf(stderr, "Failed to initialize GLFW\n"); return -1; }

	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	mainwindow = glfwCreateWindow(1024, 780, "Main", NULL, NULL);
	if (mainwindow == NULL) { fprintf(stderr, "Failed to open GLFW window"); glfwTerminate(); return -1; }
	glfwMakeContextCurrent(mainwindow);

	glfwSwapInterval(0);
	glViewport(0, 0, 1024, 780);
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) { fprintf(stderr, "Failed to initialize GLEW\n"); return -1; }
	//// Initialize the GUI
	int twret = TwInit(TW_OPENGL_CORE, NULL);
	twret = TwWindowSize(1024, 780);
	guisetup();
/////////////////////////////////////////
	Prog[0]			= Compile("Bulb.vertexshader"); 
	ViewshaderID	= programview("View.vertexshader", "View.fragmentshader");
/////////////////////////////////////////////
	glGenBuffers(1, &Computebuffer);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, Computebuffer );
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(computestructure), &computestructure, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,  3,Computebuffer);
/////////////////////////////////////////////
    glGenBuffers(1, &Framebuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, Framebuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 32 * 1024*780*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Framebuffer);

///////////////////////////////////////////
    glGenBuffers(1, &Normalbuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, Normalbuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 32 * 1024*780*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, Normalbuffer);

	float field = 0.35f;
	Projection	= glm::perspective(field, 1024.0f / 780.0f, 0.1f, 12.0f);
	View		= glm::lookAt(glm::vec3(0,0,8), glm::vec3(0,0,0), glm::vec3(0,1,0)); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
///////////////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	string savepath = getenv("APPDATA") + string("\\Fractal");
	do
	{
		int framecount = 0;
		cols = 32 * xmul;
		rows = cols;
		sizex = 32;
		sizey = 24;
		computestructure.xpoints = (float)(cols*sizex);
		computestructure.f2 = 0;
		computestructure.cam = camrange;
		computestructure.maxd = -200;
		computestructure.mind = 200;
		computestructure.order = order;
		computestructure.maxite = iteration;
		float caminc = 4.096f/(cols*sizex);
		glBindBuffer( GL_SHADER_STORAGE_BUFFER, Computebuffer );
		double time = glfwGetTime(); 
		r0 = 2.14f;
		r1 = 0;
		r2 = 0;
		do
		{	 
			framecount++;
			r0 -= 0.005f;
			r1 += 0.001f;
			//r2 += 0.002f;
			computestructure.cam -= caminc;
			if (computestructure.cam < -camrange) break;
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(computestructure), &computestructure, GL_DYNAMIC_COPY);	
			recalc(false);
			RotationMatrix = eulerAngleYXZ(r0, r1, r2);
			display();
			if (sw == 1) snapshot(true, 780, 1024, savepath);

			TwDraw();
			glfwSwapBuffers(mainwindow);
		} 
		while(running);
		sw = 0;
		time = time - glfwGetTime();
		do
		{	 
			static float r0, r1 ,r2 ;
			float inc = 0.1f;
			do {
			if (glfwGetKey(mainwindow, GLFW_KEY_LEFT)		== GLFW_PRESS)	{r0 += inc; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_RIGHT )		== GLFW_PRESS)	{r0 -= inc; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_UP )		== GLFW_PRESS)	{r1 += inc; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_DOWN )		== GLFW_PRESS)	{r1 -= inc; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_HOME)		== GLFW_PRESS)	{ r2 += inc; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_END)		== GLFW_PRESS)	{ r2 -= inc; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_PAGE_UP)	== GLFW_PRESS)	{field += 0.01f; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS)	{field -= 0.01f; break; }
			if (glfwGetKey(mainwindow, GLFW_KEY_SPACE )		== GLFW_PRESS)	printj = true;}
			while (false);
			Projection		= glm::perspective(field, 1024.0f / 780.0f, 0.1f, 12.0f);
			View			= glm::lookAt(glm::vec3(0,0,8), glm::vec3(0,0,0), glm::vec3(0,1,0)); 
			RotationMatrix	= eulerAngleYXZ(r0, r1, r2);
			MVP				= Projection * View * RotationMatrix;
			Sleep((DWORD)20.0);
			display();		
			//if (printj)
			//	snapshot(false, 780, 1024); printj = false; 
			TwDraw();
			glfwSwapBuffers(mainwindow);
			glfwWaitEvents();
		} 
		while((glfwGetKey(mainwindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS) && (glfwWindowShouldClose(mainwindow) == 0));
	}
	while(glfwWindowShouldClose(mainwindow) == 0 );

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &Computebuffer);
    glDeleteBuffers(1, &Framebuffer);
    glDeleteBuffers(1, &Normalbuffer);
	glDeleteProgram(Prog[0]); 
	glDeleteProgram(Prog[1]); 
	glDeleteProgram(Prog[2]); 
	glDeleteProgram(Prog[3]);
	glDeleteProgram(Prog[4]);
	glDeleteProgram(Prog[5]);
	glDeleteProgram(Prog[6]);
	glDeleteProgram(ViewshaderID);
	TwTerminate();
	glfwDestroyWindow(mainwindow);
	glfwTerminate();
	return 0;
}
/////////////////////////////
void recalc(bool mirror)
{
	glUseProgram(Prog[type]);
	glDispatchCompute(cols, rows, 1);
}
////////////////////////////
void display()
{	
	glPointSize(2.0f);
	glUseProgram(ViewshaderID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MVP = Projection * View * RotationMatrix * TranslationMatrix;
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	glUniform1f(acolorID, acolor);
	glUniform1f(alightID, alight);
	glUniform1f(ecolorID, ecolor);
	glUniform1f(slightID, slight);
	glUniform1f(spowerID, spower);
	glUniform1f(sineID, sine);
	glUniformMatrix4fv(MROTID, 1, GL_FALSE, &RotationMatrix[0][0]);

	glBindBuffer( GL_SHADER_STORAGE_BUFFER, Computebuffer );
	glGetBufferSubData	(GL_SHADER_STORAGE_BUFFER,  0, sizeof(computestructure), &computestructure);
	glUniform1f(maxdID, computestructure.maxd);
	glUniform1f(mindID, computestructure.mind);

	glBindBuffer(GL_ARRAY_BUFFER, Framebuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Normalbuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);
	glEnableVertexAttribArray(1);


	glDrawArrays(GL_POINTS, 0, computestructure.f2);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
////////////////////////////////////
void MousePosCB(GLFWwindow* auxwindow, double x, double y)
{
	xmpos = (int)x; ympos = (int)y;
	TwEventMousePosGLFW(mainwindow, x, y);
	return;
}
void MouseButtonCB(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(mainwindow, button, action, mods); //tweakbar call
	return;
}
///////////////////////////
void guisetup()
{	
	glfwSetMouseButtonCallback(mainwindow, MouseButtonCB);
	glfwSetCursorPosCallback(mainwindow, MousePosCB);
	//////////////////////////////
	TwBar * SetGUI = TwNewBar("Set");
	TwDefine(" Set refresh=0.2 visible=true color='255 0 255' position='0 40' size='200 280' "); 
	TwAddVarRW(SetGUI, "acolor",	TW_TYPE_FLOAT, &acolor,		"step=0.1	min=0 max= 1");
	TwAddVarRW(SetGUI, "alight",	TW_TYPE_FLOAT, &alight,		"step=0.1	min=0 max= 1");
	TwAddVarRW(SetGUI, "ecolor",	TW_TYPE_FLOAT, &ecolor,		"step=0.1	min=0 max= 1");
	TwAddVarRW(SetGUI, "slight",	TW_TYPE_FLOAT, &slight,		"step=0.1	min=0 max= 1");
	TwAddVarRW(SetGUI, "spower",	TW_TYPE_FLOAT, &spower,		"step=1		min=0 max= 100");
	TwAddVarRW(SetGUI, "sine",		TW_TYPE_FLOAT, &sine,		"step=1		min=0 max= 100");
	TwAddVarRW(SetGUI, "order",		TW_TYPE_FLOAT, &order,		"step=1		min=0 max= 10");
	TwAddVarRW(SetGUI, "iteration",	TW_TYPE_INT32, &iteration,	"step=1		min=0 max= 10");
	TwAddVarRW(SetGUI, "xpoints",	TW_TYPE_FLOAT, &xmul,				"step=0.5		min=1.0 max= 2.0");
	TwAddVarRW(SetGUI, "camera",	TW_TYPE_FLOAT, &computestructure.cam,	"step=0.0001	min=-10 max= 10");
	TwAddVarRW(SetGUI, "stored",	TW_TYPE_INT32, &computestructure.f2,	"step=1	min=0 max= 8000000");
	TwAddVarRW(SetGUI, "maxd",	TW_TYPE_FLOAT, &computestructure.maxd,	"step=0.0001		min=-10 max= 10");
	TwAddVarRW(SetGUI, "mind",	TW_TYPE_FLOAT, &computestructure.mind,	"step=0.0001		min=-10 max= 10");
	TwAddVarRW(SetGUI, "mul",	TW_TYPE_INT32, &sw,			"step=1		min=1 max= 2");
}

GLint status;
GLuint Compile(const char * compute_file_path){
	GLuint retval = LoadComputes(compute_file_path);
	glGetProgramiv(retval, GL_LINK_STATUS, &status);
	return retval;}
GLuint programview(char * vertex_file_path, const char * fragment_file_path){			
	GLuint retval	= LoadShaders( vertex_file_path, fragment_file_path );
	glGetProgramiv(retval, GL_LINK_STATUS, &status);
	MVPID			= glGetUniformLocation(retval, "MVP");
	MROTID			= glGetUniformLocation(retval, "MROT");
	maxdID			= glGetUniformLocation(retval, "maxd");
	mindID			= glGetUniformLocation(retval, "mind");
	acolorID		= glGetUniformLocation(retval, "acolor");
	alightID		= glGetUniformLocation(retval, "alight");
	ecolorID		= glGetUniformLocation(retval, "ecolor");
	slightID		= glGetUniformLocation(retval, "slight");
	spowerID		= glGetUniformLocation(retval, "spower");
	sineID			= glGetUniformLocation(retval, "sine");
	return retval;} 
////////////////////////