#version 420 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec4 normalEnd;
}gs_in[];

void generateLine(int index);

void main()
{
    generateLine(0);
	generateLine(1);
	generateLine(2);
}

void generateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gs_in[index].normalEnd;
	EmitVertex();

	EndPrimitive();
}