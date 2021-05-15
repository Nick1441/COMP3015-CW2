#version 460

//
//  --- Same As Custom Edge Particles -- Comments are not required as duplication.
//

const float PI = 3.1415926359;

uniform int Pass;
uniform int Type;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;

layout (location = 3) in vec3 ParticlePosition;
layout (location = 4) in vec3 ParticleVelocity;
layout (location = 5) in float ParticleAge;
layout (location = 6) in vec2 ParticleRotation;

out vec3 pPosition;
out vec3 pVelocity;
out float pAge;
out vec2 pRotation;

out vec3 Normal;
out vec3 Position;

out vec4 ShadowCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ShadowMatrix;
uniform mat4 ProjMatrix;

uniform float Time;
uniform float DeltaT;
uniform vec3 Accel;
uniform float ParticleLifetime;
uniform vec3 Emitter = vec3(0.0f);
uniform mat3 EmitterBasis;

uniform sampler1D RandomTex;

vec3 randomInitialVelocity()
{
	float theta = mix(0.0, PI / 6.0, texelFetch(RandomTex, 4 * gl_VertexID, 0).r);
	float phi = mix(0.0, 2.0 * PI, texelFetch(RandomTex, 4 * gl_VertexID + 1, 0).r);
	float velocity = mix(1.25, 1.5, texelFetch(RandomTex, 4 * gl_VertexID + 2, 0).r);
	vec3 v = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
	return normalize(EmitterBasis * v) * velocity;
}

float randomInitialRotationVelocity()
{
	return mix(-15.0, 15.0, texelFetch(RandomTex, 4 * gl_VertexID + 3, 0).r);
}

void Update()
{
	if (ParticleAge < 0 || ParticleAge > ParticleLifetime)
	{
		pPosition = Emitter;
		pVelocity = randomInitialVelocity();
		pRotation = vec2(0.0, randomInitialRotationVelocity());

		if (ParticleAge < 0)
		{
			pAge = ParticleAge + DeltaT;
		}
		else
		{
			pAge = (ParticleAge - ParticleLifetime) + DeltaT;
		}
	}
	else
	{
		pPosition = ParticlePosition + ParticleVelocity * DeltaT;
		pVelocity = ParticleVelocity + Accel * DeltaT;
		pRotation.x = mod(ParticleRotation.x + ParticleRotation.y * DeltaT, 2.0 * PI);
		pRotation.y = ParticleRotation.y;
		pAge = ParticleAge + DeltaT;
	}

}

void render()
{
	float cs = cos(ParticleRotation.x);
	float sn = sin(ParticleRotation.x);

	mat4 rotationAndTranslation = mat4(
	1, 0, 0, 0,
	0, cs, sn, 0,
	0, -sn, cs, 0,
	ParticlePosition.x, ParticlePosition.y, ParticlePosition.z, 1);

	mat4 m = ModelViewMatrix * rotationAndTranslation;
	Position = (m * vec4(VertexPosition, 1)).xyz;
	Normal = (m * vec4(VertexNormal, 0)).xyz;

	gl_Position = ProjMatrix *  vec4(Position, 1.0);
}

void main()
{


 if (Type == 1)
 {
	 Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
	Normal = normalize( NormalMatrix * VertexNormal );

	ShadowCoord = ShadowMatrix * vec4(VertexPosition,1.0);
	gl_Position = MVP * vec4(VertexPosition,1.0);
 }
 else
 {
	if (Pass == 1)
	{
		Update();
	}
	else
	{
		render();
	}
 }
}