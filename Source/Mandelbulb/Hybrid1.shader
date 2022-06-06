
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
	float foldx;	float foldy;	float V0div;	float dum2;
	vec3 juliav;									bool julia;
	float boxsizex; float boxsizey; float boxsizez; float dum3;
	float KleinR;	float KleinI;	float kleinf;	float losScale;
	float Boxrotiter;	float Boxiter;	float Rotiter;	float Rotiter0;
	float miniter0;	float miniter1;	float miniter2;	float dum4;
	float Bailbox;	float Bailrot;	float dum5;		bool spacemode;
	float Bailrot0;	float bail1;	float bail2;	float bail3;
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

float	mr2	= mr * mr;
float 	fr2	= fr * fr;
float	Bailiter = 0;
vec3 mbox(vec3 v0)
{		
	v0			= sign(v0) * (1.0 - abs(abs(v0) - 1.0));

	float r2	= dot(v0, v0);
	if (r2 < mr2)
	{
		v0		*= fr2 / mr2;
	} 
	else if (r2 < fr2)
	{
		v0		*= fr2 / r2; 
	} 
	return		v0;
};
//-----------------------------------------------------------------------------
vec3 rot(vec3 v0){
	float theta		= KleinR * atan(v0.y , v0.x);
    float phi		= KleinR * atan(v0.z, length(v0.xy));
	float cosphi	= cos(phi);
	return			pow(length(v0), KleinI) * vec3(cos(theta)*cosphi, sin(theta)*cosphi, -sin(phi));
};
//-----------------------------------------------------------------------------
void boxrot(in vec3 v0, in vec3 vret, out int iteration){	
	if (Rotiter0 >= 0)
	{
		for (iteration = 0; (iteration <= Rotiter0) && (length(v0) < Bailrot0); iteration++)
			v0 = rot(v0) + b2 * vret;
		Bailiter = Rotiter0;
	}

	if (Boxiter >= 0)
	{
		for (iteration = 0; (iteration <= Boxiter) && (length(v0) < Bailbox); iteration++)
			v0 = mbox(v0) * sc + b0 * vret;
		Bailiter = Boxiter;
	}

	v0 = v0 / V0div;
	if (Rotiter >= 0)
	{
		for (iteration = 0; (iteration <= Rotiter) && (length(v0) < Bailrot); iteration++)
			v0	= rot(v0) + b1 * vret;
		Bailiter = Rotiter;
	}
};
uint gx	=	gl_GlobalInvocationID.x;
uint gy	=	gl_GlobalInvocationID.y;
//-----------------------------------------------------------------------------
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

	float	mbdist = tN;
	vec3	v0		= eye;
	vec3	vret	= v0;
	float	lv0		= 0;
	int		found	= 0;
    int		iteration	= 0;

	if (tN < tF)
	{
		while (mbdist < tF) 
		{
			mbdist	+= accuracy * 2.5f;
			v0		= eye + rayt * mbdist;
			vret	= v0;
			boxrot(v0, vret, iteration);
			if (iteration >= Bailiter) {found = 1; break;}
		}
		//
		if (found ==  1)
		{
			float binary = 0.5f;
			mbdist -= accuracy * 2.5f * binary;
			for (int i = 0; i < 32; i++)
			{	
				v0		= eye + rayt * mbdist;
				vret	= v0;
				boxrot(v0, vret, iteration);
				binary = binary - binary / 2.0f;
				if (iteration >= Bailiter)
					mbdist -= accuracy * 2.5f * binary; 
				else 
					mbdist += accuracy * 2.5f * binary;
			}
		}

	}
///////////////////////////////////////////


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

