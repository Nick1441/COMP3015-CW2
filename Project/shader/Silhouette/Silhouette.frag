#version 460

//
//	--- Duplication of Geom for use in Mix1, See that file for comments, Saved Duplication.
//

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
};
uniform MaterialInfo Material;

uniform vec4 LineColor;

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout(location = 0) out vec4 FragColor;

uniform int levels;
const float scaleFactor = 1.0 / levels;

vec3 ToonShade()
{
	vec3 s = normalize(Light.Position.xyz - GPosition.xyz);
	vec3 ambient = Material.Ka;
	float cosine = dot(s, GNormal);
	vec3 diffuse = Material.Kd * ceil(cosine * levels) * scaleFactor;

	return Light.Intensity * (ambient + diffuse);
}


void main()
{
	if (GIsEdge == 1)
	{
		FragColor = LineColor;
	}
	else
	{
		FragColor = vec4(ToonShade(), 1.0);
	}
}