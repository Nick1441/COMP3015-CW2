#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (binding = 3) uniform sampler2D CarImage;
layout (binding = 2) uniform sampler2D MossImage;

uniform int Pass_3;

uniform struct LightInfo 
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Light[3];

uniform struct MaterialInfo 
{
  vec3 Ks;  //Specular Reflectivity
  float Shininess;   //Specular Shininess Factor
  vec3 Ka;
  vec3 Kd;
} Material;

//MultiTexture Model
vec3 MultiTexture(vec3 position, vec3 normal, int LightNum)
{
	//Texture Setting
	vec3 TexCar = texture(CarImage, TexCoord).rgb;
	vec4 TexMoss = texture(MossImage, TexCoord).rgba;
	vec3 col = mix(TexCar.rgb, TexMoss.rgb, TexMoss.a);

	//Ambient
	vec3 ambient = Light[LightNum].La * col;

	//Diffuse
	vec3 s = normalize(vec3(Light[LightNum].Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Light[LightNum].L * col * sDotN;
	
	//Specular
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		vec3 v = normalize(-Position.xyz);
		vec3 h = normalize(v + s);
		specular = Material.Ks * pow(max(dot(h, normal), 0.0), Material.Shininess);
	}

	return ambient + Light[LightNum].L * (diffuse + specular);
}

//Normal BlinnPhong Model
vec3 BlinnPhong(vec3 position, vec3 normal, int LightNum)
{
	//Ambient
	vec3 ambient = Material.Ka * Light[LightNum].La;

	//Diffuse
	vec3 s = normalize(vec3(Light[LightNum].Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Light[LightNum].L * Material.Kd * sDotN;
	
	//Specular
	vec3 specular = vec3(0.0);

	if (sDotN > 0.0)
	{
		vec3 v = normalize(-Position.xyz);
		vec3 h = normalize(v + s);
		specular = Material.Ks * pow(max(dot(h, normal), 0.0), Material.Shininess);
	}

	return ambient + Light[LightNum].L * (diffuse + specular);
}

void main()
{
	FragColor = vec4(0.0f);

	if (Pass_3 == 0)
	{
		//Gets Normal and Texture and combines them.
		for (int i = 0; i < 3; i++)
		{
			vec4 THIS = vec4(MultiTexture(Position, normalize(Normal), i), 1);
			vec4 IS = vec4(BlinnPhong(Position, normalize(Normal), i), 1);
			FragColor = THIS + IS;
		}
	}
	if (Pass_3 == 1)
	{
		for (int i = 0; i < 3; i++)
		{
			FragColor += vec4(BlinnPhong(Position, normalize(Normal), i), 1);
		}
	}


}
