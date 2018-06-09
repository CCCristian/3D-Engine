#version 440

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTex;
#ifdef USE_INSTANCED_RENDERING
layout (location = 5) in mat4 instanceTransform;
#else
uniform mat4 transform;
#endif
out vec3 fragPosition;
out vec4 fragLightSpacePosition;
out vec3 fragNormal;
out vec2 fragTex;
uniform mat4 world;
uniform mat4 lightSpaceMatrix;
uniform float textureRepeatCount;

void main()
{
#ifdef USE_INSTANCED_RENDERING
	gl_Position = world * instanceTransform * vec4(inPosition, 1.0);
	fragLightSpacePosition = lightSpaceMatrix * instanceTransform * vec4(inPosition, 1.0);
	fragPosition = vec3(instanceTransform * vec4(inPosition, 1.0));
	fragNormal = vec3(transpose(inverse(instanceTransform)) * vec4(inNormal, 0.0));
	fragTex = vec2(inTex.x, 1 - inTex.y) * textureRepeatCount;
#else
	gl_Position = world * transform * vec4(inPosition, 1.0);
	fragLightSpacePosition = lightSpaceMatrix * transform * vec4(inPosition, 1.0);
	fragPosition = vec3(transform * vec4(inPosition, 1.0));
	fragNormal = vec3(transpose(inverse(transform)) * vec4(inNormal, 0.0));
	fragTex = vec2(inTex.x, 1 - inTex.y) * textureRepeatCount;
#endif
}