#version 460

//Position and Normal for Vertex
in vec3 Position;
in vec3 Normal;

//Layout locations for Textures.
layout (location = 0) out vec4 FragColor;
layout (binding = 7) uniform sampler2D Texture0;
layout (binding = 8) uniform sampler2D Texture;

//Uniform Inputs, Changes through ImGUI
uniform float EdgeThreshold;
uniform int Pass;
uniform float Weight[5];
in vec2 TexCoord;

//Uniform Structs for Light/Material
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

//Works out BlinnPhong Model & Texture!
vec3 BlinnPhongModel(vec3 position, vec3 normal)
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

//Calculates Luminance
float luminance(vec3 color)
{
	return dot(lum, color);
}

//Works out normal Rendering.
vec4 pass1()
{
	return vec4(BlinnPhongModel(Position, normalize(Normal)), 1.0);
}

//Works out blur amount for Quad.
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

//Works out blur amount for Quad.
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

//Calcuates each pass based on uniform int.
void main()
{
	 if( Pass == 1 )
	FragColor = pass1();
	 else if( Pass == 2 )
	FragColor = pass2();
	 else if( Pass == 3 )
	FragColor = pass3();
}



