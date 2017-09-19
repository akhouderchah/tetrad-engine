#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform sampler2D gTexture;
uniform vec4 gTextColor;

void main()
{
	outputColor = vec4(gTextColor.rgb,
		gTextColor.a * texture(gTexture, vec2(texCoord0.s, 1-texCoord0.t)).r);
}

