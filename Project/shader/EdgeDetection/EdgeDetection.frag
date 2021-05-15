#version 460

//In Variables.
in vec3 Position;
in vec3 Normal;

//Binding for Texture inputed for Edge Detection
layout (location = 0) out vec4 FragColor;
layout (binding = 11) uniform sampler2D RenderTex;

uniform float EdgeThreshold;
uniform int Pass;

//Material And Light Structs.
uniform struct LightInfo 
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Light;

uniform struct MaterialInfo 
{
  vec3 Ks;
  vec3 Kd;
  float Shininess;
} Material;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

//Used to calculate edge, Basic BlinnPhong Model, Doesnt Require Explaining
vec3 BlinnPhongModel(vec3 position, vec3 normal)
{
	vec3 ambient = Light.La;

	vec3 s = normalize(vec3(Light.Position.xyz - position));
	float sDotN = max( dot(s, normal), 0.0 );
	vec3 diffuse = Material.Kd * sDotN;
	
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

//Runs through normaly
vec4 pass1()
{
	return vec4(BlinnPhongModel(Position, normalize(Normal)), 1.0);
}

//If edge, returnd white, Else returns black
vec4 pass2()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);

	float s00 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,1)).rgb);
	float s10 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,0)).rgb);
	float s20 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,-1)).rgb);
	float s01 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(0,1)).rgb);
	float s21 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(0,-1)).rgb);
	float s02 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,1)).rgb);
	float s12 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,0)).rgb);
	float s22 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,-1)).rgb);

	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
	float g = sx * sx + sy * sy;

	if( g > EdgeThreshold )
	{
		return vec4(1.0); //edge
	}
	else
	{
		return vec4(0.0,0.0,0.0,1.0); //no edge
	}
}

void main()
{
	//Depending on Uniform, Renders Each Pass.
    if( Pass == 1 ) FragColor = pass1();
	if( Pass == 2 ) FragColor = pass2();
}


