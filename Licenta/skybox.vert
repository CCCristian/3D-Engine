#version 440

layout (location = 0) in vec3 inPosition;
uniform mat4 world;
out vec3 tex;

void main()
{
	gl_Position = world * vec4(inPosition, 1.0);
	tex = inPosition;
}
