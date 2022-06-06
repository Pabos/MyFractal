
#version 440 core
#define LSIZEX 16
layout (local_size_x = 32, local_size_y = 20, local_size_z = 1 ) in;
layout(std430, binding=0) buffer Positionbuffer	{ vec4 Position[800][1024]; };
layout(std430, binding = 1) buffer Coordbuffer  { vec2 Coord[800][1024]; };

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

vec3 rot(vec3 v0, vec3 vectc)
{
	float theta		= phase * atan(v0.y , v0.x);
    float phi		= phase * atan(v0.z , length(v0.xy));
	float cosphi	= cos(phi);
	float sinphi	= sin(phi);
	float costheta	= cos(theta);
	float sintheta	= sin(theta);
	return		(pow(length(v0), order) * vec3(costheta*cosphi, sintheta*cosphi, -sinphi) + vectc);
};
uint gx	=	gl_GlobalInvocationID.x;
uint gy	=	gl_GlobalInvocationID.y;
void main()
{		
//	#define compil	
#ifdef compil


	vec3 coord = zoom * (vec3(Coord[gy * steps + (800 / numsteps) * slice][gx * steps], 0));
	vec3 rayt = normalize((wx * coord.x + wy * coord.y) - raycam);
	float	mbdist = 0, lv0 = 0, found = 0;
	vec3 eye = camera;
	vec3 v0 = eye, vret, vc;
	vec3 oc = v0;
	float b = dot(oc, rayt);
	float c = dot(oc, oc) - 4;
	float d = b * b - c;
	if (d > 0)
	{
		float tN = (-b - sqrt(d));
		float tF = (-b + sqrt(d));
		int	iteration = 0;
		mbdist = tN;
		eye = eye + rayt * (mbdist);
		rayt = -normalize(eye);
		mbdist = 0;
		while (mbdist < tF) 
		{
			mbdist += accuracy;
			v0 = eye + rayt * (mbdist);
			vret = v0;
			lv0 = length(v0);
			if (julia == false) vc = v0; else vc = juliav;
			for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration += 1)
			{
				v0 = rot(v0, vc);
				lv0 = length(v0);
			}
			if (iteration >= maxiter) 
			{	found = 1; break; }
		}
		float	binary = 0.5f;
		if (found == 1)
		{
			mbdist -= accuracy * binary;
			for (int i = 0; i < 10; i++)
			{	v0			= eye + rayt * (mbdist);
				vret		= v0;	
				lv0			= length(v0);
				for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration++) 
				{
					v0		= rot(v0, vret);
					lv0		= length(v0);
				}
				binary = binary / 2.0f;
				if (iteration >= maxiter) mbdist -= accuracy * binary; else mbdist += accuracy * binary;
			}
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
		uint camerafound = uint(length(eye - vret) * found * 100000);
		atomicMax(MinMax[2], camerafound);
		atomicMin(MinMax[3], camerafound);
	}



#else


	vec3 vertexpos = zoom * ((4.0f * steps / 1024.0f) * vec3(ivec2(gx, (800 / numsteps) * slice + gy), 0) - vec3(2.000f, 1.5625f, 0));
	vec3 v = vertexpos.xyz;
	v.x = (vertexpos - raycam).y * 3.14f / 4.0f;
	v.y = (vertexpos - raycam).x * 3.14f / 2.0f;
	vec3 rayt = vec3(cos(v.y) * cos(v.x), sin(v.y) * cos(v.x), -sin(v.x));
	float	mbdist = 0;
	vec3	v0 = 2.0f * normalize(rayt);
	vec3	vret = v0;
	float	lv0 = 0;
	int		found = 0;
	float	temp;
	int		iteration = 0;
	int count1 = 0;
	while (length(vret) > 0.1f) 
	{
		count1++;
		mbdist		+= accuracy;
		v0			= 2.0f * normalize(rayt) * (1 - mbdist);
		vret		= v0;
		lv0			= length(v0);
		for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration +=1) 
		{	
			v0		= rot(v0, vret);
			lv0		= length(v0); 
		}
		if (iteration >= maxiter) {	found = 1; break; }
	}

	float	binary = 0.5f;
	if (found == 1)
	{
		mbdist -= accuracy * binary;
		for (int i = 0; i < 10; i++)
		{	v0			= 2.0f * normalize(rayt) * (1 - mbdist);
			vret		= v0;	
			lv0			= length(v0);
			for (iteration = 0; (lv0 < 2.0f) && (iteration < maxiter); iteration++) 
			{
				v0		= rot(v0, vret);
				lv0		= length(v0);
			}
			binary = binary / 2.0f;
			if (iteration >= maxiter) mbdist -= accuracy * binary; else mbdist += accuracy * binary;
		}
	}
	float l	= length(camera - vret);
	Position[gy + (800 / numsteps) * slice][gx] = vec4(vret, l * found);
	atomicMax(MinMax[0], uint(l * found * 100000));
	barrier();
	if (found == 1)
	{
		atomicMin(MinMax[1], uint(l * found * 100000));
		barrier();
	}
#endif
}