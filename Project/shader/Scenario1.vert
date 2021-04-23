#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

uniform struct LightInfo 
{
  vec4 Position;
  vec3 La;
  vec3 L;
} Light;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;
out vec3 Position;
out vec3 Normal;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
 
void main()
{ 
//Creating the normal and tangent, using normal and vertex.
	vec3 norm = normalize( NormalMatrix * VertexNormal);
	vec3 tang = normalize( NormalMatrix * vec3(VertexTangent));

	//Created BiNormal using the cross product of norm and tang.
	vec3 binormal = normalize( cross( norm, tang ) ) * VertexTangent.w;
	
	//Find the local of the Binormal/tang.
	mat3 toObjectLocal = mat3(
		 tang.x, binormal.x, norm.x,
		 tang.y, binormal.y, norm.y,
		tang.z, binormal.z, norm.z ) ;

	vec3 pos = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
	
	//Setting normal and pos, Used on normal blinnPhong.
	Normal = normalize(NormalMatrix * VertexNormal);
	Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

	//Used for setting Normal Map
	LightDir = toObjectLocal * (Light.Position.xyz - pos);
	ViewDir = toObjectLocal * normalize(-pos);
	
	//Pass in the TexCoords.
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition,1.0); 
} 