#version 330 core
layout (location=0) in vec3 inPosition;
layout (location=1) in vec2 inUVCoords;
layout (location=2) in vec3 inFrustumRays;

out vec2 passUVCoords;
out vec3 passFrustumRays;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	passUVCoords = inUVCoords;
	passFrustumRays = inFrustumRays;
}