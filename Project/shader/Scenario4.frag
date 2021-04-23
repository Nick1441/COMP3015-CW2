#version 460

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

//Setting to ignre sections.
uniform int Pass;

//Set up light struct
uniform struct LightInfo 
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Light;

//Set up material Info
uniform struct MaterialInfo 
{
  vec3 Ka;  //Ambient Reflectivity
  vec3 Kd;  //Diffuse Reflectivity
  vec3 Ks;  //Specular Reflectivity
  float Shininess;   //Specular Shininess Factor
} Material;

//Set up Fog Info
uniform struct FogInfo
{
	float MaxDist;
	float MinDist;
	vec3 Color;
}Fog;

//Setting levels For Fo
const int levels = 4;
const float scaleFactor = 1.0 / levels;

vec3 FogModel(vec3 position, vec3 normal)
{
	//Ambient
	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength * Light.La;

	//Diffuse
	vec3 s = normalize(vec3(Light.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );

	//Toon Shading
	vec3 diffuse = Material.Kd * floor(sDotN * levels) * scaleFactor;


	return ambient + Light.L * (diffuse);
}

void main()
{
	if (Pass == 0)
	{
		FragColor = vec4(FogModel(Position, normalize(Normal)), 1);
	}
	if (Pass == 1)
	{
		//Getting Fog Factor using Distance and Min/Max
		float dist = abs(Position.z);
		float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist - Fog.MinDist);

		//Clamping Fog Factor.
		fogFactor = clamp(fogFactor, 0.0, 1.0);

		vec3 shadeColour = FogModel(Position, normalize(Normal));

		//Mixing Colour then setting as Fragcolour.
		vec3 colour = mix(Fog.Color, shadeColour, fogFactor);

		FragColor = vec4(colour, 1.0);
	}
}
