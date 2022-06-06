
#version 440 core
#define LSIZEX 16
layout(local_size_x = 32, local_size_y = 20, local_size_z = 1) in;
layout(std430, binding = 0) buffer Positionbuffer { vec4 Position[800][1024]; };
layout(std430, binding = 1) buffer Coordbuffer { vec2 Coord[800][1024]; };
layout(std430, binding = 3) buffer Fragments0 { vec4 fragcol[800][1024]; };
layout(std430, binding = 4) buffer Fragments1 { vec4 texcoord[800][1024]; };
layout(std430, binding = 5) buffer Fragments2 { vec4 outcol[800][1024]; };
layout(std430, binding = 6) buffer Fragments3 { vec4 Normal[800][1024]; };
layout(std430, binding = 2) buffer Computebuffer
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
	vec3 juliav;									float julia;
	float boxsizex; float boxsizey; float boxsizez; float dum3;
	float KleinR;	float KleinI;	float kleinf;	float losScale;
	float maxiter;	float maxiter0;	float maxiter1;	float maxiter2;
	float miniter0;	float miniter1;	float miniter2;	float dum4;
	float bailout;	float minbail;	float dum5;		int spacemode;
	float bail0;	float bail1;	float bail2;	float bail3;
	vec3 LightDir;									float AmbLight;
	vec3 col1;										float LightCol;
	vec3 col2;										float LightSpc;
	float EmisCol;	float SpecPwr; float sine;		float dum7;
	vec3 FogColor;									float FogEn;
	bool ccol;		float occrange;	int frames;		bool sincos;
	int b0;			int b1;			int b2;			float radius;
	vec3 dispang;									float Otrap;
	vec3 space;										float dum8;
	vec3 colcentre;									float dum9;
	uint MinMax[4];
	vec3 centrespace;								float dum11;
	vec4 dum12;
};
//-----------------------------------------------------------------------------

vec3 wrap(vec3 x, vec3 a, vec3 s) {
	x -= s;
	return (x - a * floor(x / a)) + s;
}
vec2 wrap(vec2 x, vec2 a, vec2 s) {
	x -= s;
	return (x - a * floor(x / a)) + s;
}

int i = 0, j = 0;
float retot = 1000;
float JosKleinian(vec3 z) {
	vec3 vectc = z;
	vec3 lz = z + vec3(1, 1, 1), llz = z + vec3(-1, -1, -1);
	float de = 1000;
	float DF = 1.0;
	float a = KleinR, b = KleinI;
	float f = 0;
	//float f = sign(b);
	vec4 ot, trap = vec4(0.1, 0.1, 0.1, 0.1); //OTfixed;
	for (i = 0; i < maxiter; ++i)
	{
		if (maxiter2 == 0) z += vectc;
		z.x = z.x + b / a * z.y;
		if (b1 != 0) //FourGen
			z = wrap(z, vec3(foldx * boxsizex, a, foldx * boxsizez), vec3(-boxsizex, -losScale, -boxsizez));
		else
			z.xz = wrap(z.xz, vec2(foldx * boxsizex, foldx * boxsizez), vec2(-boxsizex, -boxsizez));
		z.x = z.x - b / a * z.y;
		//If above the separation line, rotate by 180 about (-b/2, a/2)
		if (z.y >= a * (0.5 + f * 0.25 * sign(z.x + b * 0.5) * (1. - exp(-3.2 * abs(z.x + b * 0.5)))))
			z = vec3(-b, a, 0.0) - z;
		//Apply transformation a
		float iR = 1.0 / dot(z, z);
		z *= -iR;
		z.x = -b - z.x;
		z.y = a + z.y;
		DF = DF * abs(iR);
		if (dot(z - llz, z - llz) < 1e-12)
			break;						//If enters a 2-cycle , bail out.
		else
		{
			llz = lz; lz = z;			//Store previous iterates
			ot.xyz = z;
			if (b2 == 1)	ot -= trap;
			vec4 hk = vec4(ot.xyz, length(llz));
			retot = min(retot, dot(hk, hk));
		}
	}
	for (j = 0; j < maxiter1; ++j)		//WIP: Push the iterated point left 
	{									//or right depending on the sign of KleinI
		float y;
		if (b0 != 0)
			y = min(z.y, a - z.y);
		else
			y = z.y;
		de = min(de, min(y, foldy) / max(DF, foldz));
		//Apply transformation a
		float iR = kleinf / dot(z, z);
		z *= -iR;
		z.x = -b - z.x;
		z.y = a + z.y;
		DF = DF * abs(iR);
	}
	float y;
	if (b0 == 0)
		y = min(z.y, a - z.y);
	else
		y = z.y;
	de = min(de, min(y, foldy) / max(DF, foldz));
	return de;
}

float DE(vec3 p)
{
		if (b2 == 1) {
			p = p - mr;
			float r = length(p);
			float r2 = r * r;
			p = (fr * fr / r2) * p + mr;
			float an = atan(p.y, p.x) + sc;
			float ra = sqrt(p.y * p.y + p.x * p.x);
			p.x = cos(an) * ra;
			p.y = sin(an) * ra;
			float de = JosKleinian(p);
			de = r2 * de / (fr * fr + r * de);
			return de;
		}
		return JosKleinian(p);
}
//-----------------------------------------------------------------------------
vec3 lerp(vec3 v1, vec3 v2, float amount)
{
	return v1 + (v2 - v1) * amount;
}
//-----------------------------------------------------------------------------

uint gx = gl_GlobalInvocationID.x;
uint gy = gl_GlobalInvocationID.y;
//-----------------------------------------------------------------------------
void main() {
	vec3 coord = zoom * (vec3(Coord[gy * steps + (800 / numsteps) * slice][gx * steps], 0));
	if (Position[gy + (800 / numsteps) * slice][gx].w == 0) return;

	vec4 p0 = Position[gy + (800 / numsteps) * slice][gx];
	vec4 p1 = Position[gy + (800 / numsteps) * slice][gx + 1];
	vec4 p2 = Position[gy + 1 + (800 / numsteps) * slice][gx];
	if (p0.w * p1.w * p2.w == 0) return;

	vec3 normal = normalize(cross((p2.xyz - p0.xyz), (p1.xyz - p0.xyz)));
	vec3	rayt = normalize((wx * coord.x + wy * coord.y) - raycam);
	vec3 smin = centrespace - space;
	vec3 smax = centrespace + space;

	float found = 0;
	float de = accuracy, mbdist, mindist = 0.0005 / pow(3.0, df);
	vec3 v0;
	rayt	= reflect(normalize((wx * coord.x + wy * coord.y) - raycam), normal);
	vec3 neweye = Position[gy + (800 / numsteps) * slice][gx].xyz;
	mbdist = accuracy * 0.1;
	v0 = neweye + rayt * mbdist;
	while ((v0.x > smin.x) && (-v0.y > smin.y) && (v0.z > smin.z) &&	(v0.x < smax.x) && (-v0.y < smax.y) && (v0.z < smax.z) )
	{
		v0 = neweye + rayt * mbdist;
		de = DE(v0);
		mbdist += 25 * accuracy * abs(de);
		if (de < mindist)
		{found = 2;	break;}
	}
	outcol[gy + (800 / numsteps) * slice][gx] = vec4(0,0,0, found);
}

