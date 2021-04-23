#version 460
//Info In from Vertex Shader.
in vec3 LightDir;
in vec2 TexCoord;
in vec3 ViewDir;
in vec3 Position;
in vec3 Normal;

//Locations for Diffuse/Textures/Normal
layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D ColorTex;
layout (binding = 1) uniform sampler2D NormalMapTex;
layout (binding = 3) uniform sampler2D ColorTex2;
layout (binding = 4) uniform sampler2D ColorTex3;

//Used to Render Differnt Textures on Differnt Objects.
uniform int Pass;

//Strcut for Lights.
uniform struct LightInfo
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Light;

//Structs For materials
uniform struct MaterialInfo 
{
  vec3 Ks;  //Specular Reflectivity
  vec3 Ka;
  float Shininess;   //Specular Shininess Factor
  vec3 Kd;
} Material;

//Normal Mapping With BlinnPhong.
vec3 NormalMapping(vec3 LightDir, vec3 normal, int i)
{
	//Setts what Texture to Use, Each Object Selects Differnt Texture.
	vec3 texColor = vec3(0.0f);
	if (i == 1)
	{
		texColor = texture(ColorTex, TexCoord).xyz;
	}
	if (i == 2)
	{
		texColor = texture(ColorTex2, TexCoord).xyz;
	}
	if (i == 3)
	{
		texColor = texture(ColorTex3, TexCoord).xyz;
	}
	
	//Ambient
	vec3 ambient = Light.La * texColor;

	//Diffuse
	float sDotN = max( dot(LightDir, normal), 0.0 );
	vec3 diffuse = Material.Kd * texColor * sDotN;
	
	//Specular
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		vec3 h = normalize(LightDir + ViewDir);
		specular = Material.Ks * pow(max(dot(normal, h), 0.0), Material.Shininess);
	}

	return ambient + Light.L * (diffuse + specular);
}

//Normal BlinnPhong Model
vec3 BlinnPhong(vec3 position, vec3 normal)
{
	//Ambient
	vec3 ambient = Material.Ka * Light.La;

	//Diffuse
	vec3 s = normalize(vec3(Light.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Light.L * Material.Kd * sDotN;
	
	//Specular
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		vec3 v = normalize(-Position.xyz);
		vec3 h = normalize(v + s);
		specular = Material.Ks * pow(max(dot(h, normal), 0.0), Material.Shininess);
	}

	return ambient + Light.L * (diffuse + specular);
}

void main()
{
	//Setting Normal Using Texcord.
	vec3 norm = texture(NormalMapTex, TexCoord).xyz;
	norm.xy = 2.0 * norm.xy - 1.0;

	//For Multiple Lights, Include a For with i = 2, then pass in i and will work.
	if (Pass == 0)
	{
		FragColor = vec4(NormalMapping(LightDir, normalize(norm), 1), 1);	
	}
	if (Pass == 1)
	{
		FragColor = vec4(BlinnPhong(Position, normalize(Normal)), 1);
	}
	if (Pass == 2)
	{
		FragColor = vec4(NormalMapping(LightDir, normalize(norm), 2), 1);
	}
	if (Pass == 3)
	{
		FragColor = vec4(NormalMapping(LightDir, normalize(norm), 3), 1);
	}
}
