#version 460
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

//CAN BE USED IN THE MIDDLE INSTEAD?
// vec3 PhongModel(vec3 Position, vec3 n)
// {
//	//Ambient
//	vec3 ambient = Material.Ka;
//
//	//Diffuse
//	vec3 s = normalize(vec3(Light.Position.xyz - Position));
//	float sDotN = max( dot(s,n), 0.0 );
//	vec3 diffuse = Material.Kd * sDotN;
//	
//	//Specular
//	vec3 specular = vec3(0.0);
//
//	if (sDotN > 0.0)
//	{
//		vec3 v = normalize(-Position.xyz);
//		vec3 r = reflect(-s, n);
//		specular = Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
//	}
//
//	return ambient + (diffuse + specular);
// }

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