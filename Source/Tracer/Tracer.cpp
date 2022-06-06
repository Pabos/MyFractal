#include <sstream>   // std::cout
#include <iostream>   // std::cout
#include <fstream>
#include <vector>
#include <windows.h>
#include <string>
#include <cstring>
#include <ctime>
#include <malloc.h>
#include <stdlib.h>


#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
#include <AntTweakBar.h>
#include <commontracer/shader.hpp>
#include <commontracer/texture.hpp>
#include <commontracer/keyinput.hpp>

GLuint Compile(const char *);
GLuint programview(char * , const char *);
void MouseButtonCB(GLFWwindow*, int, int, int);
void MousePosCB(GLFWwindow*, double, double);
void MouseWheelCB(GLFWwindow*, double, double);
void KeybCB(GLFWwindow*, int, int, int, int);
void environment();
void gpushaders();
void gpubuffers();
void shutdown();
void guisetup();
void gasphisics(int);
void recalc();
void callrecalc();
void display();
void axesdisp();
void indata();
void setmodels(int);
void setmeshes();
void settextures();
vec3 vec43(vec4);
void modelsave(int);
void fnameprint(string, string, int);
void startload();
void modelload(int);
void meshsave(int);
void meshload(int);
void setsave(int);
void startsave();
void setload(int);
void updatemodel(int, int);
void updatemodels(int index);
void updatemesh(int, int);
void updatemeshes();
void updatemeshlist();
void updatcompute();
void tonewmodel();
void eulercalc(Model); 
void cpugassim();
void gpugassim();

vec4 help0, help1;
bool record = false, alttrace = false;
bool modelmod = true, oldmodelmod = true;
bool oldprot = false;
vec3 retang;
bool sim = false;
//////////////////// Set parameters
int wheel = 0, oldwheel, xmpos, ympos, zmpos, wheel_ = 0, oldwheel_ = 0;
int obj = 0, fnum = 0, modnum = 0;
int meshind = 0, meshnum = 0;
//#define GPU
struct Ambient
{
	vec3 camera;		int objstart;
	float steps;		int objs;				int reflection;		int obj;
	vec3 wx;			float shadow;
	vec3 wy;			float shadow1;
	vec3 LightDir;		float AmbientLight;
	vec3 LightSource;	float Alpha;
	float LightColor;	float LightSpecular;	float EmissiveColor;	float SpecularPower;
	vec3 g;				int filter0;
	float dt = 0.00001f;int iter = 1000;		int maxframes = 100;	float timer;
	float dum1;			float test;				int glass;				int filter1;
	float dum3;			float r0;				float r1;				float r2;
	mat4 camrot;
};

struct Ctrl {	bool goredraw;	 bool godisplay;	bool goprotarctor; };
Ctrl ctrl = {	true,			true,				false};
#define MODNUM 20
Model model[MODNUM];
Model modeltemp[MODNUM];
//#define MODELS 0
#define MESHNUM 2000
struct Mesh { vec4 v0; vec4 v1; vec4 v2; vec4 N; };
Mesh mesh[MESHNUM];
int meshsize;
struct MESHlist {int meshstart; int meshstop; float dm0; int dum1;};
MESHlist meshlist[20];
struct Text{ int textnum; float width; float height; int start; }; 
struct Globs
{
	int fnum;
	int modnum;
	int spare[20];
};
Globs globals;
Text textures[20];
Ambient computestruct;
mat4 invcamrot;
GLuint TracerID, AlttracerID, Filter0ID, Filter1ID, ViewshaderID, Viewshader1ID;
GLuint Spare0ID;
GLuint MatrixID, Matrix1ID;

bool go = false;
bool LEFT_PRESSED = false, RIGHT_PRESSED = false, MIDDLE_PRESSED = false; bool BOTH = false; bool SOMEPRESSED = false;
GLuint Positionbuffer, Filterbuffer, Computebuffer, Colorbuffer, Filterbuffer1, Coordbuffer, Modelbuffer, Meshbuffer, Meshlistbuffer, Tex0buffer, Texturesbuffer, Shadowbuffer, Helpbuffer;
GLuint Modeltemp;
GLuint Verticesbuffer;
int wgroupsize = 640;	
int sizex = (int)sqrt(wgroupsize * 4 / 3);
int sizey = sizex * 3 / 4;
//int col = 1024 / sizex;
int col = 32;
int row = 39;
int cols = col;
int rows = row;
float fps = 0, frames = 0, tf = 0;
float totk, totl, totu;
float dt_;

vec4 debugv;
float Sens = 0;
bool leftpressed = false;
bool rightpressed = false;
char rotlbl[3][25] = { " Rotate visible=false",		" Rotate visible=true",		" Rotate visible=false" };
char tralbl[3][25] = { " Translate visible=false",	" Translate visible=false",	" Translate visible=true" };
GLFWwindow* mainwindow;
///////////////////////////////////////////////////////////////////////////
int counter = 0;
int main( void )
{
	environment();
/////////////////////////////////////////
	gpushaders();
/////////////////////////////////////////
	gpubuffers();

	startload();
	modelload(modnum);
	setmeshes();
	setload(modnum);
	settextures();
	///////////////////////////////////////
	tf = (float)glfwGetTime();
	callrecalc();
	TwDraw();
	glfwSwapBuffers(mainwindow);
	do
	{
		glfwWaitEvents();
		//TwDraw();
		//glfwSwapBuffers(mainwindow);
		Sleep((DWORD)50.0);
	} 
	while( (glfwGetKey(mainwindow,GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(mainwindow) == 0));
	shutdown();
	return 0;
}
void environment()
{
	// Initialise GLFW
	if (!glfwInit()) { fprintf(stderr, "Failed to initialize GLFW\n"); return; }
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	mainwindow = glfwCreateWindow(1600, 780, "Main", NULL, NULL);
	if (mainwindow == NULL) { fprintf(stderr, "Failed to open GLFW window"); glfwTerminate(); return; }
	glfwMakeContextCurrent(mainwindow);

	glViewport(0, 0, 1024, 780);
	glfwSwapInterval(0);
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) { fprintf(stderr, "Failed to initialize GLEW\n"); return; }
	//// Initialize the GUI
	int twret = TwInit(TW_OPENGL_CORE, NULL);
	twret = TwWindowSize(1600, 780);
	glfwSetMouseButtonCallback(mainwindow, MouseButtonCB);
	glfwSetScrollCallback(mainwindow, MouseWheelCB);
	glfwSetCursorPosCallback(mainwindow, MousePosCB);
	glfwSetKeyCallback(mainwindow, KeybCB);
	guisetup();
}
////////////////////////////////////
void gpushaders()
{
	TracerID = Compile("Tracer.computeshader");
	//AlttracerID		= Compile("Alttracer.computeshader");
	AlttracerID = Compile("Spare0.computeshader");
	Filter0ID = Compile("Filter0.computeshader");
	Filter1ID = Compile("Filter1.computeshader");
	ViewshaderID = programview("View.vertexshader", "View.fragmentshader");
	Viewshader1ID = programview("View1.vertexshader", "View1.fragmentshader");
	Matrix1ID = glGetUniformLocation(Viewshader1ID, "MVP1");
	MatrixID = glGetUniformLocation(Viewshader1ID, "MVP");
#ifdef GPU
	Spare0ID = Compile("Spare0.computeshader");
#endif
}
////////////////////////////////////
void gpubuffers()
{
	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	///////////////////////////////////////////////////////////

	int siz = int(wgroupsize * rows * cols) * sizeof(vec4);
	//siz = 1024 * 1024 * sizeof(vec4);
	glGenBuffers(1, &Positionbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, siz, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Positionbuffer);
	/////////////////////////////////////////
	glGenBuffers(1, &Filterbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Filterbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, siz, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Filterbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, Filterbuffer);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);
	/////////////////////////////////////////
	glGenBuffers(1, &Computebuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(computestruct), &computestruct, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, Computebuffer);
	/////////////////////////////////////////
	glGenBuffers(1, &Filterbuffer1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Filterbuffer1);
	glBufferData(GL_SHADER_STORAGE_BUFFER, siz, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, Filterbuffer1);
	///////////////////////////////////////////
	glGenBuffers(1, &Colorbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Colorbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, siz, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, Colorbuffer);
	/////////////////////////////////////////
	glGenBuffers(1, &Coordbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Coordbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, siz, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, Coordbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, Coordbuffer);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, 0, (void*)0);
	///////////////////////////////////////////
	glGenBuffers(1, &Modelbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Modelbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MODNUM * sizeof(Model), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modeltemp);
	///////////////////////////////////////////
	glGenBuffers(1, &Shadowbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Shadowbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ((GLsizeiptr)wgroupsize * (GLsizeiptr)rows * (GLsizeiptr)cols) * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, Shadowbuffer);
	///////////////////////////////////////////
	glGenBuffers(1, &Helpbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Helpbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ((GLsizeiptr)wgroupsize * (GLsizeiptr)rows * (GLsizeiptr)cols) * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, Helpbuffer);
	///////////////////////////////////////////
#ifdef GPU
	glGenBuffers(1, &Modeltemp);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Modeltemp);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MODNUM * sizeof(Model), NULL, GL_DYNAMIC_COPY);
#endif
	///////////////////////////////////////////
	glGenBuffers(1, &Meshbuffer);
	glGenBuffers(1, &Tex0buffer);
	glGenBuffers(1, &Texturesbuffer);
	glGenBuffers(1, &Meshlistbuffer);

	glGenBuffers(1, &Verticesbuffer);
	vec4 frame_vertices[6];
	frame_vertices[0] = (vec4(0.0, 0, 0, 1));
	frame_vertices[1] = (vec4(2, 0, 0, 1));
	frame_vertices[2] = (vec4(0, 0, 0, 2));
	frame_vertices[3] = (vec4(0, 2.0f, 0, 2));
	frame_vertices[4] = (vec4(0, 0, 0, 3));
	frame_vertices[5] = (vec4(0, 0, 2.0f, 3));
	glBindBuffer(GL_ARRAY_BUFFER, Verticesbuffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec4), &frame_vertices[0], GL_STATIC_DRAW);
}
////////////////////////////////////
void shutdown()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDeleteBuffers(1, &Positionbuffer);
	glDeleteBuffers(1, &Filterbuffer1);
	glDeleteBuffers(1, &Filterbuffer);
	glDeleteBuffers(1, &Computebuffer);
	glDeleteBuffers(1, &Colorbuffer);
	glDeleteBuffers(1, &Coordbuffer);
	glDeleteBuffers(1, &Modelbuffer);
	glDeleteBuffers(1, &Texturesbuffer);
	glDeleteBuffers(1, &Tex0buffer);
	glDeleteBuffers(1, &Meshbuffer);
	glDeleteBuffers(1, &Meshlistbuffer);
	glDeleteBuffers(1, &Shadowbuffer);
	glDeleteBuffers(1, &Helpbuffer);
	glDeleteBuffers(1, &Verticesbuffer);
	glDeleteProgram(TracerID);
	glDeleteProgram(AlttracerID);
	glDeleteProgram(Filter0ID);
	glDeleteProgram(Filter1ID);
	glDeleteProgram(ViewshaderID);
	glDeleteProgram(Viewshader1ID);
	TwTerminate();
	glfwDestroyWindow(mainwindow);
	glfwTerminate();
}
////////////////////////////////////
void qmul(vec4 &q1, vec4 &q2, vec4 &ret)
{
	vec3 q2_ = vec3(q2.x, q2.y, q2.z); vec3 q1_ = vec3(q1.x, q1.y, q1.z);
	ret = vec4(q2_ * q1.w + q1_ * q2.w + cross(q1_, q2_), q1.w * q2.w - dot(q1_, q2_));
}
////////////////////////////////////
void rotate_vector(vec3& v, vec4& r, vec3 &rot)
{
	vec4 r_c = r * vec4(-1, -1, -1, 1);
	vec4 ret_, ret;
	qmul(vec4(v, 0), r_c, ret_);
	qmul(r, ret_, ret);
	rot = vec3(ret.x, ret.y, ret.z);
}
////////////////////////////////////
void cpugassim()
{
	//glfwSetTime(0);
	double tf_ = 1.0f / computestruct.maxframes;
	gasphisics(computestruct.iter);
	if (record) snapshot(true, 780, 1024);
	double tcomp = glfwGetTime();
	do
	{
		tf = (float)glfwGetTime();
	} while (tf < tf_);
	glfwSetTime(0);
	fps = 1 / tf;
	frames++;
	computestruct.timer -= (float)tf_;
	if (computestruct.timer < 0) sim = false;
	printf("%f %f %f \n", frames, float(tcomp * 1000.0), fps);
}
////////////////////////////////////
bool planecollision(Model &sphere, Model &plane)
{
	vec4  q = vec4(plane.orient.x, plane.orient.y, plane.orient.z, plane.orient.w);
	vec3 N; rotate_vector(vec3(0, 1, 0), q, N);
	if (abs(dot(N, sphere.centre - plane.centre)) < sphere.mdim.x) 
	{
		dt_ = 1.5f * computestruct.dt;
		sphere.speed = reflect(sphere.speed, N);
		return true;
	}
	return false;
}
////////////////////////////////////
void planebounce(Model &sphere, Model &plane)
{
	vec4  q = vec4(plane.orient.x, plane.orient.y, plane.orient.z, plane.orient.w);
	vec3 N; rotate_vector(vec3(0, 1, 0), q, N);
	vec3 pspeed = sphere.speed - dot(sphere.speed, N) * N;
	vec3 temp = cross(N, pspeed);
	if (length(temp) > 0.00001)
	{
		sphere.spin = normalize(temp);
		sphere.omega = length(temp)/(sphere.mdim.x);
	}
}
////////////////////////////////////
void spherespin(Model &sphere)
{
	if(sphere.omega > 0.0001)
	{
		sphere.orient = angleAxis(sphere.omega * dt_, sphere.spin) * sphere.orient;
		sphere.invorient = conjugate(sphere.orient);
	}
}
////////////////////////////////////
float m = 1, m1 = 1, m2 = 1;
void spheresbounce(Model &sphere1, Model &sphere2)
{
	dt_ = 1.5f * computestruct.dt;
	vec3 k = normalize(sphere1.centre - sphere2.centre);
	vec3 speed1 = sphere1.speed;
	vec3 speed2 = sphere2.speed;
	vec3 baricentro = (sphere1.centre + sphere2.centre) * 0.5f;
	vec3 bc1 = baricentro - sphere1.centre;
	vec3 bc1N = normalize(bc1);
	vec3 bc2 = baricentro - sphere2.centre;
	vec3 bc2N = normalize(bc2);

	float a = dot(2.0f * k, (sphere1.speed - sphere2.speed) / (1 / m1 + 1 / m2));
	sphere2.speed = sphere2.speed + a * k/* / m2*/;
	sphere1.speed = sphere1.speed - a * k/* / m1*/;

	vec3 v1 = cross(sphere1.omega * sphere1.spin, baricentro - sphere1.centre) + speed1 - sphere2.speed;
	vec3 v2 = cross(sphere2.omega * sphere2.spin, baricentro - sphere2.centre) + speed2 - sphere1.speed;

	vec3 temp;
	temp = v2 - dot(v2, bc1N) * bc1N;
	temp = cross(bc1N, temp);
	sphere1.spin = normalize(temp);
	sphere1.omega = length(temp) / length(bc1);

	temp = v1 - dot(v1, bc2N) * bc2N;
	temp = cross(bc2N, temp);
	sphere2.spin = normalize(temp);
	sphere2.omega = length(temp) / length(bc2);
}
////////////////////////////////////
void gasphisics(int iter)
{
	dt_ = computestruct.dt;
	totk = 0; totl = 0;
	bool endloop = false;
	for (int i = 0; i < iter; i++)
	{
		for (int spheres = 0; spheres < computestruct.objs; spheres++)		///spheres and sphere
			if (model[spheres].mtype == 1 && !model[spheres].hidden)
			{
				for (int sphere = spheres + 1; sphere <= computestruct.objs; sphere++)
					if (model[sphere].mtype == 1 && !model[sphere].hidden)
						if (length(model[sphere].centre - model[spheres].centre) < (model[sphere].mdim.x + model[spheres].mdim.x)) //// collision sphere/sphere & speed swap
						{
							spheresbounce(model[sphere], model[spheres]);
							//endloop = true;
							//break;
						} 
			}

		for(int spheres = 0; spheres <= computestruct.objs; spheres++)		///spheres and planes
			if (model[spheres].mtype == 1 && !model[spheres].hidden)
			{ 
				for (int planes = 0; planes <= computestruct.objs; planes++)
					if (model[planes].mtype == 0)
						if (planecollision(model[spheres], model[planes])		/* & !endloop*/) //// collision sphere/plane & speed swap
						{
							planebounce(model[spheres], model[planes]);
							//endloop = true;
							//break;
						}
				model[spheres].speed = model[spheres].speed + m * computestruct.g * dt_;
				model[spheres].centre = model[spheres].centre + model[spheres].speed * dt_;
				spherespin(model[spheres]);
			}

		if (endloop) break;
		dt_ = computestruct.dt;
	}

	for (int i = 0; i <= computestruct.objs; i++)		// energy simulation
		if(model[i].mtype == 1)
		{
			totk += 0.5f * dot(model[i].speed, model[i].speed);
			totl -= computestruct.g.y * model[i].centre.y;
			totu = totk + totl;
		}
	updatemodels(0);
}
////////////////////////////////////
void recalc()
{
	if(alttrace)
		glUseProgram(AlttracerID);
	else
		glUseProgram(TracerID);
	glDispatchCompute(cols, rows, 1);
	glUseProgram(Filter0ID); 
	glDispatchCompute(cols, rows, 1);
	glUseProgram(Filter1ID);
	glDispatchCompute(cols, rows, 1);
}
////////////////////////////
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(GLfloat(1));
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(3);
	glUseProgram(ViewshaderID);
	glDrawArrays(GL_POINTS, 0, 780 * 1024);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
}
////////////////////////////////////
void axesdisp()
{
	static float fovy = 2 * atan(1.5f / 4.0f);
	static glm::mat4 Projection = glm::perspectiveFov(fovy, 1024.0f, 780.0f, 1.0f, 8.0f);
	static glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 MVP;
	if(modelmod)
		MVP = Projection * View * invcamrot * translate(mat4(), model[obj].centre) * mat4_cast(model[obj].orient);
	else
		MVP = Projection * View * invcamrot * translate(mat4(), model[obj].centre);
	glUseProgram(Viewshader1ID);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, Verticesbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, 6);
	glDisableVertexAttribArray(0); 
}
////////////////////////////////////
void callrecalc()
{
	updatcompute();
	recalc();
	display();
}
void updateview()
{
	computestruct.camrot = eulerAngleYXZ(computestruct.r0, computestruct.r1, computestruct.r2);
	invcamrot = inverse(computestruct.camrot);
}
////////////////////////////////////
void indata()
{
	static int oldxpos = 0, oldypos = 0, oldzpos = 0;
	if ((xmpos < 1024) & SOMEPRESSED)
	{
		float diff = 0;
		if (LEFT_PRESSED) diff = float(xmpos - oldxpos); else
		if (RIGHT_PRESSED) diff = float(-ympos + oldypos); else
		if (BOTH) diff = float(xmpos - oldxpos);
		oldwheel_ = wheel_;
		vec3 rotax;
		if(modelmod)
		{
			vec4  q = vec4(model[obj].orient.x, model[obj].orient.y, model[obj].orient.z, model[obj].orient.w);
			if (LEFT_PRESSED) 	rotate_vector(vec3(0, 1, 0), q, rotax); else
			if (RIGHT_PRESSED)  rotate_vector(vec3(-1, 0, 0), q, rotax); else
			if (BOTH) 	rotate_vector(vec3(0, 0, 1), q, rotax);
		}
		else
		{
			if (LEFT_PRESSED)	rotax = vec3(0, 1, 0); else
			if (RIGHT_PRESSED)	rotax = vec3(-1, 0, 0); else
			if (BOTH)	rotax = vec3(0, 0, 1);
		}
		if (wheel == 1)
//  rotate
		{
			diff /= 90.0f;
			//model[obj].orient = angleAxis(diff * 1.8f / 3.14159265f, rotax) * model[obj].orient;
			model[obj].orient = angleAxis(diff * 3.141592f, rotax) * model[obj].orient;
			model[obj].invorient = conjugate(model[obj].orient);
			eulercalc(model[obj]);  // updates twbar...
		}
		else if (wheel == 2)
//  translate
		{
			diff /= 300.0f;
			model[obj].centre.x += rotax.y * diff;
			model[obj].centre.y -= rotax.x * diff;
			model[obj].centre.z += rotax.z * diff ;
		}
		updatemodel(obj, 0);
	}
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, Helpbuffer);
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, (unsigned long long(xmpos) + (780 - unsigned long long(ympos)) * 1024) * sizeof(vec4), sizeof(vec4), &debugv);
	oldxpos = xmpos;
	oldypos = ympos;
	oldzpos = zmpos;
	return;	
}
////////////////////////////////////
void eulercalc(Model model)
{
	vec4  q = vec4(model.orient.x, model.orient.y, model.orient.z, model.orient.w);
	vec3 wx; rotate_vector(vec3(1, 0, 0), q, wx);
	vec3 wy; rotate_vector(vec3(0, 1, 0), q, wy);
	vec3 wz; rotate_vector(vec3(0, 0, 1), q, wz);
	vec3 cp;
	float dp;
	float ratio = 90.0f / (float)asin(-1);
	cp = cross(vec3(1, 0, 0), wx);
	dp = dot(vec3(1, 0, 0), wx);
	retang.x = atan(length(cp), dp);
	cp = cross(vec3(0, 1, 0), wy);
	dp = dot(vec3(0, 1, 0), wy);
	retang.y = atan(length(cp), dp);
	cp = cross(vec3(),  wz);
	dp = dot(vec3(0, 0, 1), wz);
	retang.z = atan(length(cp), dp);
	retang = retang * ratio;
}
////////////////////////////////////
void MousePosCB(GLFWwindow* auxwindow,double x ,double y)
{	
	TwEventMousePosGLFW(mainwindow, x, y);
	xmpos = (int)x; 
	ympos = (int)y;
	indata();
	if (SOMEPRESSED && (xmpos< 1024))
	{
		callrecalc();
		axesdisp();
	}
	TwDraw();
	glfwSwapBuffers(mainwindow);
	//Sleep((DWORD)50.0);
}
///////////////////////////////////
void MouseWheelCB(GLFWwindow* window, double xoffset, double yoffset)
{
	sim = false;
	wheel += (int)yoffset;
	if (wheel > 2) wheel = 1; 
	if (wheel < 1) wheel = 2;
	TwDefine(rotlbl[wheel]); TwDefine(tralbl[wheel]);
	TwDraw();
	glfwSwapBuffers(mainwindow); 
}
///////////////////////////////////
void MouseButtonCB(GLFWwindow* window, int button, int action, int mods ){
	TwEventMouseButtonGLFW(mainwindow, button, action, mods); //tweakbar call

	if (action == GLFW_PRESS)
	{
		if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS))		LEFT_PRESSED = true;
		if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE))		LEFT_PRESSED = false;

		if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS))		RIGHT_PRESSED = true; 
		if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_RELEASE)) 	RIGHT_PRESSED = false;

		SOMEPRESSED = true;

		if (LEFT_PRESSED && RIGHT_PRESSED) {LEFT_PRESSED = false; RIGHT_PRESSED = false; BOTH = true; }

		if(xmpos < 1024)
		{
			vec4 crot;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionbuffer);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, (unsigned long long(xmpos) + (780 - unsigned long long(ympos)) * 1024) * sizeof(vec4), sizeof(vec4), &crot);
			help0 = crot;
			obj = (int)crot.w;
			if (obj < 0) obj = 0;
			display();
			axesdisp();
			TwDraw();
			glfwSwapBuffers(mainwindow);
			Sleep((DWORD)200.0);
		}

		if ((button == GLFW_MOUSE_BUTTON_MIDDLE) && (action == GLFW_PRESS))
		{
			ctrl.goprotarctor = !ctrl.goprotarctor;
			display();
			axesdisp();
			TwDraw();
			glfwSwapBuffers(mainwindow);
			glfwWaitEvents();
		}
	}
	else
	{
		LEFT_PRESSED = false;
		RIGHT_PRESSED = false;
		BOTH = false;
		SOMEPRESSED = false; 
		//callrecalc();
		//updatcompute();
		//recalc();
		display();
		TwDraw(); 
		glfwSwapBuffers(mainwindow);
	}
	return;
}
//////////////////////////////////
void KeybCB(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	do 
	{
		float inc = 0.01f;
		if (glfwGetKey(mainwindow, GLFW_KEY_LEFT) == GLFW_PRESS)	{ computestruct.r0 += inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_RIGHT) == GLFW_PRESS)	{ computestruct.r0 -= inc; updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_UP) == GLFW_PRESS)		{ computestruct.r1 += inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_DOWN) == GLFW_PRESS)	{ computestruct.r1 -= inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_HOME) == GLFW_PRESS)	{ computestruct.r2 += inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_END) == GLFW_PRESS)		{ computestruct.r2 -= inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_HOME) == GLFW_PRESS)	{ computestruct.r2 += inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_END) == GLFW_PRESS)		{ computestruct.r2 -= inc;	updateview(); break; }
		if (glfwGetKey(mainwindow, GLFW_KEY_N) == GLFW_PRESS)		{
			modelmod = !modelmod;
			if (modelmod)
			{
				TwDefine("Rotate label = 'Rotate mod'"); TwDefine("Translate label = 'Translate mod'");
			}
			else
			{
				TwDefine("Rotate label = 'Rotate sys'"); TwDefine("Translate label = 'Translate sys'");
			}
			inc; break; 
		}
		if (glfwGetKey(mainwindow, GLFW_KEY_SPACE) == GLFW_PRESS) 
		{
			wheel++;	if (wheel > 2) wheel = 0;
			TwDefine(rotlbl[wheel]);	TwDefine(tralbl[wheel]);	
			Sleep((DWORD)200.0); break; 
		}
	} 
	while (false);
	callrecalc();
	glfwSwapBuffers(mainwindow);
	return;
}
///////////////////////////////////
void refresh()
{
	recalc();
	display();
	if (ctrl.goprotarctor)  axesdisp();
	TwDraw(); 
	glfwSwapBuffers(mainwindow);
	Sleep((DWORD)25.0);
}
///////////////////////////////////
Model modelpaste; int modelptr;
void TW_CALL fnumset(const void* value, void* clientData)	{	fnum = *(int*)value; setload(fnum); refresh();}
void TW_CALL fnumget(void* value, void* clientData)			{ *(int*)value = fnum; }
void TW_CALL Reset(void* clientData)						{ setload(fnum); refresh();}
void TW_CALL Snap(void* clientData)							{ snapshot(false, 780, 1024); }
void TW_CALL Setsave(void* clientData)						{ setsave(fnum); startsave(); }
void TW_CALL Savefile(void *clientData)						{ modelsave(modnum); refresh();}
void TW_CALL modset(const void* value, void* clientData)	{ modnum = *(int*)value; modelload(modnum); refresh(); }
void TW_CALL modget(void* value, void* clientData)			{ *(int*)value = modnum; }
void TW_CALL Loadfile(void *clientData)		{ 
	modelload(modnum); 
	setload(fnum); 
	refresh();}
void TW_CALL Statcopy(void* clientData)		{ modelptr = obj; }
void TW_CALL Statpaste(void *clientData)	{ model[obj] = model[modelptr];	updatemodel(obj, 0); refresh(); }
void TW_CALL Statswap(void* clientData)		{ modelpaste = model[modelptr]; model[modelptr] = model[obj]; model[obj] = modelpaste; updatemodels(0); refresh(); }
void TW_CALL Statinsert(void *clientData)	{ for (int i = MODNUM -1; i > obj; i--) model[i] = model[i-1]; model[obj] = model[modelptr]; updatemodels(0); refresh();}
void TW_CALL Statdelete(void* clientData)	{ for (int i = obj; i < (MODNUM - 2); i++) model[i] = model[i+1];updatemodels(0); refresh(); }
///////////////////////////////////
void TW_CALL Colorset(const void* value, void* clientData)		{ model[obj].color = *(vec4*)value; updatemodel(obj, 0); refresh(); }
void TW_CALL Colorget(void* value, void* clientData)			{*(vec4*)value = model[obj].color;}
void TW_CALL Emissiveset(const void* value, void* clientData)	{ model[obj].EmissiveColor = *(float*)value; updatemodel(obj, 0); refresh(); }
void TW_CALL Emissiveget(void* value, void* clientData)			{*(float*)value = model[obj].EmissiveColor;}
void TW_CALL Rayset(const void* value, void* clientData)		{ model[obj].raystep = *(float*)value; updatemodel(obj, 0); refresh(); }
void TW_CALL Rayget(void* value, void* clientData)				{ *(float*)value = model[obj].raystep; }
void TW_CALL Specularset(const void* value, void* clientData)	{ model[obj].Specular = *(float*)value; updatemodel(obj, 0); refresh(); }
void TW_CALL Specularget(void* value, void* clientData)			{*(float*)value = model[obj].Specular;}
void TW_CALL Spowerset(const void* value, void* clientData)		{ model[obj].Spower = *(float*)value;  updatemodel(obj, 0); refresh(); }
void TW_CALL Spowerget(void* value, void* clientData)			{ *(float*)value = model[obj].Spower; }
void TW_CALL Refindexset(const void* value, void* clientData)	{ model[obj].refindex = *(float*)value;  updatemodel(obj, 0); refresh(); }
void TW_CALL Refindexget(void* value, void* clientData)			{ *(float*)value = model[obj].refindex; }
void TW_CALL objset(const void* value, void* clientData)		{ obj = *(int*)value; if (obj > computestruct.objs) obj = 0;  else if (obj < 0)  obj = computestruct.objs;  updatemodel(obj, 0);}
void TW_CALL objget(void* value, void* clientData)				{*(int*)value = obj; }
void TW_CALL hiddenset(const void* value, void* clientData)		{ model[obj].hidden = *(bool*)value; updatemodel(obj, 0); refresh(); }
void TW_CALL hiddenget(void* value, void* clientData)			{*(bool*)value = model[obj].hidden;}
void TW_CALL centerset(const void* value, void* clientData)		{ model[obj].centre = *(vec3*)value; updatemodel(obj, 0); refresh(); }
void TW_CALL centerget(void* value, void* clientData)			{*(vec3*)value = model[obj].centre;}
void TW_CALL speedset(const void* value, void* clientData)		{ model[obj].speed = *(vec3*)value; updatemodel(obj, 0); }
void TW_CALL speedget(void* value, void* clientData)			{ *(vec3*)value = model[obj].speed; }
void TW_CALL omegaset(const void* value, void* clientData)		{ model[obj].omega = *(float*)value; updatemodel(obj, 0); }
void TW_CALL omegaget(void* value, void* clientData)			{ *(float*)value = model[obj].omega; }
void TW_CALL debugset(const void* value, void* clientData)		{}
void TW_CALL debugget(void* value, void* clientData)			{ *(vec4*)value = vec4(model[obj].spin, 0); }
//void TW_CALL debugget(void* value, void* clientData) { *(vec4*)value = debugv; }
//////////////////// rot parameters
vec4 temp = vec4(0, 0, 0, 0);//////////////////// temp parameters
vec4 oldtemp = vec4(0, 0, 0, 0);
void TW_CALL orientset(const void* value, void* clientData)
{
	//mat4 rot;
	temp = *(vec4*)value;
	vec4 diff = temp - oldtemp;
	oldtemp = temp;
	if (diff.w != 0) 
	{ model[obj].orient = quat(1, 0, 0, 0);  temp = vec4(0); diff = vec4(0, 0, 0, 0); }

	vec4  q = vec4(model[obj].orient.x, model[obj].orient.y, model[obj].orient.z, model[obj].orient.w);
	vec3 rotax; rotate_vector(vec3(1, 0, 0), q, rotax);
	model[obj].orient = angleAxis(length(diff) * 1.8f / 3.14f, rotax) * model[obj].orient;
	model[obj].invorient = conjugate(model[obj].orient);
	updatemodel(obj, 0);
	eulercalc(model[obj]);
}
void TW_CALL orientget(void* value, void* clientData)				{ *(vec4*)value = vec4(model[obj].orient.x, model[obj].orient.y, model[obj].orient.z, model[obj].orient.w); }
////////////////////////////////////
void TW_CALL dimset(const void* value, void* clientData)			{ model[obj].mdim = *(vec3*)value;  updatemodel(obj, 0); refresh(); }
void TW_CALL dimget(void* value, void* clientData)					{*(vec3*)value = model[obj].mdim;}
void TW_CALL Typeset(const void* value, void* clientData)			{ model[obj].mtype = (float)*(int*)value;  updatemodel(obj, 0); refresh(); }
void TW_CALL Typeget(void* value, void* clientData)					{ *(int*)value = (int)model[obj].mtype; }
void TW_CALL intersectset(const void* value, void* clientData)		{ model[obj].intersect = (float)*(int*)value;  updatemodel(obj, 0); }
void TW_CALL intersectget(void* value, void* clientData)			{ *(int*)value = (int)model[obj].intersect; }
/////////////////// computestructure variables
void TW_CALL CSAmbientLightset(const void* value, void* clientData) { computestruct.AmbientLight = *(float*)value; updatcompute(); refresh();
}
void TW_CALL CSAmbientLightget(void* value, void* clientData)		{ *(float*)value = computestruct.AmbientLight; }
void TW_CALL CSLightColorset(const void* value, void* clientData)	{ computestruct.LightColor = *(float*)value; updatcompute(); refresh();
}
void TW_CALL CSLightColorget(void* value, void* clientData)			{ *(float*)value = computestruct.LightColor; }
void TW_CALL CSLightDirset(const void* value, void* clientData)		{ computestruct.LightDir = *(vec3*)value; updatcompute(); refresh();
}
void TW_CALL CSLightDirget(void* value, void* clientData)			{ *(vec3*)value = computestruct.LightDir; }
void TW_CALL CSshadowset(const void* value, void* clientData)		{ computestruct.shadow = *(float*)value; updatcompute(); refresh();
}
void TW_CALL CSshadowget(void* value, void* clientData)				{ *(float*)value = computestruct.shadow; }
void TW_CALL CSreflectionset(const void* value, void* clientData)	{ computestruct.reflection = *(int*)value; updatcompute(); refresh();}
void TW_CALL CSreflectionget(void* value, void* clientData)			{ *(int*)value = computestruct.reflection; }
void TW_CALL CSshadow1set(const void* value, void* clientData)		{ computestruct.shadow1 = *(float*)value; updatcompute(); refresh();
}
void TW_CALL CSshadow1get(void* value, void* clientData)			{ *(float*)value = computestruct.shadow1; }
void TW_CALL CSobjstartset(const void* value, void* clientData)		{  computestruct.objstart = *(int*)value; updatcompute(); refresh();}
void TW_CALL CSobjstartget(void* value, void* clientData)			{ *(int*)value = computestruct.objstart; }
void TW_CALL CSobjsset(const void* value, void* clientData)			{ computestruct.objs = *(int*)value; updatcompute(); refresh();}
void TW_CALL CSobjsget(void* value, void* clientData)				{ *(int*)value = computestruct.objs; }
void TW_CALL CSnoiseset(const void* value, void* clientData)		{ computestruct.filter0 = *(bool*)value;	updatcompute();}
void TW_CALL CSnoiseget(void* value, void* clientData)				{ *(bool*)value = computestruct.filter0;}
void TW_CALL CSedgeset(const void* value, void* clientData)			{ computestruct.filter1 = *(bool*)value;	updatcompute(); }
void TW_CALL CSedgeget(void* value, void* clientData)				{ *(bool*)value = computestruct.filter1; }
void TW_CALL CSrefractionset(const void* value, void* clientData)	{ computestruct.glass = *(bool*)value;	updatcompute(); refresh();
}
void TW_CALL CSrefractionget(void* value, void* clientData)			{ *(bool*)value = computestruct.glass; }
void TW_CALL CStestset(const void* value, void* clientData)			{ computestruct.test = *(bool*)value;	updatcompute(); }
void TW_CALL CStestget(void* value, void* clientData)				{ *(bool*)value = computestruct.test; }
void TW_CALL simset(const void* value, void* clientData)			
{
	if (!sim) {} sim = !sim; 		
	while (sim) 	
	{
		//gpugassim();
		cpugassim();
		recalc();
		display();

		TwDraw();
		glfwSwapBuffers(mainwindow);
		if (sim)				glfwPollEvents();
	}

}
void TW_CALL simget(void* value, void* clientData)					{*(bool*)value = sim; }
//////////////////// temp parameters
void TW_CALL helpset0(const void* value, void* clientData){}
void TW_CALL helpget0(void* value, void* clientData)				{ if (help0 != vec4(0, 0, 0, 0)) *(vec4*)value = help0;}
void TW_CALL helpset1(const void* value, void* clientData){}
void TW_CALL helpget1(void* value, void* clientData)				{if (help1 != vec4(0, 0, 0, 0))	*(vec4*)value = help1;}
////////////////////////////////////////////////////////////////////
typedef struct {	float	x;	float	y;	float	z;	float	w;}MyStruct;
MyStruct elem;  // an element of type MyStruct that we want to add to a tweak bar
TwStructMember myStructInt[] = {
	{ "x", TW_TYPE_FLOAT, offsetof(MyStruct, x), "step=1" },
	{ "y", TW_TYPE_FLOAT, offsetof(MyStruct, y), "step=1" },
	{ "z", TW_TYPE_FLOAT, offsetof(MyStruct, z), "step=1" },
	{ "w", TW_TYPE_FLOAT, offsetof(MyStruct, w), "step=1" }
};
TwStructMember myStructMembers[] = {
	{ "x", TW_TYPE_FLOAT, offsetof(MyStruct, x), "step=0.01" },
	{ "y", TW_TYPE_FLOAT, offsetof(MyStruct, y), "step=0.01" },
	{ "z", TW_TYPE_FLOAT, offsetof(MyStruct, z), "step=0.01" },
	{ "w", TW_TYPE_FLOAT, offsetof(MyStruct, w), "step=0.01" }
};
///////////////////////////
TwStructMember myStructlong[] = {
	{ "x", TW_TYPE_FLOAT, offsetof(MyStruct, x), "step=0.0001" },
	{ "y", TW_TYPE_FLOAT, offsetof(MyStruct, y), "step=0.0001" },
	{ "z", TW_TYPE_FLOAT, offsetof(MyStruct, z), "step=0.0001" },
	{ "w", TW_TYPE_FLOAT, offsetof(MyStruct, w), "step=0.0001" }
};
/////////////////////////////////////////
void TW_CALL Textureset(const void* value, void* clientData) {	
	model[obj].text = *(int*)value;  updatemodel(obj, 0); refresh();}
void TW_CALL Textureget(void* value, void* clientData)		{ *(int*)value = model[obj].text; }
void TW_CALL Meshset(const void* value, void* clientData)	{ model[obj].mmesh = *(int*)value;  updatemodel(obj, 0); refresh(); }
void TW_CALL Meshget(void* value, void* clientData)			{ *(int*)value = model[obj].mmesh; }
void TW_CALL Savemesh(void* clientData)						{ meshsave(0); }
void TW_CALL Loadmesh(void* clientData)						{ meshload(0); }
void TW_CALL Togglemod(void* clientData)
{
	modelmod = !modelmod;
	if (modelmod)
	{	TwDefine("Rotate label = 'Rotate mod'"); TwDefine("Translate label = 'Translate mod'"); }
	else
	{	TwDefine("Rotate label = 'Rotate sys'"); TwDefine("Translate label = 'Translate sys'"); }
}
////////////////////////////////////////////////////////////////////

void TW_CALL Meshsindset(const void* value, void* clientData)	{ meshind = *(int*)value; updatemeshlist(); refresh(); }
void TW_CALL Meshindget(void* value, void* clientData)			{ *(int*)value = meshind; }
void TW_CALL Meshstartset(const void* value, void* clientData)	{ meshlist[meshind].meshstart = *(int*)value; updatemeshlist(); refresh();}
void TW_CALL Meshstartget(void* value, void* clientData)		{*(int*)value = meshlist[meshind].meshstart;}
void TW_CALL V0set(const void* value, void* clientData)			
{
	mesh[meshnum + meshlist[meshind].meshstart].v0 = *(vec4*)value; updatemesh(meshnum + meshlist[meshind].meshstart,0); refresh();
}
void TW_CALL V0get(void* value, void* clientData)				
{ 
	*(vec4*)value = mesh[meshnum + meshlist[meshind].meshstart].v0; 
}
void TW_CALL V1set(const void* value, void* clientData)			
{ 
	mesh[meshnum + meshlist[meshind].meshstart].v1 = *(vec4*)value; updatemesh(meshnum + meshlist[meshind].meshstart, 0); refresh();
}
void TW_CALL V1get(void* value, void* clientData)				{ *(vec4*)value = mesh[meshnum + meshlist[meshind].meshstart].v1; }
void TW_CALL V2set(const void* value, void* clientData)			
{
	mesh[meshnum + meshlist[meshind].meshstart].v2 = *(vec4*)value; updatemesh(meshnum + meshlist[meshind].meshstart, 0); refresh();
}
void TW_CALL V2get(void* value, void* clientData)				{ *(vec4*)value = mesh[meshnum + meshlist[meshind].meshstart].v2; }
void TW_CALL Meshstopset(const void* value, void* clientData) { meshlist[meshind].meshstop = *(int*)value; updatemeshlist(); refresh(); }
void TW_CALL Meshstopget(void* value, void* clientData)			{ *(int*)value = meshlist[meshind].meshstop;  }
void TW_CALL Meshsnumset(const void* value, void* clientData) { meshnum = *(int*)value; updatemeshlist(); refresh(); }
void TW_CALL Meshnumget(void* value, void* clientData) { *(int*)value = meshnum; }

////////////////////////////////////////////////////////////////////
void guisetup()
{	
	TwType TWvec3 = TwDefineStruct("TWvec3", myStructMembers, 3, sizeof(MyStruct), NULL, NULL);
	TwType TWvec4 = TwDefineStruct("TWvec4", myStructMembers, 4, sizeof(MyStruct), NULL, NULL);
	TwType TWvec5 = TwDefineStruct("TWvec5", myStructlong, 4, sizeof(MyStruct), NULL, NULL);
	TwType TWvec6 = TwDefineStruct("TWvec6", myStructInt, 4, sizeof(MyStruct), NULL, NULL);

//////////////////// Rotate parameters
	TwBar* RotateGUI = TwNewBar("Rotate");
	TwDefine(" Rotate refresh=0.005  visible=false position='1225 0' size='200 40' ");
	TwDefine("Rotate label = 'Rotate mod'");
	//TwAddButton(RotateGUI, "CentreRot", CRot, NULL, NULL);
	TwAddVarRW(RotateGUI, "Sens", TW_TYPE_FLOAT, &Sens, "step=1.0	min=-5 max= 5");
	TwAddButton(RotateGUI, "Navmode", Togglemod, NULL, NULL);
//////////////////// Translate parameters
	TwBar* TranslatetGUI = TwNewBar("Translate");
	TwDefine(" Translate refresh=0.005  visible=false color='0 255 255' position='1225 0' size='200 40' ");
	TwDefine("Translate label = 'Translate mod'");
	//TwAddButton(TranslatetGUI, "store", Statstorer, NULL, NULL);
	TwAddVarRW(TranslatetGUI, "Sens", TW_TYPE_FLOAT, &Sens, "step=1.0	min=-5 max= 5");
	TwAddButton(TranslatetGUI, "Navmode", Togglemod, NULL, NULL);
//////////////////// Setup parameters
	TwBar* SetGUI = TwNewBar("Set");
	TwDefine("Set label = 'Global Set'");
	TwDefine(" Set refresh=0.2 color='255 0 255' position='1025 1' size='200 350' text=dark");
	TwAddVarCB(SetGUI, "fnum",			TW_TYPE_INT32, fnumset, fnumget, NULL, "step=1	min=0 max= 9");
	TwAddButton(SetGUI, "reset",		Reset, NULL, NULL);
	TwAddVarCB(SetGUI, "A. Light",		TW_TYPE_FLOAT, CSAmbientLightset, CSAmbientLightget, NULL,"step=0.01	min=0 max= 1");
	TwAddVarCB(SetGUI, "Light C.",		TW_TYPE_FLOAT, CSLightColorset, CSLightColorget,	NULL, "step=0.01	min=0 max= 1");
	TwAddVarCB(SetGUI, "Light D.",		TWvec3,			CSLightDirset, CSLightDirget,		NULL, "opened=false");
	TwAddVarCB(SetGUI, "Shadows",		TW_TYPE_FLOAT, CSshadowset, CSshadowget,			NULL, "step=0.01	min= 0.00 max= 1.00");
	TwAddVarCB(SetGUI, "Shadows1",		TW_TYPE_FLOAT, CSshadow1set, CSshadow1get,			NULL, "step=0.01	min= 0.00 max= 1.00");
	TwAddVarCB(SetGUI, "shadow edges",	TW_TYPE_BOOL8, CSedgeset, CSedgeget,				NULL, NULL);
	TwAddVarCB(SetGUI, "Refractions",	TW_TYPE_BOOL8, CSrefractionset, CSrefractionget,	NULL, NULL);
	TwAddVarCB(SetGUI, "Reflections",	TW_TYPE_INT32, CSreflectionset, CSreflectionget,	NULL, "step=1 min= 0 max= 14");
	TwAddVarCB(SetGUI, "noise filter",	TW_TYPE_BOOL8, CSnoiseset, CSnoiseget, NULL, NULL);
	TwAddVarCB(SetGUI, "objsstart",		TW_TYPE_INT32, CSobjstartset, CSobjstartget,		NULL, "min= 0");
	TwAddVarCB(SetGUI, "objstop",		TW_TYPE_INT32, CSobjsset, CSobjsget,				NULL, "min= 0 max= 19");
	TwAddButton(SetGUI, "snapshot",		Snap, NULL, NULL);
	TwAddVarRW(SetGUI, "record",		TW_TYPE_BOOL8, &record, NULL);
	TwAddButton(SetGUI, "Save",			Setsave, NULL, NULL);
	TwAddVarRW(SetGUI, "Alttracer",		TW_TYPE_BOOL8, &alttrace, NULL);
	TwAddVarCB(SetGUI, "help0",			TWvec5, helpset0, helpget0, NULL, "opened=false");
	TwAddVarCB(SetGUI, "help1",			TWvec5, helpset1, helpget1, NULL, "opened=false");
	TwAddVarCB(SetGUI, "Test",			TW_TYPE_BOOL8, CStestset, CStestget, NULL, NULL);
///////////////////
	TwBar* SimGUI = TwNewBar("Simulation");
	TwDefine("Simulation label = 'Simulation Set'");
	TwDefine(" Simulation refresh=0.2 color='255 0 255' position='1025 350' size='200 220' valueswidth ='100'text=dark");
	TwAddVarRW(SimGUI, "timer",			TW_TYPE_FLOAT, &computestruct.timer,	"step=1.0 min=0 max= 100000");
	TwAddVarCB(SimGUI, "run sim.",		TW_TYPE_BOOL32, simset, simget,			NULL, NULL);
	TwAddVarRW(SimGUI, "gravity",		TWvec3, &computestruct.g,				NULL);
	TwAddVarRW(SimGUI, "dt",			TW_TYPE_FLOAT, &computestruct.dt,		"step=0.000001	min=0.000001 max= 0.10");
	TwAddVarRW(SimGUI, "iter",			TW_TYPE_INT32, &computestruct.iter,		"step=10		min=10 max= 100000");
	TwAddVarRW(SimGUI, "max fps",		TW_TYPE_INT32, &computestruct.maxframes,"step=1			min=1 max= 100");
	TwAddVarRW(SimGUI, "fps",			TW_TYPE_FLOAT, &fps,	"step=0.000001");
	TwAddVarRW(SimGUI, "tframe",		TW_TYPE_FLOAT, &tf,		"step=0.000001");
	TwAddVarRW(SimGUI, "tot K",			TW_TYPE_FLOAT, &totk,	"step=0.000001");
	TwAddVarRW(SimGUI, "tot L",			TW_TYPE_FLOAT, &totl,	"step=0.000001");
	TwAddVarRW(SimGUI, "tot U",			TW_TYPE_FLOAT, &totu,	"step=0.000001");
//////////////////// Models
	TwBar* ModelGUI = TwNewBar("Models");
	TwDefine("Models label = 'Models edit'");
	TwDefine(" Models refresh=0.05 color='255 255 0' position='1225 70'  size='200 380' text=dark");
	TwAddVarCB(ModelGUI, "Type",		TW_TYPE_INT32, Typeset, Typeget,		NULL,	"step=1.0	min=0 max= 30");
	TwAddVarCB(ModelGUI, "mesh",		TW_TYPE_INT32, Meshset, Meshget,		NULL,	"group=SURFACE step=1		min=0 max= 10");
	TwAddVarCB(ModelGUI, "texture",		TW_TYPE_INT32, Textureset, Textureget,	NULL,	"group=SURFACE step=1		min=0 max= 10");
	TwAddVarCB(ModelGUI, "Color",		TW_TYPE_COLOR4F, Colorset, Colorget,	NULL,	"group=SURFACE");
	TwAddVarCB(ModelGUI, "Emis. C.",	TW_TYPE_FLOAT, Emissiveset, Emissiveget,NULL,	"group=SURFACE step=0.01	min=0 max= 1");
	TwAddVarCB(ModelGUI, "Light S.",	TW_TYPE_FLOAT, Specularset, Specularget,NULL,	"group=SURFACE step=0.01	min=0 max= 1");
	TwAddVarCB(ModelGUI, "Pow.  S.",	TW_TYPE_FLOAT, Spowerset, Spowerget,	NULL,	"group=SURFACE step=1.0		min=0 max= 30");
	TwAddVarCB(ModelGUI, "Ref. Index.",	TW_TYPE_FLOAT, Refindexset, Refindexget, NULL,	"group=SURFACE step=0.01	min=0 max= 2.0");
	TwDefine("Models/SURFACE opened=false");
	TwAddVarCB(ModelGUI, "obj",			TW_TYPE_INT32, objset, objget,			NULL,	NULL);
	TwAddVarCB(ModelGUI, "raystep",		TW_TYPE_FLOAT, Rayset, Rayget,			NULL,	"step=0.01	min=0.01 max= 1");
	TwAddVarCB(ModelGUI, "Hidden",		TW_TYPE_BOOL8, hiddenset, hiddenget,	NULL,	NULL);
	TwAddVarCB(ModelGUI, "Intersect",	TW_TYPE_INT32, intersectset, intersectget,NULL, NULL);
	TwAddVarCB(ModelGUI, "dim",			TWvec3, dimset, dimget,					NULL,	"opened=false");
	TwAddVarCB(ModelGUI, "center",		TWvec3, centerset, centerget,			NULL,	"opened=false");
	TwAddVarCB(ModelGUI, "orient",		TWvec4, orientset, orientget,			NULL,	"opened=false");
	TwAddVarRW(ModelGUI, "euler",		TWvec3, &retang,								"opened=false");
	TwAddVarCB(ModelGUI, "speed",		TWvec3, speedset, speedget,				NULL,	"opened=false");
	TwAddVarCB(ModelGUI, "omega",		TW_TYPE_FLOAT, omegaset, omegaget,		NULL,	"step=0.1	min=-10 max= 10");
	TwAddVarRW(ModelGUI, "debug",		TW_TYPE_DIR3F, &debugv, "opened=false");
//////////////////// 
	TwBar * ModelfileGUI = TwNewBar("Mfile");
	TwDefine("Mfile label = 'Models file'");
	TwDefine(" Mfile position='1025 600' color='255 255 0' size='200 180' text=dark");
	TwAddVarCB(ModelfileGUI, "modnum",	TW_TYPE_INT32, modset, modget, NULL, "step=1	min=0 max= 9");
	TwAddButton(ModelfileGUI, "load models",Loadfile,	NULL, NULL);
	TwAddButton(ModelfileGUI, "copy",		Statcopy,	NULL, NULL);
	TwAddButton(ModelfileGUI, "paste",		Statpaste,	NULL, NULL);
	TwAddButton(ModelfileGUI, "swap",		Statswap,	NULL, NULL);
	TwAddButton(ModelfileGUI, "insert",		Statinsert, NULL, NULL);
	TwAddButton(ModelfileGUI, "delete",		Statdelete, NULL, NULL);
	TwAddButton(ModelfileGUI, "save models",Savefile,	NULL, NULL);
//////////////////// Mesh
	TwBar* MeshfileGUI = TwNewBar("Meshfile");
	TwDefine(" Meshfile refresh=0.05 color='100 255 0' position='1225 450'  size='200 330' text=dark");
	TwAddVarCB(MeshfileGUI, "mesh",			TW_TYPE_INT32, Meshsindset, Meshindget,		NULL, "step=1	min=0 max= 9");
	TwAddVarCB(MeshfileGUI, "meshstart",	TW_TYPE_INT32, Meshstartset, Meshstartget,	NULL, "step=1	min=0 max= 100");
	TwAddVarCB(MeshfileGUI, "meshstop",		TW_TYPE_INT32, Meshstopset, Meshstopget,	NULL, "step=1	min=0 max= 100");
	TwAddVarCB(MeshfileGUI, "mesh num",		TW_TYPE_INT32, Meshsnumset, Meshnumget,		NULL, "step=1	min=0 max= 100");
	TwAddVarCB(MeshfileGUI, "V0",			TWvec4, V0set, V0get,						NULL, "opened=false");
	TwAddVarCB(MeshfileGUI, "V1",			TWvec4, V1set, V1get,						NULL, "opened=false");
	TwAddVarCB(MeshfileGUI, "V2",			TWvec4, V2set, V2get,						NULL, "opened=false");

	TwAddButton(MeshfileGUI, "save mesh",	Savemesh, NULL, NULL);
	TwAddButton(MeshfileGUI, "load mesh",	Loadmesh, NULL, NULL);
}
//////////////////// 
vec3 vec43(vec4 v){	return vec3(v.x, v.y, v.z);}
//////////////////// 
void setmodels(int modnum)
{
	modelload(modnum);
	setmeshes();
	setload(modnum);
};
////////////////////
void setmeshes()
{
	float* meshread = (float*)malloc(sizeof(float));
	////////// Read the mesh from the file
	std::ifstream stream("meshes.txt", std::ios::in);
	std::string Line = "";
	if (stream.is_open()) {
		fnameprint("Loading", "meshes.txt", 0);  getline(stream, Line);
	}
	else  
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", "cube.txt"); 
	int nummeshes = stoi(Line, 0);
	int count = 0;
	for(int i = 0; i < nummeshes; i++)
	{ 
		getline(stream, Line);
		char* name = new char[Line.length() + 1];
		std::strcpy(name, Line.c_str());
		std::string textin;
		std::ifstream streamdata(Line, std::ios::in);
		if (streamdata.is_open())
		{
			fnameprint("Loading", Line.c_str(), 0);
			getline(streamdata, textin);
		}
		else printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", "cube.txt");

		meshlist[i].meshstart = count / 16;
		int allocsize = (int)textin.size()/6;
		meshread = (float*)realloc(meshread, (count + allocsize) * sizeof(float));
		std::string::size_type sz = 0;     // alias of size_t
		std::string::size_type inc = 0;     // alias of size_t
		for (int y = count; sz + inc < textin.size(); y++)
		{
			meshread[y] = (stof(textin.substr(sz), &inc)); 
			printf(" %f", meshread[y]);
			sz = sz + inc;
			count++;
		}
		printf("\n");
		meshlist[i].meshstop = count / 16;
		streamdata.close();
	}
	stream.close(); 
	meshsize = count * sizeof(float);
	meshread = (float*)realloc(meshread, count * sizeof(float));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Meshbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshsize, meshread, GL_DYNAMIC_COPY);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, meshsize, &mesh);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Meshbuffer);
	free(meshread);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Meshlistbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 20 * sizeof(MESHlist), meshlist, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, Meshlistbuffer);
};
////////////////////
void settextures()
{
	std::ifstream stream("textures.txt", std::ios::in);
	std::string Line = "";
	if (stream.is_open()) { 
		fnameprint("Loading", "textures.txt", 0);  getline(stream, Line); }
	else { printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", "cube.txt"); getchar(); }
	int numfiles = stoi(Line, 0);
	textinfo info;
	GLint width, height, numBytes, totalbytes;
	totalbytes = 0;
	unsigned char* data;
	float* fpixels = (float*)malloc(sizeof(float)); // allocate image data into RAM
	for (int file = 0; file < numfiles; file++)
	{
		getline(stream, Line);
		char* name = new char[Line.length() + 1];
		std::strcpy(name, Line.c_str());

		info = my_loadBMP(name);
	
		delete[] name;
		data = info.data;
		width = info.width; height = info.height;
		numBytes = width * height * 4;
		textures[file + 1].width = (float)width;
		textures[file + 1].height = (float)height;
		textures[file + 1].start = (int)totalbytes / 4;
		totalbytes += numBytes;
		fpixels = (float*)realloc(fpixels, totalbytes * sizeof(float));
		static int count = 0;
		for (int i = 0; i < width * height * 3; i += 3)
		{
			fpixels[count + 2] = float(*(data + i)) / 255.0f;
			fpixels[count + 1] = float(*(data + i + 1)) / 255.0f;
			fpixels[count] = float(*(data + i + 2)) / 255.0f;
			count += 4;
		}
		delete[] data;
	}
	stream.close();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Tex0buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, totalbytes * sizeof(float), fpixels, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, Tex0buffer);
	free(fpixels);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Texturesbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 20 * sizeof(Text), &textures, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, Texturesbuffer);
};
////////////////////
void fnameprint(string action, string fname, int num)
{
	TCHAR  buffer[1024] = TEXT("");
	if (GetFullPathName(fname.c_str(), 1024, buffer, NULL) != 0)
		fprintf(stdout, "%s %s\n", action.c_str(), fname.c_str());
	else
		fprintf(stdout, " File not found %s\n", fname.c_str());
	return;
}
////////////////////
void startsave()
{
	FILE* fp;
	string fname = "start"; 
	globals.fnum = fnum;
	globals.modnum = modnum;
	if (fopen_s(&fp, fname.c_str(), "wb") == 0)
	{
		fnameprint("Saving", fname, 0);
		fwrite(&globals, sizeof(Globs), 1, fp);
		fclose(fp);
	}
	else
		fnameprint("Error saving ", fname, 0);
	return;
}
////////////////////
void modelsave(int modnum){
	FILE* fp;
	char fname[6] = "save";
	int storetype = 0;
	fname[4] = char(int(48 + modnum) + storetype);
	fopen_s(&fp, fname, "wb");

	//for (int i = 0; i < MODNUM; i++)
	//{
	//	model[i].invorient	= conjugate(model[i].orient);
	//}
	model[0].fnum = fnum;
	for (int i = 0; i < MODNUM; i++) fwrite(&model[i], sizeof(Model), 1, fp);
	fclose(fp);
}
////////////////////
void startload()
{
	string fname = "start";
	FILE* fp = fopen(fname.c_str(), "r");
	if (fp != 0)
	{
		fnameprint("Loading", fname, 0);
		fread(&globals, sizeof(Globs), 1, fp);

		fclose(fp);
	}
	modnum = globals.modnum;
}
////////////////////
void modelload(int modnum) {
	FILE* fp;
	char fname[6] = "save";
	int storetype = 0;
	fname[4] = char(int(48 +  modnum) + storetype);
	if (fopen_s(&fp, fname, "r") == 0)
	{
		for (int i = 0; i < MODNUM; i++)
		{
			fread(&model[i], sizeof(Model), 1, fp);
			model[i].objnum = i;
			//model[i].raystep = 0.2f;
		}
		fclose(fp);
		fnum = model[0].fnum;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modelbuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, MODNUM * sizeof(Model), &model);

		return;
	}
	else
	{
		fprintf(stderr, "****************\n");
		fprintf(stderr, "Failed file open\n");
		fprintf(stderr, "Failed file open\n");
		fprintf(stderr, "Failed file open\n");
		fprintf(stderr, "Failed file open\n");
	}
}
////////////////////
void meshsave(int type) {
	std::ifstream stream("meshes.txt", std::ios::in);
	std::string Line = "";
	if (stream.is_open())  getline(stream, Line); else  printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", "cube.txt");
	int nummeshes = stoi(Line, 0);
	for (int thismesh = 0; thismesh < nummeshes; thismesh++)
	{
		string buffer;
		for (int thisvector = meshlist[thismesh].meshstart; thisvector < meshlist[thismesh].meshstop; thisvector++)
		{
			vec4 temp = mesh[thisvector].v0;
			buffer = buffer + to_string(temp.x) + " " + to_string(temp.y) + " " + to_string(temp.z) + " 0.000000 " ; 
			temp = mesh[thisvector].v1;
			buffer = buffer + to_string(temp.x) + " " + to_string(temp.y) + " " + to_string(temp.z) + " 0.000000 ";
			temp = mesh[thisvector].v2;
			buffer = buffer + to_string(temp.x) + " " + to_string(temp.y) + " " + to_string(temp.z) + " 0.000000 ";
			buffer = buffer + "0.000000 0.000000 0.000000 0.000000 ";
		}
		char* charbuffer = new char[buffer.length() + 1];
		std::strcpy(charbuffer, buffer.c_str());

		getline(stream, Line);
		std::ofstream streamdata(Line);
		if (streamdata.is_open())
		{
			printf("Saving %s.\n", Line.c_str());
			streamdata << buffer;
		}
		else
		{
			printf("Impossible to open %s. \n", Line.c_str());
			return;
		}
		streamdata.close();
	}
	stream.close();
}
////////////////////
void meshload(int type) {
	setmeshes(); 
	refresh();
}
////////////////////
void setsave(int fnum) {
	FILE* fp;
	char fname[6] = "sets";
	int storetype = 0;
	fname[4] = char(int(48 + fnum) + storetype);
	fopen_s(&fp, fname, "wb");
	fwrite(&computestruct, sizeof(Ambient), 1, fp);
	fclose(fp);
}
////////////////////
void setload(int fnum) {
	FILE* fp;
	char fname[6] = "sets";
	int storetype = 0;
	fname[4] = char(int(48 + fnum) + storetype);
	if (fopen_s(&fp, fname, "r") == 0)
	{
		fread(&computestruct, sizeof(Ambient), 1, fp);
		fclose(fp);
		invcamrot = inverse(computestruct.camrot);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(computestruct), &computestruct, GL_DYNAMIC_COPY);
		return;
	}
	else
	{
		fprintf(stderr, "****************\n");
		fprintf(stderr, "Failed file open\n");
		fprintf(stderr, "Failed file open\n");
		fprintf(stderr, "Failed file open\n");
		fprintf(stderr, "Failed file open\n");
	}
}
////////////////////
void updatemodel(int ptr, int index) {
	int offset = ptr * sizeof(Model);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modelbuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Model), &model[ptr]);
}
////////////////////
void updatemodels(int index) {
	if (index == 0)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modelbuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(model), &model, GL_DYNAMIC_COPY);
	}
	else
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modelbuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(model), &modeltemp, GL_DYNAMIC_COPY);
	}
}
////////////////////
void updatemesh(int ptr, int index) {
	int offset = ptr * sizeof(Mesh);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Meshbuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Mesh), &mesh[ptr]);
}
////////////////////
void updatemeshes() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Meshbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Mesh), &mesh, GL_DYNAMIC_COPY);
}
////////////////////
void updatcompute() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Computebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(computestruct), &computestruct, GL_DYNAMIC_COPY);
}
////////////////////
void updatemeshlist()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Meshlistbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Mesh), &meshlist, GL_DYNAMIC_COPY);
}
//////////////////// 
GLint status;
GLuint Compile(const char* compute_file_path) {
	GLuint retval = LoadComputes(compute_file_path);
	glGetProgramiv(retval, GL_LINK_STATUS, &status);
	return retval;
}
GLuint programview(char* vertex_file_path, const char* fragment_file_path) {
	GLuint retval = LoadShaders(vertex_file_path, fragment_file_path);
	glGetProgramiv(retval, GL_LINK_STATUS, &status);
	return retval;
}
//////////////////// 

void tonewmodel()
{
	Modelnew modelnew[MODNUM];
	Modelnew modeltest[MODNUM];
	FILE* fpr;
	char frname[6] = "save";
	for (int y = 0; y < 10; y++)
	{ 
		frname[4] = char(int(48 + y) + 0);
		fopen_s(&fpr, frname, "r");
		for (int i = 0; i < MODNUM; i++) fread(&model[i], sizeof(Model), 1, fpr);
		fclose(fpr);
		for (int i = 0; i< MODNUM; i++)
		{
			modelnew[i].centre		= model[i].centre;				modelnew[i].hidden		= model[i].hidden;
			modelnew[i].speed		= model[i].speed;				modelnew[i].mtype		= model[i].mtype;
			modelnew[i].spin		= model[i].spin;				modelnew[i].omega		= model[i].omega;		
			modelnew[i].mdim		= model[i].mdim;				modelnew[i].text		= model[i].text;
			modelnew[i].rotcentre	= model[i].rotcentre;			modelnew[i].mass		= model[i].mass;
			modelnew[i].orient		= model[i].orient;
			modelnew[i].invorient	= model[i].invorient;
			modelnew[i].objnum		= model[i].objnum;				modelnew[i].meshstart	= model[i].meshstart;
			modelnew[i].meshstop	= model[i].meshstop;			modelnew[i].mmesh		= model[i].mmesh;
			modelnew[i].EmissiveColor = model[i].EmissiveColor;		modelnew[i].Specular	= model[i].Specular;
			modelnew[i].Spower		= model[i].Spower;				modelnew[i].intersect	= model[i].intersect;
			modelnew[i].color		= model[i].color; 
		}
		FILE* fpw;
		char fwname[6] = "save";
		fwname[4] = char(int(48 + y) + 0);
		fopen_s(&fpw, fwname, "wb");
		for (int i = 0; i < MODNUM; i++) fwrite(&modelnew[i], sizeof(Modelnew), 1, fpw);
		fclose(fpw);
	}
}
void gpugassim()
{
	float frms = 0;
	float run = 0;
	if (sim)
	{
		float tf_ = 1.0f / computestruct.maxframes;
		glfwSetTime(0);
		updatcompute();
		updatemodels(0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Modeltemp);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(model), &model, GL_DYNAMIC_COPY);
		do
		{
			totk = 0; totl = 0;
			glUseProgram(Spare0ID);
			for (int i = 0; i < computestruct.iter / 1.0f; i++)
			{
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modelbuffer);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Modeltemp);
				glDispatchCompute(computestruct.objs, 1, 1);
				//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, Modeltemp);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, Modelbuffer);
				glDispatchCompute(computestruct.objs, 1, 1);
				//glDispatchCompute(1, 1, 1);
				//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				run++;
			}
			do
			{
				tf = (float)glfwGetTime();
			} while (tf < tf_);
			glfwSetTime(0);
			recalc();
			display();
			if (ctrl.goprotarctor)  axesdisp();
			{
				frms++;
				printf("%8.0f %8.0f %15.6f \n", frms, run, tf);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, Modelbuffer);		// energy simulation
				glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(model), &model);
				for (int i = 0; i <= computestruct.objs; i++)
					if (model[i].mtype == 1)
					{
						totk += 0.5f * dot(model[i].speed, model[i].speed);
						totl -= computestruct.g.y * model[i].centre.y;
						totu = totk + totl;
					}
			}
			TwDraw();
			glfwSwapBuffers(mainwindow);
			glfwPollEvents();
			computestruct.timer -= tf_;
		} while ((computestruct.timer > 0) && sim && (glfwWindowShouldClose(mainwindow) == 0));
		sim = false;
		updatemodels(0);
	}
}
/////////////////////////////