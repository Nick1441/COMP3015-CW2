#version 460

layout( triangles_adjacency ) in;
layout( triangle_strip, max_vertices = 18 ) out;

//In from Vert
in vec3 VPosition[];
in vec3 VNormal[];

//Uniforms In
uniform vec4 LightPosition;
uniform mat4 ProjMatrix;

//Checking to see if it is facing light.
bool facesLight( vec3 a, vec3 b, vec3 c )
{
  vec3 n = cross( b - a, c - a );
  vec3 da = LightPosition.xyz - a;
  vec3 db = LightPosition.xyz - b;
  vec3 dc = LightPosition.xyz - c;

  return dot(n, da) > 0 || dot(n, db) > 0 || dot(n, dc) > 0; 
}

//Calculation Emiiting Quads.
void emitEdgeQuad( vec3 a, vec3 b ) {
  gl_Position = ProjMatrix * vec4(a, 1);
  EmitVertex();
  
  gl_Position = ProjMatrix * vec4(a - LightPosition.xyz, 0);
  EmitVertex();

  gl_Position = ProjMatrix * vec4(b, 1);
  EmitVertex();

  gl_Position = ProjMatrix * vec4(b - LightPosition.xyz, 0);
  EmitVertex();
  EndPrimitive();
}

void main()
{
    //If main Traigle facing Light, Check adjacent triangle. if not facing light, output a silhouette edge.
    if( facesLight(VPosition[0], VPosition[2], VPosition[4]) ) {
        if( ! facesLight(VPosition[0],VPosition[1],VPosition[2]) ) 
          emitEdgeQuad(VPosition[0],VPosition[2]);
        if( ! facesLight(VPosition[2],VPosition[3],VPosition[4]) ) 
          emitEdgeQuad(VPosition[2],VPosition[4]);
        if( ! facesLight(VPosition[4],VPosition[5],VPosition[0]) ) 
          emitEdgeQuad(VPosition[4],VPosition[0]);
    }
}