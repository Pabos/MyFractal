
#version 330 core

layout(location = 2) in vec4 vertexpos;
uniform mat4 MVP;
out float iteration;
//-----------------------------------------------------------------------------
void main(){

    vec4 v = vec4(vertexpos.x * 1000, vertexpos.y * 1000, 1,1);
    gl_Position = MVP * v;
	iteration	= float(vertexpos.w);
}

