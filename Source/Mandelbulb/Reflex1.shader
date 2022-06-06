
#version 440 core
#define LSIZEX 16
layout(local_size_x = 32, local_size_y = 20, local_size_z = 1) in;
layout(std430, binding = 0) buffer Positionbuffer { vec4 Position[800][1024]; };
layout(std430, binding = 3) buffer Fragments0 { vec4 fragcol[800][1024]; };
layout(std430, binding = 4) buffer Fragments1 { vec4 texcoord[800][1024]; };
layout(std430, binding = 5) buffer Fragments2 { vec4 outcol[800][1024]; };
layout(std430, binding = 6) buffer Fragments3 { vec4 Normal[800][1024]; };
layout(std430, binding = 2) buffer Computebuffer {
	int steps;		int step;		float zoom;		int type;
	vec3 eye;										float dmax;
	vec3 centerpos;									int numsteps;
	vec3 raycam;									float accuracy;
	vec3 wx;										float df;
	vec3 wy;										float order;
	vec3 wz;										float phase;
	float mr;		float fr;		float sc;		float dum1;
	float foldx;	float foldy;	float flodz;	float dum2;
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
	bool ccol;		float occrange;	int frames;		float sincos;
	int b0;			int b1;			int b2;			float b3;
	vec3 dispang;									float Otrap;
	vec3 space;										int Rspeed;
	vec3 colcentre;									int Tspeed;
	uint MinMax_[4];
	vec4 dum11;
	vec4 dum12;
};
//-----------------------------------------------------------------------------
uint gx = gl_GlobalInvocationID.x;
uint gy = gl_GlobalInvocationID.y;
vec3 Color;
float max_, min_, cmax, cmin;
//-----------------------------------------------------------------------------
struct Cpair { vec3 Diff;	vec3 Spec; };
Cpair ComputePerPixelLights(vec3 E, vec3 N, vec4 p0)
{
	Cpair result;
	result.Diff = vec3(AmbLight);
	result.Spec = vec3(0.0f);
	float Occl = 1.0f;
	if ((occrange > 0) && (p0.w < 0))
		Occl = 1.0f - occrange / 10.0f;
	vec3 L = normalize(LightDir);
	float dt = max(0, dot(N, L));
	result.Diff += vec3(LightCol) * dt * Occl;
	result.Diff *= vec3(EmisCol);
	vec3 H = normalize(E + L);
	if (dt != 0)
		result.Spec = vec3(LightSpc) * pow(max(0, dot(H, N)), SpecPwr);
	return result;
}
//-----------------------------------------------------------------------------
vec3 lerp(vec3 v1, vec3 v2, float amount)
{
	return v1 + (v2 - v1) * amount;
}
//-----------------------------------------------------------------------------
void main() {
	vec4 p0 = Position[gy][gx];
	vec4 p1 = Position[gy][gx + 1];
	vec4 p2 = Position[gy + 1][gx];
	max_ = float(MinMax_[0]) / 100000.0f; min_ = float(MinMax_[1]) / 100000.0f;  //color
	cmax = float(MinMax_[2]) / 100000.0f; cmin = float(MinMax_[3]) / 100000.0f;  //fog
	if ((p0.w * p1.w * p2.w) != 0.0f)
	{
		vec4 matcolor;
		vec3 posToEye = p0.xyz - eye;
		vec3 normal = normalize(cross((p2.xyz - p0.xyz), (p1.xyz - p0.xyz)));
		Cpair lightRes = ComputePerPixelLights(normalize(posToEye), normal, p0);
		Color = col1 * lightRes.Diff;
		if (outcol[gy][gx].w == 0) 
			Color = lerp(col1, FogColor, FogEn) * lightRes.Diff + lightRes.Spec;
		else
			Color = col1 * lightRes.Diff + lightRes.Spec;

		fragcol[gy][gx] = vec4(Color, 1);
	}
	else
	{
		fragcol[gy][gx] = vec4(FogColor * FogEn, 1);
	}
	outcol[gy][gx] = fragcol[gy][gx];
}

