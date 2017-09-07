#version 330

in vec2 texCoord0;
out vec4 outputColor;

uniform sampler2D gTexture;

uniform vec4 gAddColor;
uniform vec4 gMultColor;
uniform vec4 gTopMult;

int inBorder();

void main()
{
	vec4 texColor = gAddColor
		+ gMultColor * texture(gTexture, vec2(texCoord0.s, texCoord0.t));

	texColor = texColor * mix(gTopMult,
							  vec4(1, 1, 1, 1),
							  texCoord0.t);
	//gl_FragCoord.y/960);

	outputColor = mix(texColor,
					  vec4(.1, .1, .1, 1),
					  inBorder());
}

int inBorder()
{
	// Note - This is not a great way of doing this, it basically only works
	// for us because the borders are inconspicuous and small
	// TODO - Make a better border implementation
	return int(abs(texCoord0.s - 0.5) > 0.499 ||
			   abs(texCoord0.t - 0.5) > 0.499);
}
