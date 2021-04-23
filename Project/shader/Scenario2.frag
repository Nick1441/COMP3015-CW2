#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

//Setting Locations of Diffuse and Moss Images.
layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D CarImage; 
layout (binding = 2) uniform sampler2D AlphaTex;

//Setting to ignre sections.
uniform int Pass_2;

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
  vec3 Ks;  //Specular Reflectivity
  vec3 Ka;
  vec3 Kd;
  float Shininess;   //Specular Shininess Factor
} Material;

//Normal BlinnPhong Model
vec3 BlinnPhongNormal(vec3 position, vec3 normal)
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
//Alpha Model
vec3 Alpha(vec3 position, vec3 normal)
{
	//Texture Setting
	vec3 texColor = texture(CarImage, TexCoord).rbg;

	//Ambient
	vec3 ambient = Light.La * texColor;

	//Diffuse
	vec3 s = normalize(vec3(Light.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Light.L * texColor * sDotN;
	
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
	//setting Alpha Map
	vec4 alphaMap = texture(AlphaTex, TexCoord).rgba;

	if (Pass_2 == 0)
	{
		FragColor = vec4(BlinnPhongNormal(Position, normalize(Normal)), 1);
	}
	if (Pass_2 == 1)
	{
		//if it lower than 0.15, thrown away.
		if (alphaMap.a < (0.15))
		{
			discard;
		}
		else 
		{
			//if the current one is facing us.
			if (gl_FrontFacing)
			{
				FragColor = vec4(Alpha(Position, normalize(Normal)), 1);
			}
			//if its away, it is reversed.
			else 
			{
				FragColor = vec4(Alpha(Position, normalize(-Normal)), 1);
			}
		}
	}

    
}
