
#version 440 core
#define LSIZEX 16
layout(local_size_x = 32, local_size_y = 20, local_size_z = 1) in;
layout(std430, binding = 0) buffer Positionbuffer	{ vec4 Position[800][1024]; };
layout(std430, binding = 3) buffer Fragments		{ vec4 incol[800][1024]; };
layout(std430, binding = 5) buffer Fragmentsi		{ vec4 outcol[800][1024]; };
layout(std430, binding = 2) buffer Computebuffer
{
	int steps;		int step;		float zoom;		int type;
	vec3 camera;									float dmax;
	vec3 centerpos;									float dum0;
	vec3 raycam;									float accuracy;
	vec3 wx;										float df;
	vec3 wy;										float order;
	vec3 wz;										float phase;
	float mr;		float fr;		float sc;		float focal;
	float foldx;	float foldy;	float flodz;	float depth;
	vec3 juliav;									float julia;
	float boxsizex; float boxsizey; float boxsizez; float dum3;
	float KleinR;	float KleinI;	float kleinf;	float losScale;
	float maxiter;	float maxiter0;	float maxiter1;	float maxiter2;
	float miniter0;	float miniter1;	float miniter2;	int span;
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
	vec3 space;										int Rspeed;
	vec3 colcentre;									int Tspeed;
	uint MinMax[4];
	vec4 dum11;
	vec4 dum12;
};

uint gx = gl_GlobalInvocationID.x + 4;
uint gy = gl_GlobalInvocationID.y + 4;

void main()
{
	vec3 colout = vec3(0, 0, 0);
	float weight = 0;

	uint gx = gl_GlobalInvocationID.x;
	uint gy = gl_GlobalInvocationID.y;
	uint index = gy * 1024 + gx;
	vec4 p0 = Position[gy][gx];
	float minmax = 50.0f * (MinMax[2] - MinMax[3]) / 100000;
	float focus = abs((length(camera - p0.xyz) - focal) * (10 - depth) / (minmax));
	float focus_;
	for (int i = -span; i <= span; i++)
		for (int y = -span; y <= span; y++)
		{
			if (i == 0 && y == 0)	
				focus_ = 1 - focus;
			else	
				focus_ = focus;
			weight += focus_;
			colout += focus_ * incol[gy + y][gx + i].xyz;
		}
	outcol[gy][gx] = vec4(colout / weight, 1);
}