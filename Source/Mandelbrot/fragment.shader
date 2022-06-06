
#version 330 core

// Interpolated values from the vertex shaders
in float 	iteration;
out vec3 color;

uniform float maxiter = 1024;
uniform float colspin = 10;
uniform float colmode = 0;
uniform vec3 col1 = vec3(0,1,0); 
uniform vec3 col2 =  vec3(1,0,0);

vec3 lerp(vec3 v1, vec3 v2, float amount)
{return v1 + (v2 - v1) * amount;}
void main()
{
	float iters = float(iteration);
	float temp	= 0.5 + cos(colspin * (iters) / 256.0)/2.0;
	float temp1	= 0.5 + cos(colspin * 2.0 * (iters) / 256.0)/2.0;
	float temp2	= 0.5 + cos(colspin * 3.0 * (iters) / 256.0)/2.0;
	if (iters < maxiter)
	{
		if (colmode == 0)	color = lerp(col1, col2, temp);        //vec3(temp, 1.0 - temp, 0);
		if (colmode == 1)	color = vec3(1.0 - temp, 1.0 - temp1, 1.0 - temp2);
		if (colmode == 2)	color = vec3( temp, temp, temp ).rgb;
	}
	else
	color		= vec3(0, 0, 0);
}
