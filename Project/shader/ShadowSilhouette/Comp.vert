#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

//To Fragment Shader.
out vec3 Position;
out vec3 Normal;

//Unifrom Inputs.
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjMatrix;

void main()
{
    //Setting Position and Normal
    Normal = normalize( NormalMatrix * VertexNormal);
    Position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );

    gl_Position = ProjMatrix * ModelViewMatrix * vec4(VertexPosition,1.0);
}