#version 460

//Layout Locations
layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec2 VertexTexCoord;

//Setting out to Frag shader.
out vec2 TexCoord;

uniform mat4 MVP;

void main()
{
	//Setting Position + TexCoord
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition,1.0);
}