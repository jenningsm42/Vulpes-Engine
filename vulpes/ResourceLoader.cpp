#define VULPESENGINE_EXPORT
#include "include/ResourceLoader.h"
#include "Logger.h"
#include <gl/glew.h>
#include <fstream>

#define FOURCC_DXT1 0x31545844 // DXT1
#define FOURCC_DXT3 0x33545844 // DXT3
#define FOURCC_DXT5 0x35545844 // DXT5

namespace vul
{
	ResourceLoader::ResourceLoader()
	{
		createPlane();
	}

	ResourceLoader::~ResourceLoader()
	{
	}

	Handle<Mesh> ResourceLoader::loadMeshFromFile(const std::string& path)
	{
		/* VULPES ENGINE MESH FORMAT SPECIFICATION
		Header:
		magic(4): int8_t[4] "VULP"
		version(2): uint16_t 0x0003
		flags(1): uint8_t
			bit 0: set = has normals
			bit 1: set = has uvcoords
			bits 2-7: reserved
		vertex count(4): uint32_t
		index count(4): uint32_t
		Data:
			vertex coordinates(vertex count): {float, float, float}
			indices(vertex count): {uint32_t}
			normals(vertex count): {float, float, float}
			uvcoords(vertex count): {float, float}
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
		if(version != 0x0003)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Invalid version number for file '%s'", path.c_str());
			f.close();
			return mesh;
		}

		uint8_t flags;
		f.read((char*)&flags, sizeof(uint8_t));
		bool hasNormals = (flags & 1) != 0;
		bool hasUVcoords = (flags & 2) != 0;

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

		MeshData meshData(vcount, icount, hasNormals, hasUVcoords); // Diffuse not supported in VEM format yet

		f.read((char*)meshData.vertices, sizeof(float) * meshData.vertexCount * 3);
		f.read((char*)meshData.indices, sizeof(uint32_t) * meshData.indexCount);
		if(hasNormals) f.read((char*)meshData.normals, sizeof(float) * meshData.vertexCount * 3);
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
		uint32_t vbo[3];

		glGenVertexArrays(1, &mesh->vao);
		glBindVertexArray(mesh->vao);

		int vboCount = 1, vboIndex = 0;
		if(meshData.normals) vboCount++;
		if(meshData.UVCoordinates) vboCount++;
		if(meshData.diffuse) vboCount++;

		glGenBuffers(vboCount, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
		glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		if(meshData.normals)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.normals, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		if(meshData.UVCoordinates)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 2 * sizeof(float), meshData.UVCoordinates, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}

		if(meshData.diffuse)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex++]);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertexCount * 3 * sizeof(float), meshData.diffuse, GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(3);
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

	Handle<Mesh> ResourceLoader::getPlane()
	{
		return m_resourceCache.getMesh("__vul_plane");
	}

	void ResourceLoader::createPlane()
	{
		MeshData meshData(4, 6, true, false, true);

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

		for(uint32_t i = 0; i < meshData.vertexCount * 3; i++)
		{
			meshData.normals[i] = ((i + 2) % 3 == 0) ? 1.f : 0.f; // <0.f, 1.f, 0.f>
			meshData.diffuse[i] = ((i + 2) % 3 == 0) ? .8f : .1f; // <.1f, .8f, .1f>
		}

		Handle<Mesh> plane = loadMeshFromData(meshData);
		if(plane.isLoaded())
			m_resourceCache.addMesh("__vul_plane", plane);
	}
}