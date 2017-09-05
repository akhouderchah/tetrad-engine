#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform sampler2D gTexture;
uniform vec4 gTextColor;

void main()
{
	outputColor = gTextColor *
		vec4(1.f, 1.f, 1.f, texture(gTexture, vec2(texCoord0.s, 1 - texCoord0.t)).r);
}

