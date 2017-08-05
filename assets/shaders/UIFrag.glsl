#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform sampler2D gSampler;

void main()
{
	outputColor = texture(gSampler, vec2(texCoord0.s, texCoord0.t));
}

