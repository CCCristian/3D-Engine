#version 440

layout (location = 0) in vec3 inPosition;
#ifdef USE_INSTANCED_RENDERING
layout (location = 5) in mat4 instanceTransform;
#else
uniform mat4 transform;
#endif
uniform mat4 lightMatrix;
//out vec4 pos;

void main()
{
#ifdef USE_INSTANCED_RENDERING
	gl_Position = lightMatrix * instanceTransform * vec4(inPosition, 1.0);
#else
	gl_Position = lightMatrix * transform * vec4(inPosition, 1.0);
#endif
//	pos = gl_Position;
}
