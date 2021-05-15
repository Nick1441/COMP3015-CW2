#version 460

//Input from Geom
in vec3 GPosition;
in vec3 GNormal;
in vec2 GTexCoord;

//Input unifroms
uniform vec4 LightPosition;
uniform vec3 LightIntensity;

//Input textures from cpp
uniform sampler2D Tex;
uniform sampler2D Tex2;

//Material Struct but not as a struct
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float Shininess;

uniform int RenderType;

layout( location = 0 ) out vec4 Ambient;
layout( location = 1 ) out vec4 DiffSpec;

//int fro Normal Render or Silhouette
uniform int Type;

//Levels as uniform as can be changed.
uniform int levels;
const float scaleFactor = 1.0 / levels;

//Calcuating shadee
void shade( )
{
    vec3 s = normalize( vec3(LightPosition) - GPosition );
    vec3 v = normalize(vec3(-GPosition));
    vec3 r = reflect( -s, GNormal );

    vec4 texColor = vec4(1.0f);
    if (Type == 1)
    {
        texColor = texture(Tex, GTexCoord);
    }
    else
    {
       texColor = texture(Tex2, GTexCoord); 
    }

    Ambient = vec4(texColor.rgb * LightIntensity * Ka, 1.0);
    DiffSpec = vec4(texColor.rgb * LightIntensity * 
        ( Kd * max( dot(s, GNormal), 0.0 ) +
          Ks * pow( max( dot(r,v), 0.0 ), Shininess ) ) ,
          1.0 );
}

//Calculating Toon Shade
void ToonShade()
{
	vec3 s = normalize(LightPosition.xyz - GPosition.xyz);
	Ambient = vec4(Ka * LightIntensity, 1);
	float cosine = dot(s, GNormal);
	DiffSpec = vec4(Kd * ceil(cosine * levels) * scaleFactor, 1);
}

void main() {
    //Render type is for either toon shading, or normal blinnphong.
    if (RenderType == 1)
    {
        ToonShade();
    }
    else
    {
        shade();    
    }
}