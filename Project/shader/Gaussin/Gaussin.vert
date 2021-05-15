#version 460

//Inputted Location
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

//Outputs to Frag
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

//Set Uniforms
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjMatrix;
uniform mat4 MVP;
 
void main()
{
	//Setting Normal, Position, Texcoord
	TexCoord = VertexTexCoord;
	Normal = normalize(NormalMatrix * VertexNormal);
	Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
	gl_Position = MVP * vec4(VertexPosition,1.0); 
}