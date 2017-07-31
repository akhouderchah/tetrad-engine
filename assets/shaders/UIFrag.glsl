#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform sampler2D gSampler;

void main()
{
	outputColor = texture(gSampler, vec2(texCoord0.s, 1 - texCoord0.t));
}

