#version 420 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D grass;

void main()
{
    vec4 texColor = texture(grass, TexCoord);
	if(texColor.a < 0.1) discard;
    color = texColor;
}