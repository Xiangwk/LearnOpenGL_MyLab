#version 330 core

in vec3 Color;
in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
    //mix two textures:first 0.7, second 0.3
	fragColor = mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.3f);
}