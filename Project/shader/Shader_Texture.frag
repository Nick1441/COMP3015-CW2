#version 460

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

uniform struct LightInfo 
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Lights;

uniform struct MaterialInfo 
{
  vec3 Ka;  //Ambient Reflectivity
  vec3 Kd;  //Diffuse Reflectivity
  vec3 Ks;  //Specular Reflectivity
  float Shininess;   //Specular Shininess Factor
} Material;

vec3 PhongModel(vec3 position, vec3 normal)
{
	//Ambient
	vec3 ambient = Material.Ka * Lights.La;

	//Diffuse
	vec3 s = normalize(vec3(Lights.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Lights.L * Material.Kd * sDotN;
	
	//Specular
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		vec3 v = normalize(-Position.xyz);
		vec3 h = normalize(v + s);
		specular = Material.Ks * pow(max(dot(h, normal), 0.0), Material.Shininess);
	}

	return ambient + Lights.L * (diffuse + specular);
}

void main()
{
    FragColor = vec4(PhongModel(Position, normalize(Normal)), 1);
}
