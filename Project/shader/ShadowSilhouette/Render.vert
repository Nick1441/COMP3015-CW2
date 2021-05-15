#version 460

//Input layouts
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

//Outputs to fragment shader.
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

//Set uniforms in
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjMatrix;

void main()
{
    //Setting Texture coords, normal and position.
    TexCoord = VertexTexCoord;
    Normal = normalize( NormalMatrix * VertexNormal);
    Position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );

    gl_Position = ProjMatrix * ModelViewMatrix * vec4(VertexPosition,1.0);
}