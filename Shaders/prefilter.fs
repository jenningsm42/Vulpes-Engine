#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location=0) uniform sampler2D textures[4];
layout (location=4) uniform samplerCube cubeMaps[4];

layout (std140) uniform DataBlock
{
	float roughness;
} data;

in vec2 passUVCoords;

out vec3 outPosX;
out vec3 outNegX;
out vec3 outPosY;
out vec3 outNegY;
out vec3 outPosZ;
out vec3 outNegZ;

float radicalInverse(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // 0x100000000
}

vec2 hammersley(uint i, uint n)
{
	return vec2(float(i) / float(n), radicalInverse(i));
}

vec3 importanceSampleGGX(vec2 Xi, float roughness, vec3 normal) // from UE4
{
	float alpha = roughness * roughness;
	
	float phi = 2 * 3.14159 * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha * alpha - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	vec3 half = vec3(sinTheta * cos(phi), cosTheta, sinTheta * sin(phi));
	
	vec3 up = abs(normal.y) < 0.999? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentZ = cross(normal, tangentX);
	
	return tangentX * half.x + normal * half.y + tangentZ * half.z;
}

vec3 prefilter(vec3 ray)
{
	vec3 normal = ray;
	vec3 view = ray;
	
	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;
	
	const uint numSamples = 4096u;
	for(uint i = 0u; i < numSamples; i++)
	{
		vec2 Xi = hammersley(i, numSamples);
		vec3 half = importanceSampleGGX(Xi, data.roughness, normal);
		vec3 light = 2.0 * dot(view, half) * half - view;
		
		float NdotL = dot(normal, light);
		if(NdotL > 0.0)
		{
			prefilteredColor += textureLod(cubeMaps[0], light, 0.0).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	
	return prefilteredColor / totalWeight;
}

void main()
{
	float hVal = mix(-1.0, 1.0, passUVCoords.x);
	float vVal = mix(1.0, -1.0, passUVCoords.y);
	vec3 rayPosX = normalize(vec3(1.0, vVal, -hVal));
	vec3 rayNegX = normalize(vec3(-1.0, vVal, hVal));
	vec3 rayPosY = normalize(vec3(hVal, 1.0, -vVal));
	vec3 rayNegY = normalize(vec3(hVal, -1.0, vVal));
	vec3 rayPosZ = normalize(vec3(hVal, vVal, 1.0));
	vec3 rayNegZ = normalize(vec3(-hVal, vVal, -1.0));

	//outPosX = textureLod(cubeMaps[0], rayPosX, 0.0).rgb;
	outPosX = prefilter(rayPosX);
	outNegX = prefilter(rayNegX);
	//outPosY = textureLod(cubeMaps[0], rayPosY, 0.0).rgb;
	outPosY = prefilter(rayPosY);
	outNegY = prefilter(rayNegY);
	//outPosZ = textureLod(cubeMaps[0], rayPosZ, 0.0).rgb;
	outPosZ = prefilter(rayPosZ);
	outNegZ = prefilter(rayNegZ);
}