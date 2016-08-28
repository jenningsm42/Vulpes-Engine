#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location=0) uniform mat4 viewMat;
layout (location=4) uniform mat4 projMat;
layout (location=8) uniform mat4 modelMat;
layout (location=12) uniform mat3 normalMat;
layout (location=15) uniform float near;

layout (location=0) in vec3 inPosition;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec3 inTangent;
layout (location=3) in vec3 inBitangent;
layout (location=4) in vec2 inUVCoords;

out float passDepthZ;
out float passDepthW;
out vec3 passNormal;
out vec3 passTangent;
out vec3 passBitangent;
out vec2 passUVCoords;

void main()
{
	vec4 calculatedPosition = projMat * viewMat * modelMat * vec4(inPosition, 1.0);
	mat3 viewMat3 = mat3(viewMat);
	
	gl_Position = calculatedPosition;
	passDepthZ = calculatedPosition.z + near;
	passDepthW = calculatedPosition.w + near;
	passNormal = viewMat3 * normalMat * inNormal;
	passTangent = viewMat3 * inTangent;
	passBitangent = viewMat3 * inBitangent;
	passUVCoords = inUVCoords;
}