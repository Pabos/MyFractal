
#version 440 core
#define LSIZEX 16
layout (local_size_x = 32, local_size_y = 20, local_size_z = 1 ) in;
layout(std430, binding = 0) buffer Positionbuffer	{ vec4 Position[800][1024]; };
layout(std430, binding = 1) buffer Coordbuffer		{ vec2 Coord[800][1024]; };

layout(std430, binding=2) buffer Computebuffer
{
	int steps;		int slice;		float zoom;		int type;
	vec3 camera;									float dmax;
	vec3 centerpos;									int numsteps;
	vec3 raycam;									float accuracy;
	vec3 wx;										float df;
	vec3 wy;										float order;
	vec3 wz;										float phase;
	float mr;		float fr;		float sc;		float dum1;
	float foldx;	float foldy;	float foldz;	float dum2;
	vec3 juliav;									bool julia;
	float boxsizex; float boxsizey; float boxsizez; float dum3;
	float KleinR;	float KleinI;	float kleinf;	float losScale;
	float maxiter;	float maxiter0;	float maxiter1;	float maxiter2;
	float miniter0;	float miniter1;	float miniter2;	float dum4;
	float bailout;	float minbail;	bool proj;		float dum6;
	float bail0;	float bail1;	float bail2;	float bail3;
	vec3 LightDir;									float AmbLight;
	vec3 col1;										float LightCol;
	vec3 col2;										float LightSpc;
	float EmisCol;	float SpecPwr; float sine;		float dum7;
	vec3 FogColor;									float FogEn;
	bool ccol;		float occrange;	int frames;		bool sincos;
	int b0;			int b1;			int b2;			float b3;
	vec3 dispang;									float Otrap;
	vec3 space;										float dum8;
	vec3 colcentre;									float dum9;
	uint MinMax[4];
	vec4 dum11;
	vec4 dum12;
};

vec3 rot(vec3 v0, float lv0, vec3 vectc)
{
	float theta		= phase * atan(v0.y , v0.x);
    float phi		= phase * atan(v0.z, length(v0.xy));
	float cosphi	= cos(phi);
	return			pow(lv0, order) * vec3(cos(theta)*cosphi, sin(theta)*cosphi, -sin(phi)) + vectc;
};
uint gx	=	gl_GlobalInvocationID.x;
uint gy	=	gl_GlobalInvocationID.y;
void main()
{
	vec3 coord	= zoom * (vec3(Coord[gy * steps + (800 / numsteps) * slice][gx * steps], 0));
	vec3 rayt	= normalize((wx * coord.x + wy * coord.y) - raycam);
	
	float	mbdist	= 0, dedist	= 0, lv0 = 0;
	int		found	= 0;
	float	mindist	= accuracy / (1.0 + 10.0 *  df);
	vec3 eye = camera;
	vec3 v0 = eye, vret;
	vec3 oc = v0;
	float b = dot(oc, rayt);
	float c = dot(oc,oc) - 4;
	float d = b * b - c;
	if (d > 0)	
	{
		float tN = (-b - sqrt(d));
		float tF = (-b + sqrt(d));
		mbdist = max(tN, space.x);
		if (proj == true)
		{
			eye = eye + rayt * (mbdist);
			rayt = -normalize(eye);
			mbdist = 0;
		}
		while (mbdist < tF)
		{
			mbdist 		+= accuracy * 20.0f * dedist;
			v0			= eye + rayt * mbdist;
			vret		= v0;
			lv0			= length(v0);
			float dr	= 1.0;
			if (julia == true)
			for (int iteration = 0; iteration < maxiter; iteration++)
			{
				if (lv0 > 2.0f) break;
				dr	= order * pow(lv0, order - 1.0f) * dr + 1.0f;
				v0	= rot(v0, lv0, juliav);
				lv0 = length(v0);
			}
			else
			for (int iteration = 0; iteration < maxiter; iteration++) 
			{	
				if (lv0 > 2.0f) break;
				dr	= order * pow(lv0, order - 1.0f) * dr + 1.0f;
				v0	= rot(v0, lv0, vret);
				lv0	= length(v0); 
			}
			dedist =  0.5 * (log(lv0)*lv0/dr);
			if (dedist < mindist){ found = 1; break;}
		}
	}
	float l = length(vret - colcentre);
	Position[gy + (800 / numsteps) * slice][gx] = vec4(vret, l * found);
	found = abs(found);
	if (found == 1)
	{
//		barrier();
		atomicMax(MinMax[0], uint(l * 100000));
		atomicMin(MinMax[1], uint(l * 100000));
		uint camerafound =  uint(length(eye - vret) * found * 100000);
		atomicMax(MinMax[2], camerafound);
		atomicMin(MinMax[3], camerafound);
	}
}