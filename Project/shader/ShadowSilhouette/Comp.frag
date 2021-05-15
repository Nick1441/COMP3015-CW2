#version 460

in vec3 Position;
in vec3 Normal;

//Uniform Inputs.
uniform sampler2D DiffSpecTex;

layout( location = 0 ) out vec4 FragColor;

void main()
{
	//Setting DiffSpec Using Tex Fetch
  vec4 diffSpec = texelFetch(DiffSpecTex, ivec2(gl_FragCoord), 0);

  FragColor = vec4(diffSpec.xyz, 1);
}