
#version 440 core
#define DEBUG

layout(location = 5) in vec4 vertexpos;
uniform int axcurr;
uniform float rotang;
uniform mat4 MVP;

out vec4 colorout;
void main()
{ //
	gl_Position = MVP * vec4(vertexpos.xy, vertexpos.z, 1);
	switch ( int(vertexpos.w) )	
	{ 
		case 1 : { if (axcurr == 0)	colorout = vec4(1,0,0,1); else colorout = vec4(1,1,1,1); break;}
		case 2 : { if (axcurr == 1)	colorout = vec4(0,1,0,1); else colorout = vec4(1,1,1,1); break;}
		case 3 : { if (axcurr == 2)	colorout = vec4(0,0,1,1); else colorout = vec4(1,1,1,1); break;}
		case 4 : { colorout = vec4(0.5,0,0.5,1); break;}
		case 5:	 {
					colorout = vec4(1, 1, 1, 1);
					if((rotang > 0) && (vertexpos.z < rotang)) colorout = vec4(1, 0, 1, 1);
					if ((rotang <= 0) && (vertexpos.z > 6.28 + rotang)) colorout = vec4(1, 0, 1, 1);
					gl_Position = MVP * vec4(vertexpos.xy, 0, 1);
					break; 
		}
		default : { colorout = vec4(1,1,1,1); break;}
	}
	return;
}