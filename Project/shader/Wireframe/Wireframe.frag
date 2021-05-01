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
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

uniform struct LineInfo
{
	float Width;
	vec4 Color;
	vec4 BackColor;
} Line;

in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

layout(location = 0) out vec4 FragColor;

 vec3 PhongModel(vec3 Position, vec3 n)
 {
	//Ambient
	vec3 ambient = Material.Ka * Light.Intensity;

	//Diffuse
	vec3 s = normalize(vec3(Light.Position.xyz - Position));
	float sDotN = max( dot(s,n), 0.0 );
	vec3 diffuse = Material.Kd * sDotN;
	
	//Specular
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		vec3 v = normalize(-Position.xyz);
		vec3 r = reflect(-s, n);
		specular = Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
	}

	return ambient + (diffuse + specular);
 }

void main()
{
	bool Test = false;
	bool Test2 = false;

	vec4 color = vec4(PhongModel(GPosition, GNormal), 1.0);
	vec4 NewColour;

	float d = min(GEdgeDistance.x, GEdgeDistance.y);
	d = min(d, GEdgeDistance.z);

	float MixVal;
	if (d < Line.Width - 1)
	{
		MixVal = 1.0f;
	}
	else if (d > Line.Width + 1)
	{
		MixVal = 0.0f;
		Test = true;
	}
	else 
	{
		float x = d - (Line.Width - 1);
		MixVal = exp2(-2.0 * (x*x));
	}

	if (Test == true)
	{
		FragColor = mix(color * Line.BackColor, Line.Color, MixVal);
	}
	else
	{
		FragColor = mix(color, Line.Color, MixVal);
	}

	//FragColor = mix(color, Line.Color, MixVal);
}