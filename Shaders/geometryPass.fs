#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location=13) uniform float f0;
layout (location=14) uniform sampler2D colorMap;
layout (location=15) uniform sampler2D normalMap;
layout (location=16) uniform sampler2D roughnessMap;

in float passDepthZ;
in float passDepthW;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;
in vec2 passUVCoords;

out vec4 outDiffuse;
out vec4 outNormal;		// 1 byte free
out vec4 outDepth;
out vec4 outMisc;		// x = f0, y = roughness, zw unused

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
	mat3 TBN = mat3(
		normalize(passTangent),
		normalize(passBitangent),
		normalize(passNormal));
	
	vec3 tangentNormal = texture(normalMap, passUVCoords).xyz * 2.0 - vec3(1.0);

	outDiffuse = texture(colorMap, passUVCoords);
	//outNormal = vec4(passNormal, 0.0); // No normal map
    outNormal = vec4(normalize(TBN * tangentNormal), 0.0);
	outDepth = packDepth(passDepthZ / passDepthW);
	outMisc = vec4(f0, texture(roughnessMap, passUVCoords).x, 0.0, 0.0);
}