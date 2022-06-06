

#version 440 core
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
	vec3 juliav;									float julia;
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
bool found = false;
float found_ = 0;
float binary = 0.5f;
int iteration;
int iteration1;
float	mr2	= mr * mr;
float 	fr2	= fr * fr;
//----------------------------------------------------------------------------
bool searchloop(float x,float y,float z)
{
	float pigs;
	pigs = radians(180);
	float f=KleinI<0 ?-1:1 ;
	for (int i=0; i < maxiter1; i++)
	{
		float vxx	= x + boxsizex;
		float vxz	= z + boxsizez;
		float vax	= foldx * boxsizex ;
		float vaz	= foldy * boxsizez;
		x			= vxx - vax * floor(vxx/vax) - boxsizex;
		y			-= KleinR * floor(y/KleinR);
		z			= vxz - vaz * floor(vxz/vaz) - boxsizez ;

		if(b0 == 1)
		{
			if (y >= KleinR * (0.5 + losScale * sin(f * pigs * x / boxsizex)))
			{
				x = -KleinI-x;
				y = KleinR-y;
				z = -z;
			}
		}
		float r		= x * x + y * y + z * z;
		if (r > 0)
		{
			x /= -r;
			y /= -r;
			z /= -r;
			if(r < minbail)return true;
			if(r > bailout)return true;
		}
		x = -KleinI -x;
		y = KleinR + y;
	}
	return false;
}
/////////////////
uint gx	=	gl_GlobalInvocationID.x;
uint gy	=	gl_GlobalInvocationID.y;
void main()
{
	vec3 coord = zoom * (vec3(Coord[gy * steps + (800 / numsteps) * slice][gx * steps], 0));
	vec3 rayt	= normalize((wx * coord.x + wy * coord.y) - raycam);

	vec3 eye = camera;
	vec3 m = 1/rayt;
	vec3 n = m * eye;
	vec3 k = abs(m) * space;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );

	float accuracy_ = accuracy * 5.0;
	float	mbdist	= tN;
	vec3	vret	= eye;
	if(tN < tF)
		while (mbdist < tF) 
		{
			mbdist	= mbdist + accuracy_;	
			vret = eye + rayt * mbdist;
			found = searchloop(vret.x, vret.y, vret.z);
			if (found == true)
			{
				found_ = 1;
				float raystep = -accuracy_ * binary;
				for (int i = 0; (i < 16); i++)
				{	
					vret	= vret + rayt * raystep;
					found	= searchloop(vret.x, vret.y, vret.z);
					binary	= binary - binary / 2.0f;
					if (found == true)	
						raystep = -accuracy_ * binary; 
					else 
						raystep = +accuracy_ * binary;
				}
				break;
			}	
		}
	float l = length(vret - colcentre);
	Position[gy + (800 / numsteps) * slice][gx] = vec4(vret, l * found_);
	if (found_ == 1)
	{
//		barrier();
		uint lfound = uint(l * found_ * 100000);
		atomicMax(MinMax[0], lfound);
		atomicMin(MinMax[1], lfound);
		uint camerafound =  uint(length(eye - vret) * found_ * 100000);
		atomicMax(MinMax[2], camerafound);
		atomicMin(MinMax[3], camerafound);
	}
}
//--------------------------------------------------------