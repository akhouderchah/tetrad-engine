#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform float gTime;
uniform sampler2D gSampler;

uniform vec4 gAddColor;
uniform vec4 gMultColor;
uniform vec4 gTopMultiplier;

int inBorder();

void main()
{
	vec4 texColor = gAddColor
		+ gMultColor * texture(gSampler, vec2(texCoord0.s + gTime, texCoord0.t));

	outputColor = texColor * mix(gTopMultiplier,
								 vec4(1, 1, 1, 1),
								 texCoord0.t);

	/*
	outputColor = mix(texColor,
					  vec4(0.1, 0.1, 0.1, 1),
					  inBorder());
	*/
}

int inBorder()
{
	return int(abs(texCoord0.s - 0.5) > 0.49 ||
			   abs(texCoord0.t - 0.5) > 0.49);
}
