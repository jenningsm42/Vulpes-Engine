#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location=16) uniform sampler2D colorMap;
layout (location=17) uniform sampler2D normalMap;
layout (location=18) uniform sampler2D roughnessMap;
layout (location=19) uniform sampler2D metalMap;

in float passDepthZ;
in float passDepthW;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;
in vec2 passUVCoords;

out vec4 outAlbedo;
out vec2 outNormal;
out vec4 outDepth;
out vec4 outMisc;		// x = metal, y = roughness, zw unused

vec4 encodeDepth(float depth)
{
	const vec4 bitShift = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
	const vec4 bitMask = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
	vec4 res = fract(depth * bitShift);
	res -= res.xxyz * bitMask;
	return res;
}

vec2 encodeNormal(vec3 normal)
{
	float f = sqrt(8.0 * normal.z + 8.0);
	return normal.xy / f + vec2(0.5);
}

void main()
{
	mat3 TBN = mat3(
		normalize(passTangent),
		normalize(passBitangent),
		normalize(passNormal));
	
	vec3 tangentNormal = texture(normalMap, passUVCoords).xyz * 2.0 - vec3(1.0);

	outAlbedo = texture(colorMap, passUVCoords);
	//outNormal = encodeNormal(normalize(passNormal)); // No normal map
    outNormal = encodeNormal(normalize(TBN * tangentNormal));
	outDepth = encodeDepth(passDepthZ / passDepthW);
	outMisc = vec4(clamp(texture(metalMap, passUVCoords).x, 0.03, 0.99), texture(roughnessMap, passUVCoords).x, 0.0, 0.0);
}