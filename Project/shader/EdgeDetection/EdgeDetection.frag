#version 460

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;
layout (binding = 11) uniform sampler2D RenderTex;

uniform float EdgeThreshold;
uniform int Pass;

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

vec3 PhongModel(vec3 position, vec3 normal)
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
	return vec4(1.0); //edge
	else
	return vec4(0.0,0.0,0.0,1.0); //no edge

}

void main()
{
    if( Pass == 1 ) FragColor = pass1();
	if( Pass == 2 ) FragColor = pass2();

}


