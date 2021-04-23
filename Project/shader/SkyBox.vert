#version 460

layout (location = 0) in vec3 VertexPosition;

//Output to fragment shader.
out vec3 Vec;

uniform mat4 MVP;
 
void main()
{ 
	//setting vec for fragment shader.
	Vec = VertexPosition;
	gl_Position = MVP * vec4(VertexPosition, 1.0); 
} 