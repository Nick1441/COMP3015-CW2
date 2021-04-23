#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

//Setting Locations of Diffuse and Moss Images.
layout (location = 0) out vec4 FragColor;
layout (binding = 3) uniform sampler2D ObjTex1;
layout (binding = 1) uniform sampler2D ObjTex2;

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
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float Shininess;
} Material;

//Setting Spotlight Struct
uniform struct SpotLightInfo 
{
  vec3 Position;
  vec3 La;
  vec3 L;
  vec3 Direction;
  float Exponent;
  float CutOff;
} Spot;

//Light One
vec3 LightModel(vec3 position, vec3 normal)
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

//SpotLight!
vec3 SpotLightModel(vec3 position, vec3 normal)
{
	//Ambient
	vec3 ambient = Material.Ka * Spot.La;

	//Diffuse
	vec3 s = normalize(vec3(Spot.Position.xyz - position));

	float cosAng = dot(-s, normalize(Spot.Direction));
	float angle_1 = acos(cosAng);
	float spotScale = 0.0;

	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	if (angle_1 < Spot.CutOff)
	{
		spotScale = pow(cosAng, Spot.Exponent);

		float sDotN = max( dot(s, normal), 0.0 );
		diffuse = Spot.L * Material.Kd * sDotN;
	
		//Specular
		specular = vec3(0.0);

		if (sDotN > 0.0)
		{
			vec3 v = normalize(-Position.xyz);
			vec3 h = normalize(v + s);
			specular = Material.Ks * pow(max(dot(h, normal), 0.0), Material.Shininess);
		}
	}

	return ambient + spotScale * Spot.L * (diffuse + specular);
}

vec3 TextureModel(vec3 position, vec3 normal, int i)
{
	//Texture Setting, Sets differnt texture depending on "pass".
	vec3 texColor = vec3(0.0f);

	if (i == 1)
	{
		texColor = texture(ObjTex1, TexCoord).rgb;
	}
	if (i == 2)
	{
		texColor = texture(ObjTex2, TexCoord).rgb;
	}

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

vec4 Object(int i)
{	
	//Gets Spot, Light & Texture, combines and outputs.
	vec4 FinalResultLights = vec4(LightModel(Position, normalize(Normal)), 1);
	vec4 FinalResultSpot = vec4(SpotLightModel(Position, normalize(Normal)), 1);
	vec4 TexMeth = vec4(TextureModel(Position, normalize(Normal), i), 1);
    FragColor = FinalResultLights + FinalResultSpot + TexMeth;

	return FragColor;
}

vec4 Other()
{
	//Find Spot & Light, Outputs it. Ignores any Textures.
	vec4 FinalResultLights = vec4(LightModel(Position, normalize(Normal)), 1);
	vec4 FinalResultSpot = vec4(SpotLightModel(Position, normalize(Normal)), 1);
    FragColor = FinalResultLights + FinalResultSpot;

	return FragColor;
}

void main()
{
	//Sets where to go depending on Pass, Changes Texture in  Each Method.
	if( Pass == 0 ) FragColor = Other();
    if( Pass == 1 ) FragColor = Object(1);
	if( Pass == 2 ) FragColor = Object(2);
}
