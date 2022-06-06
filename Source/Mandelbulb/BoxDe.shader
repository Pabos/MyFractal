
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
	float bailout;	float minbail;	float dum5;		float dum6;
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
//-----------------------------------------------------------------------------

struct rotout
{
	vec3 v0;
	float  de;
};
float	mr2	= mr * mr;
float 	fr2	= fr * fr;
rotout rot(vec3 v0, float de)
{	
	rotout result;
	v0			= sign(v0) * (1.0 - abs(abs(v0) - 1.0));
	float r2	= dot(v0, v0);
	if (r2 < mr2)
	{
		v0		*= fr2 / mr2;
        de		*= fr2 / mr2;
	} 
	else if (r2 < fr2)
	{
		v0		*= fr2 / r2; 
		de		*= fr2 / r2;
	} 
	result.v0 = v0;
	result.de = de;
	return		result;
};
//-----------------------------------------------------------------------------

uint gx	=	gl_GlobalInvocationID.x;
uint gy	=	gl_GlobalInvocationID.y;
void main()
{
	vec3 coord = zoom * (vec3(Coord[gy * steps + (800 / numsteps) * slice][gx * steps], 0));
	vec3	rayt	= normalize((wx * coord.x + wy * coord.y) - raycam);
		
	vec3 m = 1/rayt;
	vec3 eye = camera;
	vec3 n = m * eye;
	vec3 k = abs(m) * (space);
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
    float tN = max(max( max( t1.x, t1.y ), t1.z ), 0);
    float tF = min( min( t2.x, t2.y ), t2.z );

	float mbdist = tN;
	float	dedist	= 0;
	vec3	v0		= eye;
	vec3	vret	= v0;
	float	lv0		= 0;
	int		found	= 0;
	float	temp;
    int		iteration	= 0;
	float count = 0;
	
	if(tN < tF)
		while (mbdist < tF) 
		{
			float de	= 1;
			mbdist 		+= accuracy * 20.0f * dedist;
			v0			= eye + rayt * mbdist;
			vret		= v0;
			lv0			= length(v0);
			rotout	vrot;
			for (iteration = 0; iteration < maxiter; iteration++) 
			{	
				if (lv0 > 12.0f) break;//
				vrot	= rot(v0, de);
				if (!julia == true)
					v0		= vrot.v0 * sc + vret;
				else
					v0		= vrot.v0 * sc + juliav;
				de		= vrot.de * sc;
				lv0			= length(v0);
			}
				count ++;
			dedist		= lv0 / abs(de);
			float	mindist	= 2.0f * accuracy / (1.0 + df);
			if (dedist < mindist)	{found = 1; break;}
			//if ((dedist < 0.1f/(1 + 4.0f * df)) ) {found = 1; break;}
			////if ((dedist < 0.1f/(1 + 4.0f * df)) && (lv0 < 12)) {found = 1; break;}
			////if (lv0 < 12) {found = 1; break;}
		}
	float l = length(vret - colcentre);
	Position[gy + (800 / numsteps) * slice][gx] = vec4(vret, l * found);
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
//-----------------------------------------------------------------------------