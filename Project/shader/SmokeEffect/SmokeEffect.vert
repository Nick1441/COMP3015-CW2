#version 460

const float PI = 3.14159265329;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexAge;

//Render pass
uniform int Pass;

// Output to transform feedback buffers update Pass
out vec3 Position;
out vec3 Velocity;
out float Age;

// Out to fragment shader
out float Transp; // Transparency
out vec2 TexCoord; // Texture coordinate 

uniform float Time; // Simulation Time
uniform float DeltaT; // Elapsed time between frames
uniform vec3 Accel; // Particle acceleration (gravity)
uniform float ParticleLifetime; // Partical lifespan
uniform vec3 Emitter = vec3(0); // World position of the Emitter
uniform mat3 EmitterBasis; // Rotation that rotates y axis to the direction of emitter
uniform float ParticleSize; // Size of particle

uniform float MinParticleSize = 0.1; // Minimum size
uniform float MaxParticleSize = 2.5; // Maximum size

uniform mat4 ModelViewMatrix;
uniform mat4 ProjMatrix;

uniform float left;
uniform float right;

uniform sampler1D RandomTex;

const vec3 offsets[] = vec3[](vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0), vec3(0.5, 0.5, 0),
            vec3(-0.5, -0.5, 0), vec3(0.5, 0.5, 0), vec3(-0.5, 0.5, 0));

const vec2 texCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,0), vec2(1,1), vec2(0,1));

vec3 randomInitialVelocity()
{
    float theta = mix(0.0, PI / 8.0, texelFetch(RandomTex, 3 * gl_VertexID, 0).r);
    float phi = mix(0.0, 2.0 * PI, texelFetch(RandomTex, 3 * gl_VertexID + 1, 0).r);
    float velocity = mix(1.25, 1.5, texelFetch(RandomTex, 3* gl_VertexID + 2, 0).r);
    vec3 v = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
    return normalize(EmitterBasis * v) * velocity;
}

vec3 randomInitialPosition()
{
    float offset = mix(left, right, texelFetch(RandomTex, 2 * gl_VertexID + 1, 0).r);
    return Emitter + vec3(offset, 0, 0);
}

void update()
{
    Age = VertexAge + DeltaT;

    if(VertexAge < 0 || VertexAge > ParticleLifetime) {

        Position = randomInitialPosition();
        Velocity = randomInitialVelocity();
        if(VertexAge > ParticleLifetime)
            Age = (VertexAge - ParticleLifetime) + DeltaT;
    } else {
        Position = VertexPosition + VertexVelocity * DeltaT;
        Velocity = VertexVelocity + Accel * DeltaT;
    }
}

void render() {
 Transp = 0.0;
 vec3 posCam = vec3(0.0);
     if( VertexAge >= 0.0 ) 
     {
         float agePct = VertexAge / ParticleLifetime;
         Transp = clamp(1.0 - agePct, 0, 1);
         posCam =
         (ModelViewMatrix * vec4(VertexPosition,1)).xyz +
         offsets[gl_VertexID] *
         mix(MinParticleSize, MaxParticleSize, agePct);
     }
     TexCoord = texCoords[gl_VertexID];
     gl_Position = ProjMatrix * vec4(posCam,1);
}

void main()
{
   if( Pass == 1 )
        update();
    else
        render();
}