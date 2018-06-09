#version 440

layout (location = 0) in vec3 inPosition;
layout (location = 3) in vec2 inTex;
uniform mat4 transform;
out vec2 fragTex;

void main()
{
	gl_Position = transform * vec4(inPosition, 1.0);
	fragTex = vec2(inTex.x, inTex.y);
}
