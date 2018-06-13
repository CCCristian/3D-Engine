#version 440

uniform samplerCube skyboxSampler;
in vec3 tex;
layout(location = 0) out vec4 gl_FragColor;

void main()
{
	gl_FragColor = texture(skyboxSampler, tex);
}
