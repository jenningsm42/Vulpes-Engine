#version 330 core
layout (location=0) in vec3 inPosition;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec3 inTangent;
layout (location=3) in vec3 inBitangent;
layout (location=4) in vec2 inUVCoords;

out vec2 passUVCoords;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	passUVCoords = inUVCoords;
}