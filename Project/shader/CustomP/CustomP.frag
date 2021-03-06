#version 460

//In Variables.
in vec3 fPosition;
in vec3 fNormal;
in vec2 TexCoord;	

layout (location = 0) out vec4 FragColor;

//For Toon Shading. This does not change.
const int levels = 20;
const float scaleFactor = 1.0 / levels;

//Ligh and material Structs. Basic.
uniform struct LightInfo 
{
  vec4 Position;
  vec3 Intensity;
} Light;

uniform struct MaterialInfo 
{
  vec3 Ks;
  vec3 Ka;
  vec3 Kd;
  vec3 E;
  float Shininess;
} Material;

//Toon shade Calcultes colour and outputs as frag colour.
vec3 ToonShade()
{
	vec3 s = normalize(Light.Position.xyz - fPosition.xyz);
	vec3 ambient = Material.Ka;
	float cosine = dot(s, normalize(fNormal));
	vec3 diffuse = Material.Kd * ceil(cosine * levels) * scaleFactor;

	return Light.Intensity * (ambient + diffuse);
}

void main()
{
	FragColor = vec4(ToonShade(), 1.0);
}

