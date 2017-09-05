#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform float gTime;
uniform sampler2D gTexture;

uniform vec4 gAddColor;
uniform vec4 gMultColor;

void main()
{
	outputColor = gAddColor +
		gMultColor * texture(gTexture, vec2(texCoord0.s + gTime, texCoord0.t));
}

