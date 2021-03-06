#version 330 core

layout(location = 0) in vec3 vPosition;

void main()
{
    //let x * 0.75 to eliminate the viewport's extend transform on x-oriented
	gl_Position = vec4(vPosition.x * 0.75f, vPosition.y, vPosition.z, 1.0f);
}