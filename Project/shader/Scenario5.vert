#version 460

//Setting Input from locations.
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

//Setting outputs to Fragment Shader.
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

//Uniform set in C++
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP; 
 
void main()
{ 
	//Setting the normal and position using Normal/Vertex Matrix
	Normal = normalize(NormalMatrix * VertexNormal);
	Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition,1.0); 
} 