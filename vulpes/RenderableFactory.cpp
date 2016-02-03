#define VULPESENGINE_EXPORT
#include "include/RenderableFactory.h"
#include "include/ResourceLoader.h"
#include "include/Material.h"

namespace vul
{
	RenderableFactory::RenderableFactory(ResourceLoader* resourceLoader)
		: m_currentID(0), m_resourceLoader(resourceLoader)
	{
	}

	uint32_t RenderableFactory::loadRenderable(const std::string& meshPath,
		const std::string& texturePath, Material* material)
	{
		RenderableObjectParameters param;
		param.id = m_currentID++;

		MeshResource* mr = m_resourceLoader->loadMeshFromFile(meshPath);
		if(mr == nullptr)
			return -1;
		param.meshResource = *mr;
		param.textureResource = *m_resourceLoader->loadTextureFromFile(texturePath);
		param.material = material;

		material->addRenderable(RenderableObject(param));

		delete mr;
		return m_currentID - 1;
	}

	uint32_t RenderableFactory::createPlane(const std::string& texturePath,
		Material* material)
	{
		RenderableObjectParameters param;
		param.id = m_currentID++;

		const uint32_t vertexCount = 12;
		float* vertices = new float[vertexCount];

		vertices[0] = -1.f;
		vertices[1] = 0.f;
		vertices[2] = -1.f;

		vertices[3] = 1.f;
		vertices[4] = 0.f;
		vertices[5] = -1.f;

		vertices[6] = -1.f;
		vertices[7] = 0.f;
		vertices[8] = 1.f;

		vertices[9] = 1.f;
		vertices[10] = 0.f;
		vertices[11] = 1.f;

		const uint32_t indexCount = 6;
		uint32_t* indices = new uint32_t[indexCount];

		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;

		indices[3] = 1;
		indices[4] = 2;
		indices[5] = 3;

		float* normals = new float[vertexCount];

		for(int32_t i = 0; i < vertexCount; i++)
			normals[i] = ((i - 1) % 3 == 0)? 1.f : 0.f; // <0.f, 1.f, 0.f>

		MeshResource* mr = m_resourceLoader->loadMeshFromData(vertices, vertexCount, indices,
			indexCount, normals);
		if(mr == nullptr)
			return -1;

		param.meshResource = *mr;
		
		if(texturePath != "")
		{
			TextureResource* tr = m_resourceLoader->loadTextureFromFile(texturePath);
			if(tr != nullptr)
			{
				param.textureResource = *tr;
				delete tr;
			}
		}

		param.material = material;
		material->addRenderable(RenderableObject(param));

		delete mr;
		delete normals;
		delete indices;
		delete vertices;

		return m_currentID - 1;
	}

	uint32_t RenderableFactory::createCylinder(int32_t resolution, float height, Material* material)
	{
		RenderableObjectParameters param;
		param.id = m_currentID++;

		const uint32_t vertexCount = resolution * 18;
		float* vertices = new float[vertexCount];
		float* normals = new float[vertexCount];

		const float faceWidth = 2.f*3.14159f / (float)resolution;

		// For each face
		for(int32_t i = 0; i < resolution; i++)
		{
			float angle1 = faceWidth * (float)i;
			float angle2 = faceWidth * (float)(i + 1);

			// These can also be used as normals
			float x1 = cosf(angle1);
			float z1 = sinf(angle1);

			float x2 = cosf(angle2);
			float z2 = sinf(angle2);

			// First poly
			vertices[18 * i] = x2;
			vertices[1 + 18 * i] = 0.f;
			vertices[2 + 18 * i] = z2;

			vertices[3 + 18 * i] = x1;
			vertices[4 + 18 * i] = 0.f;
			vertices[5 + 18 * i] = z1;

			vertices[6 + 18 * i] = x2;
			vertices[7 + 18 * i] = height;
			vertices[8 + 18 * i] = z2;

			normals[18 * i] = x2;
			normals[1 + 18 * i] = 0.f;
			normals[2 + 18 * i] = z2;

			normals[3 + 18 * i] = x1;
			normals[4 + 18 * i] = 0.f;
			normals[5 + 18 * i] = z1;

			normals[6 + 18 * i] = x2;
			normals[7 + 18 * i] = 0.f;
			normals[8 + 18 * i] = z2;

			// Second poly
			vertices[9 + 18 * i] = x1;
			vertices[10 + 18 * i] = 0.f;
			vertices[11 + 18 * i] = z1;
			
			vertices[12 + 18 * i] = x1;
			vertices[13 + 18 * i] = height;
			vertices[14 + 18 * i] = z1;
			
			vertices[15 + 18 * i] = x2;
			vertices[16 + 18 * i] = height;
			vertices[17 + 18 * i] = z2;

			normals[9 + 18 * i] = x1;
			normals[10 + 18 * i] = 0.f;
			normals[11 + 18 * i] = z1;

			normals[12 + 18 * i] = x1;
			normals[13 + 18 * i] = 0.f;
			normals[14 + 18 * i] = z1;

			normals[15 + 18 * i] = x2;
			normals[16 + 18 * i] = 0.f;
			normals[17 + 18 * i] = z2;
		}

		const uint32_t indexCount = resolution * 6;
		uint32_t* indices = new uint32_t[indexCount];

		for(int32_t i = 0; i < indexCount; i++)
			indices[i] = i;

		MeshResource* mr = m_resourceLoader->loadMeshFromData(vertices, vertexCount, indices,
			indexCount, normals);
		if(mr == nullptr)
			return -1;

		param.meshResource = *mr;
		param.material = material;
		material->addRenderable(RenderableObject(param));

		delete mr;
		delete indices;
		delete normals;
		delete vertices;

		return m_currentID - 1;
	}
}