#version 460

in vec3 GPosition;
in vec3 GNormal;
in vec2 GTexCoord;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform sampler2D Tex;
uniform sampler2D Tex2;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

uniform int RenderType;

layout( location = 0 ) out vec4 Ambient;
layout( location = 1 ) out vec4 DiffSpec;

uniform int Type;

uniform int levels;
//const int levels = 3;
const float scaleFactor = 1.0 / levels;

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

void ToonShade()
{
	vec3 s = normalize(LightPosition.xyz - GPosition.xyz);
	Ambient = vec4(Ka * LightIntensity, 1);
	float cosine = dot(s, GNormal);
	DiffSpec = vec4(Kd * ceil(cosine * levels) * scaleFactor, 1);
}

void main() {
    //shade();
    if (RenderType == 1)
    {
        ToonShade();
    }
    else
    {
        shade();    
    }



    //ToonShade();

//    do if statment for is Edge??
//
//    	if (GIsEdge == 1)
//	{
//		FragColor = LineColor;
//	}
//	else
//	{
//		FragColor = vec4(ToonShade(), 1.0);
//	}
}