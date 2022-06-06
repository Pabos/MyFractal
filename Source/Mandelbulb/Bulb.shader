

#version 440 core
layout (local_size_x = 32, local_size_y = 20, local_size_z = 1 ) in;
layout(std430, binding = 0) buffer Positionbuffer	{ vec4 Position[800][1024]; };
layout(std430, binding = 1) buffer Coordbuffer		{ vec2 Coord[800][1024]; };

layout(std430, binding=2) buffer Computebuffer
{
	int steps;		int slice;		float zoom;		int type;
	vec3 eye;										float dmax;
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
	float bailout;	float minbail;	int proj;		float dum6;
	float bail0;	float bail1;	float bail2;	float bail3;
	vec3 LightDir;									float AmbLight;
	vec3 col1;										float LightCol;
	vec3 col2;										float LightSpc;
	float EmisCol;	float SpecPwr; float sine;		float dum7;
	vec3 FogColor;									float FogEn;
	bool ccol;		float occrange;	int frames;		float sincos;
	int b0;			int b1;			int b2;			float b3;
	vec3 dispang;									float Otrap;
	vec3 space;										float dum8;
	vec3 colcentre;									float dum9;
	uint MinMax[4];
	vec4 dum11;
	vec4 dum12;
};

void rot(inout vec3 v, in vec3 c){
	float theta		= phase * atan(v.y , v.x);
    float phi		= phase * atan(v.z, length(v.xy));
	float cosphi	= cos(phi);
	v				= pow(length(v), order) * vec3(cos(theta) * cosphi, sin(theta) * cosphi, -sin(phi)) + c;
};

uint gx = gl_GlobalInvocationID.x;
uint gy = gl_GlobalInvocationID.y;

void main()
{
	vec3 coord	= zoom * (vec3(Coord[gy * steps + (800 / numsteps) * slice][gx * steps], 0));
	vec3 rayt	= normalize((wx * coord.x + wy * coord.y) - raycam);
	float mbdist, lv0, found = 0;
	vec3 v0, vret, vc;
	float b = dot(eye, rayt);
	float c = dot(eye, eye) - 4;
	float d = b * b - c;
	if (d > 0)
	{
		float tN = (-b - sqrt(d));
		float tF = (-b + sqrt(d));
		int	iteration = 0;
		mbdist = max(tN, space.x);
		while (mbdist < tF) 
		{
			mbdist		+= accuracy;
			v0			= eye + rayt *(mbdist);
			vret		= v0;
			lv0			= 0;
			if (julia == false) vc =	v0; else vc =juliav;
			for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration +=1) 
			{	
				rot(v0, vc);
				lv0		= length(v0); 
			}
			if (iteration >= maxiter) 
			{	
				found	= 1;
				float	binary = 0.5f;
				mbdist -= accuracy * binary;
				for (int i = 0; i < 16; i++)
				{
					v0 = eye + rayt * (mbdist);
					vret = v0;
					lv0 = 0;
					if (julia == false) vc = v0;	else	vc = juliav;
					for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration++)
					{
						rot(v0, vc);
						lv0 = length(v0);
					}
					binary /= 2.0f;
					if (iteration >= maxiter)	mbdist -= accuracy * binary;
					else						mbdist += accuracy * binary;
				}
				// find occlusion  ////////////////////////////
				if (occrange > 0)
				{
					mbdist = accuracy;
					rayt = normalize(LightDir);
					while (mbdist < 2)
					{
						mbdist += accuracy;
						v0 = vret - rayt * mbdist;
						lv0 = 0;
						vc = v0;

						for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration += 1)
						{
							rot(v0, vc);
							lv0 = length(v0);
						}
						if (iteration >= maxiter)
						{
							found = -1;
							break;
						}
					}
				}
				break; 
			}
		}
	}
	float l = length(vret - colcentre);
	Position[gy+ (800 / numsteps) * slice][gx] = vec4(vret, l * found);
	found = abs(found);
	if (found == 1)
	{
		//barrier();
		atomicMax(MinMax[0], uint(l * 100000));
		//barrier();
		atomicMin(MinMax[1], uint(l * 100000));
		uint camerafound =  uint(length(eye - vret) * 100000);
		//barrier();
		atomicMax(MinMax[2], camerafound);
		atomicMin(MinMax[3], camerafound);
	}
}