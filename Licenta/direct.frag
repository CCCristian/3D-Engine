#version 440

uniform sampler2D colorSampler;
in vec2 fragTex;
out vec4 gl_FragColor;

void main()
{
//	gl_FragColor = vec4(1, 0, 1, 1);
	gl_FragColor = texture(colorSampler, fragTex);
}
