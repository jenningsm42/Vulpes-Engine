#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#define LIGHT_COUNT 3

struct PointLight // 3 uniform locations
{
	vec3 position;
	vec3 color;
	float intensity;
};

layout (location=0) uniform sampler2D tColor;
layout (location=1) uniform sampler2D tNormal;
layout (location=2) uniform sampler2D tDepth;
layout (location=3) uniform sampler2D tMisc;
layout (location=4) uniform float near;
layout (location=5) uniform float far;
layout (location=6) uniform PointLight light[LIGHT_COUNT];

in vec2 passUVCoords;
in vec3 passFrustumRays;

out vec4 outColor;

float unpackDepth(vec4 packedDepth)
{
	const vec4 bitShift = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
	return dot(packedDepth, bitShift);
}

float getDepth()
{
	// Linearizes depth
	return (2 * near) / (far + near - unpackDepth(texture2D(tDepth, passUVCoords)) * (far - near));
}

vec3 getPixelPosition(float depth)
{
	return depth * passFrustumRays;
}

float fresnelSchlick(float f0, float cosineFactor)
{
	return f0 + (1.0 - f0) * exp2((-5.55473 * cosineFactor - 6.98316) * cosineFactor); // UE4 method
}

float distributionGGX(float NdotH, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;
	float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (3.14159 * denom * denom);
}

float G1Schlick(float cosineFactor, float roughness)
{
	float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	return cosineFactor / (cosineFactor * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float roughness)
{
	return G1Schlick(NdotV, roughness) * G1Schlick(NdotL, roughness);
}

float brdfSpec(float f0, float NdotH, float NdotV, float NdotL, float HdotV, float roughness)
{
	float f = fresnelSchlick(f0, HdotV);
	float d = distributionGGX(NdotH, roughness);
	float g = geometrySmith(NdotV, NdotL, roughness);
	
	return f * d * g / (4.0 * NdotL * NdotV);
}

vec3 brdfLambert(vec3 reflectivity)
{
	return reflectivity / 3.14159;
}

vec3 radiance(float spec, vec3 diff, float NdotL)
{
	return (vec3(spec) + diff) * NdotL;
}

vec3 calculateLightContribution(PointLight pl, vec3 view, vec3 normal, vec3 position, vec3 color, float f0, float roughness)
{
	// Calculate light value based on distance and attenuation
	vec3 pixelToLight = pl.position - position;
	float dist = length(pixelToLight);
	float radius = 0.5; // Testing spherical lighting
	float att_denom = 1.0 + dist / radius;
	float attenuation = 1.0 / (att_denom * att_denom);
	vec3 lightValue = pl.color * pl.intensity;
	
	// Continue?
	if(attenuation < 0.00001) return vec3(0, 0, 0);
	
	// Calculate light and half vectors
	vec3 light = normalize(pixelToLight);
	vec3 half = normalize(light + view);

	// Calculate required dot products, clamped
	float NdotH = max(dot(normal, half), 0.001);
	float NdotV = max(dot(normal, view), 0.001);
	float NdotL = max(dot(normal, light), 0.001);
	float HdotV = max(dot(half, view), 0.001);
	
	// Calculate specular lighting
	float spec = brdfSpec(f0, NdotH, NdotV, NdotL, HdotV, roughness);
	
	// Calculate diffuse
	vec3 diff = brdfLambert(color);
	
	return radiance(spec, diff, NdotL) * attenuation * lightValue;
}

void main()
{
	// Unpack G-buffer
	vec3 color = texture2D(tColor, passUVCoords).xyz; // No support for alpha channel yet
	vec3 normal = texture2D(tNormal, passUVCoords).xyz;
	float depth = getDepth();
	vec2 misc = texture2D(tMisc, passUVCoords).xy;
	float f0 = misc.x;
	float roughness = misc.y;
	
	//test
	color = pow(color, vec3(2.2));
	
	vec3 position = getPixelPosition(depth);
	vec3 view = normalize(-position); // in view space already
	
	vec3 totalLightContribution = vec3(0.0);
	for(int i = 0; i < LIGHT_COUNT; i++)
		totalLightContribution += calculateLightContribution(light[i], view, normal, position, color, f0, roughness);
	
	// Gamma correction
	vec3 gamma = vec3(1.0 / 2.2);
	vec3 finalColor = pow(totalLightContribution, gamma);
	
	outColor = vec4(finalColor, 1.0);
}