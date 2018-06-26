#version 440

layout (location = 0) in vec3 inPosition;
#ifdef USE_INSTANCED_RENDERING
layout (location = 5) in mat4 transform;
#else
uniform mat4 transform;
#endif
uniform mat4 lightMatrix;

void main()
{
	gl_Position = lightMatrix * transform * vec4(inPosition, 1.0);
}
