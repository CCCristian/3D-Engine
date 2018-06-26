#version 440

uniform sampler2D reflectionSampler;
uniform sampler2D refractionSampler;
uniform sampler2D dudvSampler;
uniform sampler2D normalSampler;
uniform vec3 cameraPosition;
uniform vec3 directionalLight;
uniform vec3 directionalColor;
uniform float movement;
in vec4 clipPosition;
in vec3 worldPosition;
in vec2 texCoords;
out vec4 gl_FragColor;
const float displacementPower = 0.03;
const float shininess = 15;
const float reflectivity = 0.6;

void main()
{
	vec2 normalizedTexCoords = (clipPosition.xy/clipPosition.w + 1) / 2;

	vec2 distortionCoords = texture(dudvSampler, texCoords + vec2(movement, 0)).rg * 0.1;
	distortionCoords = texCoords + distortionCoords + vec2(0, movement);
	vec2 distortion = (texture(dudvSampler, distortionCoords).rg * 2 - 1) * displacementPower;

	const vec2 reflectionCoords = clamp(vec2(normalizedTexCoords.x, 1 - normalizedTexCoords.y) + distortion, 0, 1);
	const vec2 refractionCoords = clamp(normalizedTexCoords + distortion, 0, 1);

	vec4 reflection = texture(reflectionSampler, reflectionCoords);
	vec4 refraction = texture(refractionSampler, refractionCoords);

	float refractiveIndex = clamp(dot(vec3(0, 0, 1), normalize(cameraPosition - worldPosition)), 0.1, 0.9);
	gl_FragColor = mix(reflection, refraction, pow(refractiveIndex, 2));
	
	vec4 normalColor = texture(normalSampler, distortionCoords);
	vec3 normal = normalize(vec3(normalColor.xy * 2 - 1, normalColor.z));
	vec3 reflectedLight = normalize(reflect(normalize(directionalLight), normal));
	float specularFactor = max(dot(reflectedLight, normalize(cameraPosition - worldPosition)), 0);
	gl_FragColor += gl_FragColor * vec4(directionalColor, 1) * (reflectivity * pow(specularFactor, shininess)) * vec4(0.8, 0.8, 1.2, 1);
}
