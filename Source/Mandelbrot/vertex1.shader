
#version 330 core

layout(location = 1) in vec3 framepos;
uniform float zoomdisp = 1.0f;
uniform mat4 MVP;
uniform vec4 mpos;
out vec3 color_;
//-----------------------------------------------------------------------------
void main()
{
    vec4 v = vec4((framepos.x * zoomdisp + mpos.x) * 1000, (framepos.y * zoomdisp + mpos.y) * 1000, 1,1);
    gl_Position = MVP * v;
	color_ = vec3 (framepos.z, 1,0);
}

