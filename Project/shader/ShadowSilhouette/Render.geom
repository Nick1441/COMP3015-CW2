#version 460

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 15) out;

//outputs.
out vec3 GNormal;
out vec3 GPosition;

in vec2 TexCoord[];
out vec2 GTexCoord;

flat out int GIsEdge;

in vec3 Normal[];
in vec3 Position[];

//Unfiroms set in ImGUI
uniform float EdgeWidth;
uniform float PctExtend;

//calculates if front facing.
bool isFrontFacing(vec3 a, vec3 b, vec3 c)
{
	return ((a.x * b.y - b.x * a.y) + (b.x * c.y - c.x * b.y) + (c.x * a.y - a.x * c.y)) > 0;
}

//Emmites quads.
void emitEdgeQuad(vec3 e0, vec3 e1)
{
	vec2 ext = PctExtend * (e1.xy - e0.xy);
	vec2 v = normalize(e1.xy - e0.xy);
	vec2 n = vec2(-v.y, v.x) * EdgeWidth;

	GIsEdge = 1;

	gl_Position = vec4(e0.xy - ext, e0.z, 1.0); EmitVertex();

	gl_Position = vec4(e0.xy - n - ext, e0.z, 1.0); EmitVertex();

	gl_Position = vec4(e1.xy + ext, e1.z, 1.0); EmitVertex();

	gl_Position = vec4(e1.xy - n + ext, e1.z, 1.0); EmitVertex();

	EndPrimitive();
}

//Calulating Vertex Position
void main()
{
	vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
	vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
	vec3 p3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
	vec3 p4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
	vec3 p5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

	if (isFrontFacing(p0, p2, p4))
	{
		if (!isFrontFacing(p0, p1, p2))
		{
			emitEdgeQuad(p0, p2);
		}

		if (!isFrontFacing(p2, p3, p4))
		{
			emitEdgeQuad(p2, p4);
		}

		if (!isFrontFacing(p4, p5, p0))
		{
			emitEdgeQuad(p4, p0);
		}
	}

	GIsEdge = 0;

	GNormal = Normal[0];
	GPosition = Position[0];
	GTexCoord = TexCoord[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	GNormal = Normal[2];
	GPosition = Position[2];
	GTexCoord = TexCoord[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	GNormal = Normal[4];
	GPosition = Position[4];
	GTexCoord = TexCoord[4];
	gl_Position = gl_in[4].gl_Position;
	EmitVertex();

	EndPrimitive();
}