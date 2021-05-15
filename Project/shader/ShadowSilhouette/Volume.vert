#version 460

//In Layouts
layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 VertexTexCoord;

//To Frag
out vec3 VPosition;
out vec3 VNormal;

//Set Uniforoms.
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjMatrix;

void main()
{ 
    //Calcuating Normal + Position.
    VNormal = NormalMatrix * VertexNormal;
    VPosition = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
    gl_Position = ProjMatrix * ModelViewMatrix * vec4(VertexPosition,1.0);
}


 