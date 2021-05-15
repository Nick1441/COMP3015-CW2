#version 460

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;
layout (binding = 7) uniform sampler2D Texture0;
layout (binding = 8) uniform sampler2D Texture;

uniform float EdgeThreshold;
uniform int Pass;
in vec2 TexCoord;
uniform float Weight[5];

uniform struct LightInfo 
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Light;

uniform struct MaterialInfo 
{
  vec3 Ks;  //Specular Reflectivity
  vec3 Kd;
  float Shininess;   //Specular Shininess Factor
} Material;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

vec3 PhongModel2(vec3 position, vec3 normal)
{
	//Texture Setting

	//Ambient
	vec3 ambient = Light.La;

	//Diffuse
	vec3 s = normalize(vec3(Light.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Material.Kd * sDotN;
	
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
vec3 PhongModel(vec3 position, vec3 normal)
{
	//Texture Setting
	vec3 texColor = texture(Texture, TexCoord).rgb; //Extract Colour for each frag.

	//Ambient
	vec3 ambient = Light.La * texColor;

	//Diffuse
	vec3 s = normalize(vec3(Light.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Material.Kd * texColor * sDotN;
	
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

float luminance(vec3 color)
{
	return dot(lum, color);
}

vec4 pass1()
{
	return vec4(PhongModel(Position, normalize(Normal)), 1.0);
}

vec4 pass2()
{
	 ivec2 pix = ivec2( gl_FragCoord.xy );
	 vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,1) ) * Weight[1];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-1) ) * Weight[1];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,2) ) * Weight[2];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-2) ) * Weight[2];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,3) ) * Weight[3];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-3) ) * Weight[3];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,4) ) * Weight[4];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-4) ) * Weight[4];
	 return sum;
}

vec4 pass3()
{
	 ivec2 pix = ivec2( gl_FragCoord.xy );
	 vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(1,0) ) * Weight[1];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(-1,0) ) * Weight[1];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(2,0) ) * Weight[2];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(-2,0) ) * Weight[2];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(3,0) ) * Weight[3];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(-3,0) ) * Weight[3];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(4,0) ) * Weight[4];
	 sum += texelFetchOffset( Texture0, pix, 0, ivec2(-4,0) ) * Weight[4];
	 return sum;
}


void main()
{
	 if( Pass == 1 )
	FragColor = pass1();
	 else if( Pass == 2 )
	FragColor = pass2();
	 else if( Pass == 3 )
	FragColor = pass3();
}



