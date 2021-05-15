#version 460

const float PI = 3.1415926359;

uniform int Pass;

//Locations for layouts.
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

layout (location = 2) in vec2 VertexTexCoord;

layout (location = 3) in vec3 ParticlePosition;
layout (location = 4) in vec3 ParticleVelocity;
layout (location = 5) in float ParticleAge;
layout (location = 6) in vec2 ParticleRotation;

//Variables going to Feedback
out vec3 Position;
out vec3 Velocity;
out float Age;
out vec2 Rotation;

out vec3 fPosition;
out vec3 fNormal;
out vec2 TexCoord;

//Uniforms input. Can customise using ImGUI
uniform float Time;
uniform float DeltaT;
uniform vec3 Accel;
uniform float ParticleLifetime;
uniform vec3 Emitter = vec3(0.0f);
uniform mat3 EmitterBasis;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjMatrix;


//TEXTURE
uniform mat3 NormalMatrix;

layout (binding = 15) uniform sampler1D RandomTex;

//Creating Random Velocity for Each Particle, at start of life.
vec3 randomInitialVelocity()
{
	float theta = mix(0.0, PI / 6.0, texelFetch(RandomTex, 4 * gl_VertexID, 0).r);
	float phi = mix(0.0, 2.0 * PI, texelFetch(RandomTex, 4 * gl_VertexID + 1, 0).r);
	float velocity = mix(1.25, 1.5, texelFetch(RandomTex, 4 * gl_VertexID + 2, 0).r);
	vec3 v = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
	return normalize(EmitterBasis * v) * velocity;
}

//Creating Random Rotation at start of particle life
float randomInitialRotationVelocity()
{
	return mix(-15.0, 15.0, texelFetch(RandomTex, 4 * gl_VertexID + 3, 0).r);
}

//Creating Random Position at start of lifetime.
vec3 randomInitialPosition()
{
    float offset = mix(-10.0, 10.0, texelFetch(RandomTex, 2 * gl_VertexID + 1, 0).r);
    return Emitter + vec3(offset, 0, 0);
}

void Update()
{
	//If start of life, Create Rotation, Position and velocity!
	if (ParticleAge < 0 || ParticleAge > ParticleLifetime)
	{
		Position = randomInitialPosition();
		Velocity = randomInitialVelocity();
		Rotation = vec2(0.0, randomInitialRotationVelocity());
		
		if (ParticleAge < 0)
		{
			Age = ParticleAge + DeltaT;
		}
		else
		{
			Age = (ParticleAge - ParticleLifetime) + DeltaT;
		}
	}
	else
	{
		//Updating Position, velocity & Rotation on each particle.
		Position = ParticlePosition + ParticleVelocity * DeltaT;
		Velocity = ParticleVelocity + Accel * DeltaT;
		Rotation.x = mod(ParticleRotation.x + ParticleRotation.y * DeltaT, 2.0 * PI);
		
		Rotation.y = ParticleRotation.y;
		Age = ParticleAge + DeltaT;
	}

}

void render()
{
	//Render Particle using fPos & fNormal.
	float cs = cos(ParticleRotation.x);
	float sn = sin(ParticleRotation.x);

	mat4 rotationAndTranslation = mat4(
	1, 0, 0, 0,
	0, cs, sn, 0,
	0, -sn, cs, 0,
	ParticlePosition.x, ParticlePosition.y, ParticlePosition.z, 1);

	mat4 m = ModelViewMatrix * rotationAndTranslation;
	fPosition = (m * vec4(VertexPosition, 1)).xyz;
	fNormal = normalize(m * vec4(VertexNormal, 0)).xyz;

	TexCoord = VertexTexCoord;

	gl_Position = ProjMatrix * vec4(fPosition, 1.0);
}

void main()
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