#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location=0) uniform mat4 viewMat;
layout (location=4) uniform mat4 projMat;
layout (location=8) uniform mat4 modelMat;
layout (location=12) uniform float near;

layout (location=0) in vec3 inPosition;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 inUVCoords;
layout (location=3) in vec3 inDiffuse;

out float passDepthZ;
out float passDepthW;
out vec3 passNormal;
out vec2 passUVCoords;
out vec3 passDiffuse;

void main()
{
	vec4 calculatedPosition = projMat * viewMat * modelMat * vec4(inPosition, 1.0);
	gl_Position = calculatedPosition;
	passDepthZ = calculatedPosition.z + near;
	passDepthW = calculatedPosition.w + near;
	passNormal = mat3(viewMat) * inNormal;
	passUVCoords = inUVCoords;
	passDiffuse = inDiffuse;
}