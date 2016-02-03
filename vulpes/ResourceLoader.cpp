#define VULPESENGINE_EXPORT
#include "include/ResourceLoader.h"
#include "include/ResourceCache.h"
#include "Logger.h"
#include <gl/glew.h>
#include <fstream>

#define FOURCC_DXT1 0x31545844 // DXT1
#define FOURCC_DXT3 0x33545844 // DXT3
#define FOURCC_DXT5 0x35545844 // DXT5

namespace vul
{
	ResourceLoader::ResourceLoader() : m_resourceCache(new ResourceCache())
	{
	}

	ResourceLoader::~ResourceLoader()
	{
		delete m_resourceCache;
	}

	MeshResource* ResourceLoader::loadMeshFromFile(const std::string& path)
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

		if(m_resourceCache->hasResource(path))
			return (MeshResource*)m_resourceCache->getResource(path);

		std::ifstream f(path, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Failed to open file '%s'", path.c_str());
			return nullptr;
		}

		int8_t magic[4];
		f.read((char*)magic, 4);
		if(memcmp(magic, "VULP", 4))
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Invalid mesh format for file '%s'", path.c_str());
			f.close();
			return nullptr;
		}

		uint16_t version = 0;
		f.read((char*)&version, sizeof(uint16_t));
		if(version != 0x0003)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Invalid version number for file '%s'", path.c_str());
			f.close();
			return nullptr;
		}

		uint8_t flags;
		f.read((char*)&flags, sizeof(uint8_t));
		bool hasNormals = (flags & 1) != 0;
		bool hasUVcoords = (flags & 2) != 0;

		uint32_t vcount;
		f.read((char*)&vcount, sizeof(uint32_t));
		if(vcount == 0)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: No vertices in file '%s'", path.c_str());
			f.close();
			return nullptr;
		}

		uint32_t icount;
		f.read((char*)&icount, sizeof(uint32_t));
		if(icount == 0)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: No indices in file '%s'", path.c_str());
			f.close();
			return nullptr;
		}

		float* vertices = new float[vcount * 3];
		uint32_t* indices = new uint32_t[icount];
		float* normals = hasNormals ? new float[vcount * 3] : 0;
		float* uvcoords = hasUVcoords ? new float[vcount * 2] : 0;

		f.read((char*)vertices, sizeof(float) * vcount * 3);
		f.read((char*)indices, sizeof(uint32_t) * icount);
		if(hasNormals) f.read((char*)normals, sizeof(float) * vcount * 3);
		if(hasUVcoords) f.read((char*)uvcoords, sizeof(float) * vcount * 2);
		f.close();

		MeshResource* mr = loadMeshFromData(vertices, vcount, indices, icount, normals, uvcoords);

		delete[] vertices;
		delete[] indices;
		if(normals) delete[] normals;
		if(uvcoords) delete[] uvcoords;

		m_resourceCache->addResource(path, mr);

		return mr;
	}

	MeshResource* ResourceLoader::loadMeshFromData(float* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount,
		float* normals, float* uvcoords)
	{
		MeshResource* mr = new MeshResource();

		if(!vertices)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromData: No data in vertices");
			return nullptr;
		}

		if(!indices)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromData: No data in indices");
			return nullptr;
		}

		mr->ic = indexCount;
		uint32_t vbo[3];

		glGenVertexArrays(1, &mr->vao);
		glBindVertexArray(mr->vao);

		glGenBuffers(normals ? (uvcoords ? 3 : 2) : (uvcoords ? 2 : 1), vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		if(normals)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), normals, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		if(uvcoords)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[normals ? 2 : 1]);
			glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), uvcoords, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}

		glBindVertexArray(0);

		glGenBuffers(1, &mr->ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mr->ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

		return mr;
	}

	TextureResource* ResourceLoader::loadTextureFromFile(const std::string& path)
	{
		if(m_resourceCache->hasResource(path))
			return (TextureResource*)m_resourceCache->getResource(path);

		uint8_t header[124];
		TextureResource* tr = new TextureResource();

		std::ifstream f(path, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Unable to open file '%s'", path.c_str());
			return nullptr;
		}

		char magic[4];
		f.read(magic, 4);
		if(strncmp(magic, "DDS ", 4))
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Invalid file format for file '%s' (must be DDS)", path.c_str());
			f.close();
			return nullptr;
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
			return nullptr;
		} break;
		}

		glGenTextures(1, &tr->textureHandle);
		glBindTexture(GL_TEXTURE_2D, tr->textureHandle);
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

		m_resourceCache->addResource(path, tr);

		return tr;
	}

	MaterialResource* ResourceLoader::loadMaterialFromFile(const std::string& vsPath, const std::string& fsPath)
	{
		if(m_resourceCache->hasResource(vsPath))
			return (MaterialResource*)m_resourceCache->getResource(vsPath);

		std::ifstream f(vsPath, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadMaterialFromFile: Unable to open file '%s'", vsPath.c_str());
			return nullptr;
		}

		f.seekg(0, std::ios::end);
		uint32_t size = f.tellg();
		int8_t* vsContent = new int8_t[size + 1];
		f.seekg(0, std::ios::beg);

		f.read((char*)vsContent, sizeof(int8_t) * size);
		vsContent[size] = 0;
		f.close();

		f.open(fsPath, std::ifstream::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::loadMaterialFromFile: Unable to open file '%s'", fsPath.c_str());
			delete[] vsContent;
			return nullptr;
		}

		f.seekg(0, std::ios::end);
		size = f.tellg();
		int8_t* fsContent = new int8_t[size + 1];
		f.seekg(0, std::ios::beg);

		f.read((char*)fsContent, sizeof(int8_t) * size);
		fsContent[size] = 0;
		f.close();

		MaterialResource* mr = loadMaterialFromText(vsContent, fsContent);

		delete[] vsContent;
		delete[] fsContent;

		m_resourceCache->addResource(vsPath, mr);

		return mr;
	}

	MaterialResource* ResourceLoader::loadMaterialFromText(const int8_t* vsContent, const int8_t* fsContent)
	{
		MaterialResource* mr = new MaterialResource();

		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, (const GLchar**)&vsContent, 0);
		glCompileShader(vertexShader);
		if(!validateShader(vertexShader)) return nullptr;

		glShaderSource(fragmentShader, 1, (const GLchar**)&fsContent, 0);
		glCompileShader(fragmentShader);
		if(!validateShader(fragmentShader)) return nullptr;

		mr->programHandle = glCreateProgram();

		glAttachShader(mr->programHandle, vertexShader);
		glAttachShader(mr->programHandle, fragmentShader);

		glBindAttribLocation(mr->programHandle, 0, "inPosition");
		glBindAttribLocation(mr->programHandle, 1, "inNormal");
		glBindAttribLocation(mr->programHandle, 2, "inUVCoords");

		glLinkProgram(mr->programHandle);
		if(!validateProgram(mr->programHandle)) return nullptr;

		return mr;
	}

	bool ResourceLoader::validateShader(uint32_t shaderHandle)
	{
		char buffer[2048];
		memset(buffer, 0, 2048);
		GLsizei len = 0;

		glGetShaderInfoLog(shaderHandle, 2048, &len, buffer);
		if(len > 0)
		{
			Logger::log("vul::ResourceLoader::validateShader: Failed to compile shader - %s", buffer);
			return false;
		}

		return true;
	}

	bool ResourceLoader::validateProgram(uint32_t programHandle)
	{
		char buffer[2048];
		memset(buffer, 0, 2048);
		GLsizei len = 0;

		glGetProgramInfoLog(programHandle, 2048, &len, buffer);
		if(len > 0)
		{
			Logger::log("vul::ResourceLoader::validateProgram: Failed to link program - %s", buffer);
			return false;
		}

		glValidateProgram(programHandle);
		GLint status;
		glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &status);
		if(status == GL_FALSE)
		{
			Logger::log("vul::ResourceLoader::validateProgram: Failed to validate program");
			return false;
		}

		return true;
	}
}