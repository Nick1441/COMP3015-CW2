#version 460

#define PI 3.14159265

uniform vec4 Color;

//Noise TEx sent to Frag
uniform sampler2D NoiseTex;

//Uniforms that change Due to ImGUI
uniform vec4 SkyColor = vec4(0.1, 0.3, 0.9, 1.0);
uniform vec4 CloudColor = vec4(1.0, 1.0, 1.0, 1.0);

in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

void main()
{
	//Setting Noise of mixture of Cloud + Sky Colour
	vec4 noise = texture(NoiseTex, TexCoord);
	float t = (cos(noise.a * PI) + 1.0) / 2.0f;
	vec4 color = mix(SkyColor, CloudColor, t);

	FragColor = vec4(color.rgb, 1.0);
}

