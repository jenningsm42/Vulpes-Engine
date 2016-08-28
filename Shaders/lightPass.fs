#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#define LIGHT_COUNT 1

struct PointLight // 4 uniform locations
{
	vec3 position;
	vec3 color;
	float brightness;
	float radius;
};

layout (location=0) uniform sampler2D tColor;
layout (location=1) uniform sampler2D tNormal;
layout (location=2) uniform sampler2D tDepth;
layout (location=3) uniform sampler2D tMisc;
layout (location=4) uniform float near;
layout (location=5) uniform float far;
layout (location=6) uniform mat3 invViewMat;
layout (location=9) uniform samplerCube tEnvironment;
layout (location=10) uniform samplerCube tDiffuseEnvironment;
layout (location=11) uniform float environmentMipMaps;
layout (location=12) uniform sampler2D environmentLUT;
layout (location=13) uniform PointLight light[LIGHT_COUNT];

in vec2 passUVCoords;
in vec3 passFrustumRays;

out vec4 outColor;

float decodeDepth()
{
	const vec4 bitShift = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
	return dot(texture(tDepth, passUVCoords), bitShift);
}

vec3 decodeNormal()
{
	vec2 fenc = texture(tNormal, passUVCoords).rg * 4.0 - 2.0;
	float f = dot(fenc, fenc);
	float g = sqrt(1.0 - f / 4.0);
	
	vec3 normal;
	normal.xy = fenc * g;
	normal.z = 1.0 - f / 2.0;
	return normal;
}

float getDepth()
{
	// Linearizes depth
	return (2 * near) / (far + near - decodeDepth() * (far - near));
}

vec3 getPixelPosition(float depth)
{
	return depth * passFrustumRays;
}

float fresnelSchlick(float f0, float cosineFactor)
{
	return f0 + (1.0 - f0) * exp2((-5.55473 * cosineFactor - 6.98316) * cosineFactor); // UE4 method
}

float distributionGGX(float NdotH, float alpha)
{
	float alphaSq = alpha * alpha;
	float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (3.14159 * denom * denom);
}

float G1Schlick(float cosineFactor, float roughness)
{
	//float k = (roughness + 1.0) * (roughness + 1.0) / 8.0; // Too dark for IBL at grazing angles
	float k = (roughness * roughness) / 2.0;
	return cosineFactor / (cosineFactor * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float roughness)
{
	return G1Schlick(NdotV, roughness) * G1Schlick(NdotL, roughness);
}

float brdfSpec(float f0, float NdotH, float NdotV, float NdotL, float HdotV, float roughness)
{
	float alpha = roughness * roughness;
	float f = fresnelSchlick(f0, HdotV);
	float d = distributionGGX(NdotH, alpha);
	float g = geometrySmith(NdotV, NdotL, roughness);
	
	return f * d * g / (4.0 * NdotL * NdotV);
}

vec3 brdfLambert(vec3 reflectivity)
{
	return reflectivity / 3.14159;
}

vec3 radiance(vec3 spec, vec3 diff, float NdotL)
{
	return (spec + diff) * NdotL;
}

vec3 blendMaterial(vec3 diffuse, vec3 specular, vec3 color, float metallic)
{
	return specular * mix(vec3(0.03), color, metallic) + diffuse * (1.0 - metallic);
}

vec3 calculateLightContribution(PointLight pl, vec3 view, vec3 normal, vec3 position, vec3 color, float metallic, float roughness)
{
	// Calculate light value based on distance and attenuation
	vec3 pixelToLight = pl.position - position;
	float dist = length(pixelToLight);
	float falloff = clamp(1.0 - pow(dist / pl.radius, 4.0), 0.0, 1.0);
	falloff = falloff * falloff / (dist * dist + 1.0);
	
	//if(falloff <= 0.0) return vec3(0.0); // Continue?
	vec3 lightValue = pl.color * pl.brightness * falloff;
	
	// Calculate light and half vectors
	vec3 light = normalize(pixelToLight);
	vec3 half = normalize(light + view);

	// Calculate required dot products, clamped
	float NdotH = max(dot(normal, half), 0.001);
	float NdotV = max(dot(normal, view), 0.001);
	float NdotL = max(dot(normal, light), 0.001);
	float HdotV = max(dot(half, view), 0.001);
	
	// Calculate specular lighting
	float specular = brdfSpec(metallic, NdotH, NdotV, NdotL, HdotV, roughness);
	
	// Calculate diffuse
	vec3 diffuse = brdfLambert(color);
	
	// Calculate fresnel term
	float fresnel = fresnelSchlick(metallic, HdotV);
	
	return blendMaterial(diffuse, vec3(specular), color, metallic) * NdotL;
	//return mix(diffuse, vec3(specular), fresnel) * lightValue * NdotL;
}

float radicalInverse(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // 0x100000000
}

vec2 Hammersley(uint i, uint n)
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

vec3 specIBL(vec3 specular, float roughness, vec3 normal, vec3 view) // from UE4
{
	vec3 specularLighting = vec3(0.0);
	
	const uint numSamples = 1024u;
	for(uint i = 0u; i < numSamples; i++)
	{
		vec2 Xi = Hammersley(i, numSamples);
		vec3 half = importanceSampleGGX(Xi, roughness, normal);
		vec3 light = 2 * dot(view, half) * half - view;
		
		float NdotH = max(dot(normal, half), 0.001);
		float NdotV = max(dot(normal, view), 0.001);
		float NdotL = max(dot(normal, light), 0.001);
		float HdotV = max(dot(half, view), 0.001);
		
		if(NdotL > 0.0)
		{
			vec3 sampleColor = textureLod(tEnvironment, light, 0.0).rgb;
			
			float G = geometrySmith(NdotV, NdotL, roughness);
			float Fc = pow(1.0 - HdotV, 5.0);
			vec3 F = (1.0 - Fc) * specular + Fc;
			
			specularLighting += sampleColor * F * G * HdotV / (NdotH * NdotV);
		}
	}
	
	return specularLighting / numSamples;
}

vec3 diffIBL(vec3 baseColor, vec3 normal)
{
	return texture(tDiffuseEnvironment, normal).rgb * baseColor;
}

vec3 approximateSpecIBL(vec3 specular, float roughness, vec3 normal, vec3 view)
{
	float NdotV = max(dot(normal, view), 0.001);
	vec3 ray = 2.0 * dot(normal, view) * normal - view;
	
	vec3 prefilteredColor = textureLod(tEnvironment, ray, roughness * environmentMipMaps).rgb;
	vec2 envBRDF = texture(environmentLUT, vec2(NdotV, roughness)).rg;
	
	return prefilteredColor * (specular * envBRDF.x + envBRDF.y);
}

void main()
{
	// Unpack G-buffer
	float depth = getDepth();
	vec3 position = getPixelPosition(depth);
	vec3 view = normalize(-position); // in view space already
	vec3 viewWorld = invViewMat * view;
	
	if(depth > 0.0)
	{
		// Unpack rest of G-buffer
		vec3 color = texture(tColor, passUVCoords).rgb; // No support for alpha channel yet
		vec3 normal = decodeNormal();
		vec3 normalWorld = invViewMat * normal;
		
		vec2 misc = texture(tMisc, passUVCoords).xy;
		float metallic = clamp(misc.x, 0.02, 0.99);
		float roughness = clamp(misc.y, 0.01, 1.0);
		
		
		// Calculate dynamic lighting
		vec3 totalLightContribution = vec3(0.0);
		for(int i = 0; i < LIGHT_COUNT; i++)
			totalLightContribution += calculateLightContribution(light[i], view, normal, position, color, metallic, roughness);

		vec3 spec = approximateSpecIBL(vec3(1.0), roughness, normalWorld, viewWorld);
		vec3 diff = textureLod(tDiffuseEnvironment, normalWorld, 0.0).rgb * color;
		totalLightContribution += blendMaterial(diff, spec, color, metallic);
		
		// Tone mapping
		vec3 toneMappedColor = totalLightContribution / (totalLightContribution + vec3(1.0));
			
		// Gamma correction
		vec3 gamma = vec3(1.0 / 2.2);
		vec3 finalColor = pow(toneMappedColor, gamma);
		
		outColor = vec4(finalColor, 1.0);
	}
	else outColor = vec4(textureLod(tEnvironment, viewWorld, 0.0).rgb, 1.0);
}