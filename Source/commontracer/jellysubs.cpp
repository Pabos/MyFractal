
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

// Include GLFW
//#include <GL/glfw.h>
 
// Include GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
using namespace glm;

struct jelly
{
	float dt, friction, k, density, radius, spherespeed, spheremass;
	vec3 sphere;
	vec4 gravity;
};
struct jelly loadjelly(int index)
{
		jelly retval;
		char fname[13] = "jellysave";
		fname[9] =char(int(48) + index);
		ifstream inputfile;
		inputfile.open(fname, ios::in);
	if(inputfile.fail() | (index == 0))
	//		exit(1);
	//if (index == 0) 
	{
		retval.dt		= 0.0001f;
		retval.friction = 1.1000f;
		retval.k		= 1000.0f;
		retval.density	= 0.01f;
		retval.radius	= 10.0f;
		retval.spherespeed	= -10.0f;
		retval.spheremass	= 1000.0f;
		retval.sphere	= vec3(30.0f, 10, 10);
		retval.gravity		= vec4(-0.0f, 0, 0, 0); 
		return retval;
	}
	else
	{
		char fname[13] = "jellysave";
		fname[9] =char(int(48) + index);
		ifstream inputfile;
		inputfile.open(fname, ios::in);
		if(inputfile.fail())
			exit(1);
		string str;
		float temp;
		char text[30];
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.dt = (float)atof(text);
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.friction = (float)atof(text);
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.k = (float)atof(text);
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.density = (float)atof(text);
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.radius = (float)atof(text);
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.spherespeed = (float)atof(text);
		getline (inputfile,str);
		strcpy_s(text,str.c_str());
		retval.spheremass = (float)atof(text);
		for (int x = 0; x < 3; x++)
		{ 
			getline (inputfile,str);
			strcpy_s(text,str.c_str());
			retval.sphere[x] = (float)atof(text);
			getline (inputfile,str);
			strcpy_s(text,str.c_str());
			retval.gravity[x] = (float)atof(text);
		}
			getline (inputfile,str);
			strcpy_s(text,str.c_str());
			retval.gravity[3] = (float)atof(text);
		}
		inputfile.close();
		return retval;
}
#include "sstream"
void savejelly(float dt, float friction, float k, float density, vec3 sphere, float radius, 
					float spherespeed, float spheremass, vec4 gravity, int filen)
{
	char fname[13] = "jellysave";
	fname[9] =char(int(48) + filen);
	ostringstream outbuffer;
	
	outbuffer << (float)dt<<"\n";
	outbuffer << (float)friction<<"\n";
	outbuffer << (float)k<<"\n";
	outbuffer << (float)density<<"\n";
	outbuffer << (float)radius<<"\n";
	outbuffer << (float)spherespeed<<"\n";
	outbuffer << (float)spheremass<<"\n";
	for (int x = 0; x < 3; x++)
	{ 
		outbuffer << (float)sphere[x]<<"\n";
		outbuffer << (float)gravity[x]<<"\n";
	}
		outbuffer << (float)gravity[3]<<"\n";
	ofstream outputfile;
	remove(fname);
	outputfile.open(fname, ios::out|ios::trunc);
	if(outputfile.fail())
		exit(1);
	string str;
	char text[30];
	str = outbuffer.str();
	outputfile<<str;
	outputfile.close();
	if(outputfile.fail())
		exit(1);
}

