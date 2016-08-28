#version 330 core
in vec2 passUVCoords;

out vec4 outColor;

float G1Schlick(float cosineFactor, float roughness)
{
	float k = (roughness * roughness) / 2.0;
	return cosineFactor / (cosineFactor * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float roughness)
{
	return G1Schlick(NdotV, roughness) * G1Schlick(NdotL, roughness);
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

vec2 integrateBRDF(float roughness, float NdotV)
{
	vec3 view = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
	
	float a = 0;
	float b = 0;
	
	const uint numSamples = 1024u;
	for(uint i = 0u; i < numSamples; i++)
	{
		vec2 Xi = hammersley(i, numSamples);
		vec3 half = importanceSampleGGX(Xi, roughness, vec3(0.0, 0.0, 1.0));
		vec3 light = 2.0 * dot(view, half) * half - view;
		
		float NdotL = max(light.z, 0.0);
		float NdotH = max(half.z, 0.001);
		float VdotH = max(dot(view, half), 0.001);
		
		if(NdotL > 0.0)
		{
			float G = geometrySmith(NdotV, NdotL, roughness);
			float G_Vis = G * VdotH / (NdotH * NdotV);
			float Fc = pow(1.0 - VdotH, 5.0);
			
			a += (1.0 - Fc) * G_Vis;
			b += Fc * G_Vis;
		}
	}
	
	return vec2(a, b) / numSamples;
}

void main()
{
	vec2 integration = integrateBRDF(passUVCoords.y, passUVCoords.x);
	outColor = vec4(integration.x, integration.y, 0.0, 0.0);
}