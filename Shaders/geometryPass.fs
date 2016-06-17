#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location=13) uniform float f0;
layout (location=14) uniform float roughness;
layout (location=15) uniform sampler2D colorMap;

in float passDepthZ;
in float passDepthW;
in vec3 passNormal;
in vec2 passUVCoords;
in vec3 passDiffuse;

out vec4 outDiffuse;
out vec4 outNormal;		// 1 byte free
out vec4 outDepth;
out vec4 outMisc;		// x = f0, y = roughness, zw free

vec4 packDepth(float depth)
{
	const vec4 bitShift = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
	const vec4 bitMask = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
	vec4 res = fract(depth * bitShift);
	res -= res.xxyz * bitMask;
	return res;
}

void main()
{
	outDiffuse = texture(colorMap, passUVCoords) * vec4(passDiffuse, 1.0);
	outNormal = vec4(normalize(passNormal), 0.0);
	outDepth = packDepth(passDepthZ / passDepthW);
	outMisc = vec4(f0, roughness, 0.0, 0.0);
}