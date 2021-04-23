#version 460
layout (binding = 5) uniform samplerCube SkyBoxTex;

in vec3 Vec;

layout (location = 0) out vec4 FragColor;

void main()
{
	//Getting skybox colour from samplercube & vec.
	vec3 texColor = texture(SkyBoxTex, normalize(Vec)).rgb;

	//Raises it to power, Gamma Correction.
	texColor = pow( texColor, vec3(1.0/2.2));

	FragColor = vec4(texColor, 1);
}
