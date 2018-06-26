#version 440

struct AmbientLight
{
	vec3 color;
	float intensity;
};
struct DirectionalLight
{
	vec3 direction;
	float intensity;
	vec3 diffuseColor;
	vec3 specularColor;
};
struct PointLight
{
	vec3 position;
	float power;
	float quadraticFactor;
	float linearFactor;
	float constantFactor;
	vec3 diffuseColor;
	vec3 specularColor;
};
struct SpotLight
{
	vec3 position;
	vec3 direction;
	float innerAngleCosine;
	float outerAngleCosine;
	float power;
	float quadraticFactor;
	float linearFactor;
	float constantFactor;
	vec3 diffuseColor;
	vec3 specularColor;
};

// /////////////////////////////

uniform vec3 cameraPosition;

uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialSpecularIntensity;
uniform float materialSpecularPower;

uniform AmbientLight ambientLight;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform int textureCount;
uniform sampler2D colorSampler;
uniform sampler2D colorSampler2;
uniform sampler2D colorSampler3;
uniform sampler2D blendSampler;
uniform sampler2D specularSampler;
uniform sampler2D normalSampler;
uniform sampler2D lightSampler;
uniform sampler2D heightSampler;
uniform sampler2DShadow shadowSampler;

in vec3 fragPosition;
in vec4 fragLightSpacePosition;
in vec3 fragNormal;
in vec2 fragTex;
layout(location = 0) out vec4 gl_FragColor;


// AMBIENT LIGHT
vec3 calcAmbientLight(vec4 fragmentColor)
{
	// Computing the ambient component for the scene
	vec3 ambientComponent = fragmentColor.xyz * ambientLight.color * ambientLight.intensity;
	return ambientComponent;
}

// DIRECTIONAL LIGHT
vec3 calcDirectionalLight(vec4 fragmentColor, vec3 normal, vec3 fragmentToEye)
{
	// Computing the diffuse component for the directional light
	float diffuseFactor = directionalLight.intensity * max(dot(normal, normalize(-directionalLight.direction)), 0);
	vec3 diffuseComponent = fragmentColor.xyz * materialDiffuseColor * directionalLight.diffuseColor * diffuseFactor;
	
	// Computing the specular component for the directional light
	vec3 reflectedLight = normalize(reflect(normalize(directionalLight.direction), normal));
	float specularFactor = max(dot(reflectedLight, fragmentToEye), 0);
	vec3 specularComponent = directionalLight.intensity * fragmentColor.xyz * materialSpecularColor * directionalLight.specularColor * (materialSpecularIntensity * pow(specularFactor, materialSpecularPower));

	// This is the directional light effect
	vec3 directionalComponent = diffuseComponent + specularComponent;
	return directionalComponent;
}

// POINT LIGHT
vec3 calcPointLight(vec4 fragmentColor, vec3 normal, vec3 fragmentToEye)
{
	// Computing distance to the point light
	float dist = distance(fragPosition, pointLight.position);
	// Computing attenuation for the point light
	float attenuation = 1 / (pointLight.constantFactor + pointLight.linearFactor*dist + pointLight.quadraticFactor*dist*dist);

	// Computing the diffuse component for the point light
	float diffuseFactor = attenuation * pointLight.power * max(dot(normal, normalize(pointLight.position - fragPosition)), 0);
	vec3 diffuseComponent = fragmentColor.xyz * materialDiffuseColor * pointLight.diffuseColor * pointLight.diffuseColor * diffuseFactor;
	
	// Computing the specular component for the point light
	vec3 reflectedLight = normalize(reflect(normalize(fragPosition - pointLight.position), normal));
	float specularFactor = max(dot(reflectedLight, fragmentToEye), 0);
	vec3 specularComponent = fragmentColor.xyz * attenuation * pointLight.power * materialSpecularColor * pointLight.specularColor * (materialSpecularIntensity * pow(specularFactor, materialSpecularPower));
	
	// This is the point light effect
	vec3 pointLightComponent = diffuseComponent + specularComponent;
	return pointLightComponent;
}

// SPOT LIGHT
vec3 calcSpotLight(vec4 fragmentColor, vec3 normal, vec3 fragmentToEye)
{
	// Computing distance to the spot light
	float dist = distance(fragPosition, spotLight.position);
	// Computing attenuation for the spot light
	float attenuation = 1 / (spotLight.constantFactor + spotLight.linearFactor*dist + spotLight.quadraticFactor*dist*dist);
	// Computing spotlight / fragment angle cosine
	float angleCos = dot(normalize(spotLight.direction), normalize(fragPosition - spotLight.position));
	// Computing the angle lighting factor
	float angleFactor = clamp((angleCos - spotLight.outerAngleCosine) / (spotLight.innerAngleCosine - spotLight.outerAngleCosine), 0, 1);
	
	// Computing the diffuse component for the spot light
	float diffuseFactor = attenuation * angleFactor * spotLight.power * max(dot(normal, normalize(spotLight.position - fragPosition)), 0);
	vec3 diffuseComponent = fragmentColor.xyz * materialDiffuseColor * spotLight.diffuseColor * diffuseFactor;
	
	// Computing the specular component for the spot light
	vec3 reflectedLight = normalize(reflect(normalize(fragPosition - spotLight.position), normal));
	float specularFactor = max(dot(reflectedLight, fragmentToEye), 0) * angleFactor;
	vec3 specularComponent = fragmentColor.xyz * attenuation * spotLight.power * materialSpecularColor * spotLight.specularColor * (materialSpecularIntensity * pow(specularFactor, materialSpecularPower));
	
	// This is the spot light effect
	vec3 spotLightComponent = diffuseComponent + specularComponent;
	return spotLightComponent;
}

void main()
{
	if (textureCount == 1)
		gl_FragColor = texture(colorSampler, fragTex);
	else if (textureCount == 2)
		gl_FragColor = mix(texture(colorSampler, fragTex), texture(colorSampler2, fragTex), texture(blendSampler, fragTex));
	else if (textureCount == 3)
	{
		vec3 blend = texture(blendSampler, fragTex).xyz;
		vec4 color1 = texture(colorSampler, fragTex) * blend.x;
		vec4 color2 = texture(colorSampler2, fragTex) * blend.y;
		vec4 color3 = texture(colorSampler3, fragTex) * blend.z;
		gl_FragColor = color1 + color2 + color3;
	}
	

	// Precalculations
	vec3 normal = normalize(fragNormal);
	vec3 fragmentToEye = normalize(cameraPosition - fragPosition);
	vec3 shadowMapCoods = fragLightSpacePosition.xyz/fragLightSpacePosition.w * 0.5 + 0.5;
	float bias = max(0.01 * (1 - dot(normal, directionalLight.direction)), 0.002);
	shadowMapCoods.z -= 0.006;

	vec3 ambientComponent = calcAmbientLight(gl_FragColor);
	vec3 directionalComponent = calcDirectionalLight(gl_FragColor, normal, fragmentToEye);
	vec3 pointLightComponent = calcPointLight(gl_FragColor, normal, fragmentToEye);
	vec3 spotLightComponent = calcSpotLight(gl_FragColor, normal, fragmentToEye);

	float specShadow = texture(shadowSampler, shadowMapCoods);
	gl_FragColor = vec4(ambientComponent + specShadow * directionalComponent + pointLightComponent + spotLightComponent, 1);


}
