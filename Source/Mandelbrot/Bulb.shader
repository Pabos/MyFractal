

#version 440 core
layout (local_size_x = 32, local_size_y = 24, local_size_z = 1 ) in;
layout(std430, binding=0) buffer Coordbuffer	{vec4 Coord[];};
layout(std430, binding=2) buffer Centrebuffer	{dvec2 Centre[];};
layout(std430, binding=3) buffer Positionbuffer	{dvec2 Position[];};
layout(std430, binding=1) buffer setupbuffer	{
	float colspin;	float colmode;	float maxiter;		float zoom;
	double centrey;					double centrex;
	double juliax;					double juliay;	
	bool julia;		bool polar;		float power;		float phase;	
												 };

void main()
{		
	int fracwidth = 2048;
    int	iteration;
	dvec2 v0 = 2.0f * ((1.024f / 1024.0f) * gl_GlobalInvocationID.xy - dvec2(1.024f, 0.768f));
	uint index		= gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * fracwidth;
	Coord[index]	= vec4(dvec2(v0),0 ,0) / 2.0f;
	if (polar == false)
	{
		double lv0;
		dvec2 vc;
		v0 = v0 * zoom + dvec2(centrex, centrey);
		Position[index]	= v0;
		if (julia == true)
			vc = dvec2(juliax, juliay);
		else
			vc = v0;
		for (iteration = 0; (lv0 < 4.0) && (iteration < maxiter); iteration ++) 
		{	
			double x	= v0.x;
			v0.x		= v0.x * v0.x - v0.y * v0.y + vc.x;
			v0.y		= 2.0f * x * v0.y + vc.y;
			lv0			= (v0.x * v0.x + v0.y * v0.y);
		}
		Coord[index].w	= iteration/* * float(lv0)*/;
		return;
	}
	else
	{
		float lv0_;
		vec2 v0_ = vec2(v0.x , v0.y);
		vec2 vconst;
		v0_ = v0_ * zoom + vec2(centrex, centrey);
		Position[index]	= v0_;	
		if (julia == true)
			vconst = vec2(juliax, juliay);
		else
			vconst = v0_;	
		lv0_	=  length(v0_);
		float theta;
#define compil
#ifdef compil
		for (iteration = 0; (lv0_ < 2.0) && (iteration < maxiter); iteration ++)
		{
			theta	= phase * atan(v0_.y , v0_.x);
			v0_		= pow(lv0_, power) * vec2(cos(theta), sin(theta)) + vconst;
			lv0_	=  length(v0_);
		}
#else
		float dr = 1.0;
		float dedist = 0;
		float	mindist = phase * 0.00001f;
		for (iteration = 0; (lv0_ < 2.0) && (iteration < maxiter); iteration ++)
		{
			theta = 2.0f * atan(v0_.y, v0_.x);
			v0_ = pow(lv0_, power) * vec2(cos(theta), sin(theta)) + vconst;
			lv0_ = length(v0_);
			dr = power * pow(lv0_, power - 1.0f) * dr + 1.0f;
			dedist = 0.5 * (log(lv0_) * lv0_ / dr);
		}

#endif
		Coord[index].w	= iteration/* * pow(lv0_, power)*/;
		return;
	}
}
