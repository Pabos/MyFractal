#ifndef KEYINPUT_HPP
#define KEYINPUT_HPP
#include <vector> 
using namespace std;

void simul();


struct Computestruct
{
	vec3 camera;	float dmin;
	vec3 raycam;	float dmax;
	vec3 wx;		float steps;
	vec3 wy;		float shadow1;
	float maxiter;	float frames;		int objs;			int intersect;

	vec3 LightDir;	int filter;			vec3 LightSource;	float AmbientLight;
	vec3 col1;		float LightColor;	vec3 col2;			float LightSpecular;
	vec3 FogColor;	float EmissiveColor; vec3 vcamera;		float SpecularPower;
	float Alpha;	float shadow;		int reflection;		float FogEnabled;
	float occwidth;	float depth;		int objstart;		int obj;
};

//#define compil
#ifdef compil
struct Modelnew
#else
struct Model
#endif
{
	vec3 centre;		int hidden;
	vec3 speed;			float mtype;
	vec3 spin;			float omega;
	vec3 mdim;			int text;
	vec3 rotcentre;		float mass;
	quat orient;
	quat invorient;
	int objnum;			int meshstart;		int meshstop;	int mmesh;
	float EmissiveColor; float Specular;	float Spower;	float intersect;
	vec4 color;
	vec2 debug;								float raystep;	float refindex;
	vec4 dum0;
	vec4 dum1;
	vec4 dum2;
	vec4 dum3;
	vec3 dum4;			int fnum;
};

#ifdef compil
struct Model
#else
struct Modelnew
#endif
{
	vec3 centre;		int hidden;
	vec3 rotcentre;		float mtype;
	vec3 mdim;			int text;
	vec4 color;
	mat4 rota;
	mat4 invrota;
	quat orient;
	vec4 raycam;
	vec3 wxa;			int meshstart;
	vec3 wya;			int meshstop;
	vec3 wza;			int mmesh;
	vec4 lightdir;
	float EmissiveColor; float Specular; float Spower; float intersect;
	vec3 speed;			float mass;
	vec3 spin;			float omega;
	vec4 debug;
	vec3 dum1;			int objnum;
	quat invorient;
	vec4 dum3;
	vec4 dum4;
	vec4 dum5;
	vec4 dum6;
	vec4 dum7;
};
////////////////////////////////
void snapshot(bool, int, int);

#endif

