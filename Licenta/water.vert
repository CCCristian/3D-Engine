#version 440

layout (location = 0) in vec3 inPosition;
layout (location = 3) in vec2 inTexCoords;
uniform mat4 world;
uniform vec2 tiles;
#ifdef USE_INSTANCED_RENDERING
layout (location = 5) in mat4 instanceTransform;
#else
uniform mat4 transform;
#endif
out vec4 clipPosition;
out vec3 worldPosition;
out vec2 texCoords;


void main()
{
#ifdef USE_INSTANCED_RENDERING
	gl_Position = instanceTransform * vec4(inPosition, 1.0);
#else
	gl_Position = transform * vec4(inPosition, 1.0);
#endif
	worldPosition = gl_Position.xyz;
	gl_Position = world * gl_Position;
	clipPosition = gl_Position;
	texCoords = tiles * inTexCoords;
}
