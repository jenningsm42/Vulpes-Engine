#define VULPESENGINE_EXPORT
#include "include/ResourceLoader.h"
#include "Logger.h"
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <fstream>

#define FOURCC_DXT1 0x31545844 // DXT1
#define FOURCC_DXT3 0x33545844 // DXT3
#define FOURCC_DXT5 0x35545844 // DXT5

namespace vul
{
	ResourceLoader::ResourceLoader()
	{
		createPlane();
		createSphere();
	}

	ResourceLoader::~ResourceLoader()
	{
	}

	Handle<Mesh> ResourceLoader::loadMeshFromFile(const std::string& path)
	{
		/* VULPES ENGINE MESH FORMAT SPECIFICATION
		Header:
		magic(4): int8_t[4] "VULP"
		version(2): uint16_t 0x0004
		flags(1): uint8_t
			bit 0: set = has normals
			bit 1: set = has uvcoords
			bit 2: set = has tangent and bitangents
			bits 3-7: reserved
		vertex count(4): uint32_t
		index count(4): uint32_t
		Data:
			vertex coordinates(vertex count): {float, float, float}
			indices(vertex count): {uint32_t}
			normals(vertex count): {float, float, float}
			uvcoords(vertex count): {float, float}
			tangents(vertex count): {float, float, float}
			bitangents(vertex count): {float, float, float}
		*/

		if(m_resourceCache.hasResource(path))
			return m_resourceCache.getMesh(path);

		Handle<Mesh> mesh;

		std::ifstream f(path, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Failed to open file '%s'", path.c_str());
			return mesh;
		}

		int8_t magic[4];
		f.read((char*)magic, 4);
		if(memcmp(magic, "VULP", 4))
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Invalid mesh format for file '%s'", path.c_str());
			f.close();
			return mesh;
		}

		uint16_t version = 0;
		f.read((char*)&version, sizeof(uint16_t));
		if(version != 0x0005)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Invalid version number for file '%s'", path.c_str());
			f.close();
			return mesh;
		}

		uint8_t flags;
		f.read((char*)&flags, sizeof(uint8_t));
		bool hasNormals = (flags & 1) != 0;
		bool hasUVcoords = (flags & 2) != 0;
		bool hasTB = (flags & 4) != 0;

		uint32_t vcount = 0;
		f.read((char*)&vcount, sizeof(uint32_t));
		if(vcount == 0)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: No vertices in file '%s'", path.c_str());
			f.close();
			return mesh;
		}

		uint32_t icount = 0;
		f.read((char*)&icount, sizeof(uint32_t));
		if(icount == 0)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: No indices in file '%s'", path.c_str());
			f.close();
			return mesh;
		}

		MeshData meshData(vcount, icount, hasNormals, hasUVcoords, hasTB); // Diffuse not supported in VEM format yet

		f.read((char*)meshData.vertices, sizeof(float) * meshData.vertexCount * 3);
		f.read((char*)meshData.indices, sizeof(uint32_t) * meshData.indexCount);
		if(hasNormals) f.read((char*)meshData.normals, sizeof(float) * meshData.vertexCount * 3);
		if(hasTB)
		{
			f.read((char*)meshData.tangents, sizeof(float) * meshData.vertexCount * 3);
			f.read((char*)meshData.bitangents, sizeof(float) * meshData.vertexCount * 3);
		}
		if(hasUVcoords) f.read((char*)meshData.UVCoordinates, sizeof(float) * meshData.vertexCount * 2);
		f.close();

		mesh = loadMeshFromData(meshData);

		if(mesh.isLoaded())
			m_resourceCache.addMesh(path, mesh);

		return mesh;
	}

	Handle<Mesh> ResourceLoader::loadMeshFromData(const MeshData& meshData)
	{
		Handle<Mesh> mesh;

		if(!meshData.vertices)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromData: No data in vertices");
			return mesh;
		}

		if(!meshData.indices)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromData: No data in indices");
			return mesh;
		}

		mesh->ic = meshData.indexCount;
		uint32_t vbo[6];

		glGenVertexArrays(1, &mesh->vao);
		glBindVertexArray(mesh->vao);

		int vboCount = 1, vboIndex = 0;
		if(meshData.normals) vboCount++;
		if(meshData.tangents && meshData.bitangents) vboCount += 2; // Need both
		if(meshData.UVCoordinates) vboCount++;

		glGenBuffers(vboCount, vbo);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
		glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// Normals
		if(meshData.normals)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.normals, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		// Tangents and bitangents
		if(meshData.tangents && meshData.bitangents)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.tangents, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(3);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.bitangents, GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(4);
		}

		// UV Coordinates
		if(meshData.UVCoordinates)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 2 * sizeof(float), meshData.UVCoordinates, GL_STATIC_DRAW);
			glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}

		glBindVertexArray(0);

		glGenBuffers(1, &mesh->ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indexCount * sizeof(uint32_t), meshData.indices, GL_STATIC_DRAW);

		mesh.setLoaded();
		return mesh;
	}

	Handle<Texture> ResourceLoader::loadTextureFromFile(const std::string& path)
	{
		if(m_resourceCache.hasResource(path))
			return m_resourceCache.getTexture(path);

		Handle<Texture> texture;
		uint8_t header[124];

		std::ifstream f(path, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Unable to open file '%s'", path.c_str());
			return texture;
		}

		char magic[4];
		f.read(magic, 4);
		if(strncmp(magic, "DDS ", 4))
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Invalid file format for file '%s' (must be DDS)", path.c_str());
			f.close();
			return texture;
		}

		f.read((char*)header, 124);
		uint32_t height = *(uint32_t*)&header[8];
		uint32_t width = *(uint32_t*)&header[12];
		uint32_t linearSize = *(uint32_t*)&header[16];
		uint32_t mipmapCount = *(uint32_t*)&header[24];
		uint32_t fourCC = *(uint32_t*)&header[80];

		if(mipmapCount == 0) mipmapCount = 1;

		uint32_t len = mipmapCount > 1 ? linearSize * 2 : linearSize;
		uint8_t* buffer = new uint8_t[len];
		f.read((char*)buffer, len);
		f.close();

		uint32_t format = 0;
		switch(fourCC)
		{
		case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
		case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
		case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
		default:
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: DDS file '%s' is not DXT1, DXT3, or DXT5", path.c_str());
			delete[] buffer;
			return texture;
		} break;
		}

		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_2D, texture->textureHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapCount - 1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.f);

		uint32_t blockSize = (fourCC == FOURCC_DXT1 ? 8 : 16);
		uint32_t offset = 0;

		for(uint32_t level = 0; level < mipmapCount && (width || height); level++)
		{
			uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, &buffer[offset]);
			offset += size;
			width /= 2;
			height /= 2;
		}

		delete[] buffer;

		m_resourceCache.addTexture(path, texture);

		texture.setLoaded();
		return texture;
	}

	Handle<Texture> ResourceLoader::loadTextureFromColor(float red, float green, float blue)
	{
		char path[25];
		sprintf(path, "__vul_r%1.3fg%1.3fb%1.3f", red, green, blue);

		if(m_resourceCache.hasResource(path))
			m_resourceCache.getTexture(path);

		Handle<Texture> texture;

		float* data = new float[3];
		data[0] = red;
		data[1] = green;
		data[2] = blue;

		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_2D, texture->textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, data);

		delete[] data;

		m_resourceCache.addTexture(path, texture);

		texture.setLoaded();
		return texture;
	}

	Handle<Texture> ResourceLoader::loadCubeMap(const std::string& frontPath, const std::string & backPath, const std::string & topPath, const std::string & bottomPath, const std::string & leftPath, const std::string & rightPath)
	{
		std::string resourcePath = frontPath + backPath + topPath + bottomPath + leftPath + rightPath;
		if(m_resourceCache.hasResource(resourcePath))
			return m_resourceCache.getTexture(resourcePath);

		Handle<Texture> texture;

		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureHandle);

		bool success = loadCubeMapSide(rightPath, texture, 0);
		success &= loadCubeMapSide(leftPath, texture, 1);
		success &= loadCubeMapSide(topPath, texture, 2);
		success &= loadCubeMapSide(bottomPath, texture, 3);
		success &= loadCubeMapSide(frontPath, texture, 4);
		success &= loadCubeMapSide(backPath, texture, 5);

		if(!success) return texture; // Error message in loadCubeMapSide

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_resourceCache.addTexture(resourcePath, texture);

		texture.setLoaded();
		return texture;
	}

	Handle<Mesh> ResourceLoader::getPlane()
	{
		return m_resourceCache.getMesh("__vul_plane");
	}

	Handle<Mesh> ResourceLoader::getSphere()
	{
		return m_resourceCache.getMesh("__vul_sphere");
	}

	void ResourceLoader::createPlane()
	{
		MeshData meshData(4, 6, true, true, true);

		meshData.vertices[0] = -1.f;
		meshData.vertices[1] = 0.f;
		meshData.vertices[2] = -1.f;

		meshData.vertices[3] = 1.f;
		meshData.vertices[4] = 0.f;
		meshData.vertices[5] = -1.f;

		meshData.vertices[6] = -1.f;
		meshData.vertices[7] = 0.f;
		meshData.vertices[8] = 1.f;

		meshData.vertices[9] = 1.f;
		meshData.vertices[10] = 0.f;
		meshData.vertices[11] = 1.f;

		meshData.indices[0] = 0;
		meshData.indices[1] = 2;
		meshData.indices[2] = 1;

		meshData.indices[3] = 1;
		meshData.indices[4] = 2;
		meshData.indices[5] = 3;

		meshData.UVCoordinates[0] = 0.f;
		meshData.UVCoordinates[1] = 0.f;

		meshData.UVCoordinates[2] = 40.f;
		meshData.UVCoordinates[3] = 0.f;

		meshData.UVCoordinates[4] = 0.f;
		meshData.UVCoordinates[5] = 40.f;

		meshData.UVCoordinates[6] = 40.f;
		meshData.UVCoordinates[7] = 40.f;

		for(uint32_t i = 0; i < meshData.vertexCount * 3; i++)
		{
			meshData.normals[i] = ((i + 2) % 3 == 0) ? 1.f : 0.f; // <0.f, 1.f, 0.f>
			meshData.tangents[i] = (i % 3 == 0) ? 1.f : 0.f; // <1.f, 0.f, 0.f>
			meshData.bitangents[i] = ((i + 1) % 3 == 0)? 0.f : -1.f; // <0.f, 0.f, -1.f>
		}

		Handle<Mesh> plane = loadMeshFromData(meshData);
		if(plane.isLoaded())
			m_resourceCache.addMesh("__vul_plane", plane);
	}

	void ResourceLoader::createSphere()
	{
		MeshData meshData(32 * 30 + 2, (32 * 2 * 30) * 3, true, true, true);

		// Top vertex
		meshData.vertices[0] = 0.f;
		meshData.vertices[1] = 1.f;
		meshData.vertices[2] = 0.f;

		meshData.normals[0] = 0.f;
		meshData.normals[1] = 1.f;
		meshData.normals[2] = 0.f;

		meshData.tangents[0] = 1.f;
		meshData.tangents[1] = 0.f;
		meshData.tangents[2] = 0.f;

		meshData.bitangents[0] = 0.f;
		meshData.bitangents[1] = 0.f;
		meshData.bitangents[2] = -1.f;

		meshData.UVCoordinates[0] = 0.5f;
		meshData.UVCoordinates[1] = 0.f;

		// 30 rings of 32 vertices in the middle
		uint32_t iter = 1;
		for(uint32_t i = 1; i < 31; i++)
		{
			float vAngle = 3.14159f * (float)i / 32.f; // [0, pi]
			float y = cosf(vAngle);

			for(uint32_t j = 0; j < 32; j++)
			{
				float hAngle = 3.14159f * (float)j / 16.f; // [0, 2pi]
				float x = cosf(hAngle) * sinf(vAngle);
				float z = sinf(hAngle) * sinf(vAngle);

				// Add vertex
				meshData.vertices[iter * 3] = x;
				meshData.vertices[iter * 3 + 1] = y;
				meshData.vertices[iter * 3 + 2] = z;

				// Normals are equivalent
				meshData.normals[iter * 3] = x;
				meshData.normals[iter * 3 + 1] = y;
				meshData.normals[iter * 3 + 2] = z;

				// Tangents are rotated pi/2 rad
				meshData.tangents[iter * 3] = cosf(hAngle - 3.14159f / 2.f);
				meshData.tangents[iter * 3 + 1] = sinf(hAngle - 3.14159f / 2.f);
				meshData.tangents[iter * 3 + 2] = 0.f;

				// Bitangent is cross product of N and T
				glm::vec3 n = glm::vec3(meshData.normals[iter * 3],
										meshData.normals[iter * 3 + 1],
										meshData.normals[iter * 3 + 2]);
				glm::vec3 t = glm::vec3(meshData.tangents[iter * 3],
										meshData.tangents[iter * 3 + 1],
										meshData.tangents[iter * 3 + 2]);
				glm::vec3 b = glm::cross(n, t);

				meshData.bitangents[iter * 3] = b.x;
				meshData.bitangents[iter * 3 + 1] = b.y;
				meshData.bitangents[iter * 3 + 2] = b.z;

				// Calculate UV coordinates
				meshData.UVCoordinates[iter * 2] = 0.5f + atan2f(z, x) / (2.f * 3.14159f);
				meshData.UVCoordinates[iter * 2 + 1] = 0.5f - asinf(y) / 3.14159f;

				iter++;
			}
		}

		// Bottom vertex
		meshData.vertices[2883] = 0.f;
		meshData.vertices[2884] = -1.f;
		meshData.vertices[2885] = 0.f;

		meshData.normals[2883] = 0.f;
		meshData.normals[2884] = -1.f;
		meshData.normals[2885] = 0.f;

		meshData.tangents[2883] = -1.f;
		meshData.tangents[2884] = 0.f;
		meshData.tangents[2885] = 0.f;

		meshData.bitangents[2883] = 0.f;
		meshData.bitangents[2884] = 0.f;
		meshData.bitangents[2885] = 1.f;

		meshData.UVCoordinates[1922] = 0.5f;
		meshData.UVCoordinates[1923] = 1.f;

		// Top indices
		for(uint32_t i = 0; i < 31; i++)
		{
			meshData.indices[i * 3] = 0;
			meshData.indices[i * 3 + 1] = i + 2;
			meshData.indices[i * 3 + 2] = i + 1;
		}

		// Middle indices
		iter = 96;
		for(uint32_t i = 0; i < 29; i++)
		{
			for(uint32_t j = 0; j < 31; j++)
			{
				meshData.indices[iter++] = i * 32 + j + 1;
				meshData.indices[iter++] = i * 32 + j + 2;
				meshData.indices[iter++] = (i + 1) * 32 + j + 1;

				meshData.indices[iter++] = (i + 1) * 32 + j + 1;
				meshData.indices[iter++] = i * 32 + j + 2;
				meshData.indices[iter++] = (i + 1) * 32 + j + 2;
			}
		}

		// Bottom indices
		for(uint32_t i = 0; i < 31; i++)
		{
			uint32_t offset = i + 1888;
			meshData.indices[offset * 3] = 961;
			meshData.indices[offset * 3 + 1] = i + 929;
			meshData.indices[offset * 3 + 2] = i + 930;
		}

		// Stitch rest of sphere together
		// Top
		meshData.indices[93] = 0;
		meshData.indices[94] = 1;
		meshData.indices[95] = 32;

		// Middle
		for(uint32_t i = 0; i < 29; i++)
		{
			meshData.indices[iter++] = (i + 1) * 32;
			meshData.indices[iter++] = i * 32 + 1;
			meshData.indices[iter++] = (i + 2) * 32;

			meshData.indices[iter++] = i * 32 + 1;
			meshData.indices[iter++] = (i + 1) * 32 + 1;
			meshData.indices[iter++] = (i + 2) * 32;
		}

		// Bottom
		meshData.indices[5757] = 961;
		meshData.indices[5758] = 960;
		meshData.indices[5759] = 929;

		Handle<Mesh> sphere = loadMeshFromData(meshData);
		if(sphere.isLoaded())
			m_resourceCache.addMesh("__vul_sphere", sphere);
	}

	bool ResourceLoader::loadCubeMapSide(const std::string& path, Handle<Texture> texture, uint32_t side)
	{
		// TODO: Cache cubemap sides
		// TODO: Create more general DDS loader to reduce redundant code
		uint8_t header[124];

		std::ifstream f(path, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Unable to open file '%s'", path.c_str());
			return false;
		}

		char magic[4];
		f.read(magic, 4);
		if(strncmp(magic, "DDS ", 4))
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Invalid file format for file '%s' (must be DDS)", path.c_str());
			f.close();
			return false;
		}

		f.read((char*)header, 124);
		uint32_t height = *(uint32_t*)&header[8];
		uint32_t width = *(uint32_t*)&header[12];
		uint32_t linearSize = *(uint32_t*)&header[16];
		uint32_t mipmapCount = *(uint32_t*)&header[24];
		uint32_t fourCC = *(uint32_t*)&header[80];

		if(mipmapCount == 0) mipmapCount = 1;

		uint32_t len = mipmapCount > 1 ? linearSize * 2 : linearSize;
		uint8_t* buffer = new uint8_t[len];
		f.read((char*)buffer, len);
		f.close();

		uint32_t format = 0;
		switch(fourCC)
		{
		case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
		case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
		case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
		default:
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: DDS file '%s' is not DXT1, DXT3, or DXT5", path.c_str());
			delete[] buffer;
			return false;
		} break;
		}

		uint32_t blockSize = (fourCC == FOURCC_DXT1 ? 8 : 16);
		uint32_t offset = 0;

		for(uint32_t level = 0; level < mipmapCount && (width || height); level++)
		{
			uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, level, format, width, height, 0, size, &buffer[offset]);
			offset += size;
			width /= 2;
			height /= 2;
		}

		delete[] buffer;

		return true;
	}
}