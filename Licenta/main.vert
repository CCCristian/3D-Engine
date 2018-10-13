#version 440

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTex;
layout (location = 4) in vec3 inTangent;
#ifdef USE_INSTANCED_RENDERING
layout (location = 5) in mat4 transform;
#else
uniform mat4 transform;
#endif

out vec3 fragPosition;
out vec4 fragLightSpacePosition;
out vec3 fragNormal;
out vec2 fragTex;
out vec3 fragTangent;

uniform mat4 world;
uniform mat4 lightSpaceMatrix;
uniform float textureRepeatCount;
uniform vec4 clipPlane;

void main()
{
	gl_Position = world * transform * vec4(inPosition, 1.0);
	fragPosition = vec3(transform * vec4(inPosition, 1.0));
	fragNormal = vec3(transpose(inverse(transform)) * vec4(inNormal, 0.0));
	fragTex = vec2(inTex.x, 1 - inTex.y) * textureRepeatCount;
	fragTangent = (transform * vec4(inTangent, 0.0)).xyz;

	fragLightSpacePosition = lightSpaceMatrix * transform * vec4(inPosition, 1.0);

	gl_ClipDistance[0] = dot(transform * vec4(inPosition, 1.0), clipPlane);
}
