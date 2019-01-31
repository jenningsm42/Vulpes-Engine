#define VULPESENGINE_EXPORT
#include "include/ResourceLoader.h"
#include "include/RenderTarget.h"
#include "include/CustomRenderer.h"
#include "Logger.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstring>
#include <fstream>

namespace vul
{
	ResourceLoader::ResourceLoader()
	{
		createPlane();
		createSphere();
		createQuad();
		createIBLLUT();
	}

	ResourceLoader::~ResourceLoader()
	{
	}

	Handle<Mesh> ResourceLoader::loadMeshFromFile(const std::string& path)
	{

		if(m_resourceCache.hasResource(path))
			return m_resourceCache.getMesh(path);

		std::vector<uint8_t> fileData = readFile(path);
		if(fileData.size() == 0)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Returned empty '%s'", path.c_str());
			return Handle<Mesh>();
		}

		MeshData meshData;
		if(!m_parserVEM.parse(&meshData, fileData.data(), fileData.size()))
		{
			Logger::log("vul::ResourceLoader::loadMeshFromFile: Unable to load '%s'", path.c_str());
			return Handle<Mesh>();
		}

		Handle<Mesh> mesh = loadMeshFromData(meshData);

		if(mesh.isLoaded())
			m_resourceCache.addMesh(path, mesh);

		return mesh;
	}

	Handle<Mesh> ResourceLoader::loadMeshFromData(const MeshData& meshData)
	{
		if(!meshData.vertices)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromData: No data in vertices");
			return Handle<Mesh>();
		}

		if(!meshData.indices)
		{
			Logger::log("vul::ResourceLoader::loadMeshFromData: No data in indices");
			return Handle<Mesh>();
		}

		Handle<Mesh> mesh;
		mesh->ic = meshData.indexCount;
		uint32_t vbo[5];

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

		// Read file
		std::vector<uint8_t> fileData = readFile(path);
		if(fileData.size() == 0)
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Returned empty '%s'", path);
			return Handle<Texture>();
		}

		// Parse data
		const uint8_t* buffer = reinterpret_cast<const uint8_t*>(fileData.data());
		if(!m_imageParser.parse(buffer, fileData.size()))
		{
			Logger::log("vul::ResourceLoader::loadTextureFromFile: Unable to load '%s'", path);
			return Handle<Texture>();
		}

		// Query for image information
		ImageInfo info = m_imageParser.getImageInfo();

		Handle<Texture> texture;
		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_2D, texture->textureHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.numMipMaps > 1? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, info.numMipMaps);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.f);

		// Upload all mipmaps
		uint32_t width = info.width;
		uint32_t height = info.height;
		for(uint32_t i = 0; i < info.numMipMaps; i++)
		{
			uint32_t size = m_imageParser.getSize(i);
			if(info.s3tc)
			{
				uint8_t* data = new uint8_t[size];
				m_imageParser.getIntImage(data, i);
				glCompressedTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, width, height, 0, size, data);
				delete[] data;
			}
			else
			{
				// Since only DDS and HDR are supported, uncompressed images are assumed to be floating point textures
				float* data = new float[size];
				m_imageParser.getFloatImage(data, i);
				glTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, width, height, 0, info.format, info.channelType, data);
				delete[] data;
			}
			width /= 2;
			height /= 2;
		}

		texture.setLoaded();

		if(texture.isLoaded())
			m_resourceCache.addTexture(path, texture);

		return texture;
	}

	Handle<Texture> ResourceLoader::loadTextureFromColor(float red, float green, float blue)
	{
		char path[25];
		sprintf(path, "__vul_r%1.3fg%1.3fb%1.3f", red, green, blue);

		if(m_resourceCache.hasResource(path))
			m_resourceCache.getTexture(path);

		float* data = new float[3];
		data[0] = red;
		data[1] = green;
		data[2] = blue;

		Handle<Texture> texture;
		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_2D, texture->textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, data);

		delete[] data;

		m_resourceCache.addTexture(path, texture);

		texture.setLoaded();
		return texture;
	}

	Handle<Texture> ResourceLoader::loadCubeMap(const std::string& frontPath, const std::string & backPath, const std::string & topPath, const std::string & bottomPath, const std::string & leftPath, const std::string & rightPath, bool prefilter)
	{
		std::string resourcePath = frontPath + backPath + topPath + bottomPath + leftPath + rightPath;
		if(m_resourceCache.hasResource(resourcePath))
			return m_resourceCache.getTexture(resourcePath);

		Handle<Texture> texture;
		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		uint32_t width = 0;
		bool result = loadCubeMapSide(rightPath, texture, 0, &width);
		result &= loadCubeMapSide(leftPath, texture, 1);
		result &= loadCubeMapSide(topPath, texture, 2);
		result &= loadCubeMapSide(bottomPath, texture, 3);
		result &= loadCubeMapSide(frontPath, texture, 4);
		result &= loadCubeMapSide(backPath, texture, 5);

		if(!result)
		{
			glDeleteTextures(1, &texture->textureHandle);
			return texture; // Error message in loadCubeMapSide
		}

		if(prefilter) prefilterCubeMap(texture, width);

		m_resourceCache.addTexture(resourcePath, texture);

		texture.setLoaded();
		return texture;
	}

	Handle<Texture> ResourceLoader::loadCubeMapCross(const std::string& path, bool prefilter)
	{
		if(m_resourceCache.hasResource(path))
			return m_resourceCache.getTexture(path);

		// Read file
		std::vector<uint8_t> fileData = readFile(path);
		if(fileData.size() == 0)
		{
			Logger::log("vul::ResourceLoader::loadCubeMapCross: Returned empty '%s'", path.c_str());
			return Handle<Texture>();
		}

		// Parse data
		const uint8_t* buffer = reinterpret_cast<const uint8_t*>(fileData.data());
		if(!m_imageParser.parse(buffer, fileData.size()))
		{
			Logger::log("vul::ResourceLoader::loadCubeMapCross: Unable to load '%s'", path.c_str());
			return Handle<Texture>();
		}

		// Query for image information
		ImageInfo info = m_imageParser.getImageInfo();

		// Cannot splice compressed images
		if(info.s3tc)
		{
			Logger::log("vul::ResourceLoader::loadCubeMapCross: Cannot splice compressed images '%s'", path.c_str());
			return Handle<Texture>();
		}

		// Ensure aspect ratio is correct
		bool vertical = false;
		if(info.width / 3 == info.height / 4) vertical = true;
		else if(info.width / 4 == info.height / 3) vertical = false; // Horizontal
		else
		{
			Logger::log("vul::ResourceLoader::loadCubeMapCross: Incorrect aspect ratio '%s'", path.c_str());
			return Handle<Texture>();
		}

		Handle<Texture> texture;
		glGenTextures(1, &texture->textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureHandle);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Upload all mipmaps
		uint32_t width = info.width;
		uint32_t height = info.height;
		for(uint32_t i = 0; i < info.numMipMaps; i++)
		{
			uint32_t sideWidth = vertical? width / 3 : width / 4;

			// TODO: Only HDR/float textures supported for cubemap cross right now, update for future formats
			float* data = new float[m_imageParser.getSize(i)];
			float* sideData = new float[sideWidth * sideWidth * info.numChannels];
			m_imageParser.getFloatImage(data, i); // TODO: check rtn

			for(uint32_t k = 0; k < 6; k++)
			{
				uint32_t yStart, yEnd, xStart, xEnd;

				// Right, left, top, bottom, and front coordinates are equivalent regardless of orientation
				switch(k)
				{
				case 0: // Right
				{
					yStart = sideWidth;
					yEnd = 2 * sideWidth;
					xStart = 2 * sideWidth;
					xEnd = 3 * sideWidth;
				} break;
				case 1: // Left
				{
					yStart = sideWidth;
					yEnd = 2 * sideWidth;
					xStart = 0;
					xEnd = sideWidth;
				} break;
				case 2: // Top
				{
					yStart = 0;
					yEnd = sideWidth;
					xStart = sideWidth;
					xEnd = 2 * sideWidth;
				} break;
				case 3: // Bottom
				{
					yStart = 2 * sideWidth;
					yEnd = 3 * sideWidth;
					xStart = sideWidth;
					xEnd = 2 * sideWidth;
				} break;
				case 4: // Front
				{
					yStart = sideWidth;
					yEnd = 2 * sideWidth;
					xStart = sideWidth;
					xEnd = 2 * sideWidth;
				} break;
				case 5: // Back
				{
					// TODO: turns out flipped
					if(vertical)
					{
						yStart = 4 * sideWidth - 1;
						yEnd = 3 * sideWidth - 1;
						xStart = 2 * sideWidth - 1;
						xEnd = sideWidth - 1;
					}
					else
					{
						yStart = sideWidth;
						yEnd = 2 * sideWidth;
						xStart = 3 * sideWidth;
						xEnd = 4 * sideWidth;
					}
				} break;
				}

				uint32_t index = 0;
				for(uint32_t y = yStart; (k == 5 && vertical)? y > yEnd : y < yEnd; (k == 5 && vertical)? y-- : y++)
				{
					for(uint32_t x = xStart; (k == 5 && vertical)? x > xEnd : x < xEnd; (k == 5 && vertical)? x-- : x++)
					{
						for(uint32_t j = 0; j < info.numChannels; j++)
							sideData[index++] = data[info.numChannels * (x + y * width) + j];
					}
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, i, info.internalFormat, sideWidth, sideWidth, 0, info.format, info.channelType, sideData);
			}
			
			delete[] sideData;
			delete[] data;

			width >>= 1;
			height >>= 1;
		}
		if(info.numMipMaps == 1) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		if(prefilter) prefilterCubeMap(texture, vertical ? info.width / 3 : info.width / 4);

		texture.setLoaded();

		if(texture.isLoaded())
			m_resourceCache.addTexture(path, texture);

		return texture;
	}

	Handle<Shader> ResourceLoader::loadShaderFromFile(const std::string& vsPath, const std::string& fsPath)
	{
		if(m_resourceCache.hasResource(vsPath + fsPath))
			m_resourceCache.getShader(vsPath + fsPath);

		std::vector<uint8_t> vsData = readFile(vsPath);
		if(vsData.size() == 0)
		{
			Logger::log("vul::ResourceLoader::loadShaderFromFile: '%s' returned empty", vsPath.c_str());
			return Handle<Shader>();
		}

		std::vector<uint8_t> fsData = readFile(fsPath);
		if(fsData.size() == 0)
		{
			Logger::log("vul::ResourceLoader::loadShaderFromFile: '%s' returned empty", fsPath.c_str());
			return Handle<Shader>();
		}

		Handle<Shader> shader = loadShaderFromText(vsData.data(), fsData.data());

		if(shader.isLoaded())
			m_resourceCache.addShader(vsPath + fsPath, shader);

		return shader;
	}

	Handle<Shader> ResourceLoader::loadShaderFromText(const uint8_t* vsContent, const uint8_t* fsContent)
	{
		Handle<Shader> shader;

		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, reinterpret_cast<const GLchar**>(&vsContent), 0);
		glCompileShader(vertexShader);
		if(!validateShader(vertexShader)) return shader;

		glShaderSource(fragmentShader, 1, reinterpret_cast<const GLchar**>(&fsContent), 0);
		glCompileShader(fragmentShader);
		if(!validateShader(fragmentShader)) return shader;

		shader->programHandle = glCreateProgram();

		glAttachShader(shader->programHandle, vertexShader);
		glAttachShader(shader->programHandle, fragmentShader);

		glLinkProgram(shader->programHandle);
		if(!validateProgram(shader->programHandle)) return shader;

		shader.setLoaded();
		return shader;
	}

	Handle<Mesh> ResourceLoader::getPlane()
	{
		return m_resourceCache.getMesh("__vul_plane");
	}

	Handle<Mesh> ResourceLoader::getSphere()
	{
		return m_resourceCache.getMesh("__vul_sphere");
	}

	Handle<Mesh> ResourceLoader::getQuad()
	{
		return m_resourceCache.getMesh("__vul_quad");
	}

	Handle<ResourceCache> ResourceLoader::getResourceCache()
	{
		return Handle<ResourceCache>(m_resourceCache);
	}

	bool ResourceLoader::validateShader(uint32_t shaderHandle)
	{
		char buffer[2048];
		memset(buffer, 0, 2048);
		GLsizei len = 0;

		glGetShaderInfoLog(shaderHandle, 2048, &len, buffer);
		if(len > 0)
		{
			Logger::log("vul::ResourceLoader::validateShader: Failed to compile shader:\n%s", buffer);
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
			Logger::log("vul::ResourceLoader::validateProgram: Failed to link program:\n%s", buffer);
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
		meshData.UVCoordinates[1] = 1.f;

		meshData.UVCoordinates[2] = 1.f;
		meshData.UVCoordinates[3] = 1.f;

		meshData.UVCoordinates[4] = 0.f;
		meshData.UVCoordinates[5] = 0.f;

		meshData.UVCoordinates[6] = 1.f;
		meshData.UVCoordinates[7] = 0.f;

		for(uint32_t i = 0; i < meshData.vertexCount * 3; i++)
		{
			meshData.normals[i] = ((i + 2) % 3 == 0) ? 1.f : 0.f; // <0.f, 1.f, 0.f>
			meshData.tangents[i] = (i % 3 == 0) ? 1.f : 0.f; // <1.f, 0.f, 0.f>
			meshData.bitangents[i] = ((i + 1) % 3 == 0)? -1.f : 0.f; // <0.f, 0.f, -1.f>
		}

		Handle<Mesh> plane = loadMeshFromData(meshData);
		if(plane.isLoaded())
			m_resourceCache.addMesh("__vul_plane", plane);
	}

	void ResourceLoader::createSphere()
	{
		const uint32_t resolution = 64;

		MeshData meshData(resolution * (resolution + 1), ((resolution - 1) * resolution * 2) * 3, true, true, true);

		uint32_t iter = 0;
		for(uint32_t i = 0; i < resolution; i++)
		{
			float vAngle = 3.14159f * (float)i / float(resolution - 1); // [0, pi]
			float y = cosf(vAngle);

			for(uint32_t j = 0; j < resolution + 1; j++)
			{
				// Check is to ensure there is no seam due to floating point inaccuracy
				float hAngle = (j == resolution)? 0.f : 3.14159f * (float)j / ((float)resolution / 2.f); // [0, 2pi]
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
				meshData.tangents[iter * 3] = cosf(hAngle - 3.14159f / 2.f) * sinf(vAngle);
				meshData.tangents[iter * 3 + 1] = cosf(vAngle);
				meshData.tangents[iter * 3 + 2] = sinf(hAngle - 3.14159f / 2.f) * sinf(vAngle);

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

				// UV coordinates are a plain grid
				meshData.UVCoordinates[iter * 2] = 1.f - (float)j / (float)resolution;
				meshData.UVCoordinates[iter * 2 + 1] = (float)i / float(resolution - 1);

				iter++;
			}
		}

		iter = 0;
		for(uint32_t i = 0; i < resolution - 1; i++)
		{
			for(uint32_t j = 0; j < resolution; j++)
			{
				meshData.indices[iter++] = i * (resolution + 1) + j;
				meshData.indices[iter++] = i * (resolution + 1) + j + 1;
				meshData.indices[iter++] = (i + 1) * (resolution + 1) + j;

				meshData.indices[iter++] = (i + 1) * (resolution + 1) + j;
				meshData.indices[iter++] = i * (resolution + 1) + j + 1;
				meshData.indices[iter++] = (i + 1) * (resolution + 1) + j + 1;
			}
		}

		Handle<Mesh> sphere = loadMeshFromData(meshData);
		if(sphere.isLoaded())
			m_resourceCache.addMesh("__vul_sphere", sphere);
	}

	void ResourceLoader::createQuad()
	{
		MeshData meshData(4, 6, true, true, true);

		meshData.vertices[0] = -1.f;
		meshData.vertices[1] = -1.f;
		meshData.vertices[2] = 0.f;

		meshData.vertices[3] = 1.f;
		meshData.vertices[4] = -1.f;
		meshData.vertices[5] = 0.f;

		meshData.vertices[6] = -1.f;
		meshData.vertices[7] = 1.f;
		meshData.vertices[8] = 0.f;

		meshData.vertices[9] = 1.f;
		meshData.vertices[10] = 1.f;
		meshData.vertices[11] = 0.f;

		meshData.indices[0] = 0;
		meshData.indices[1] = 1;
		meshData.indices[2] = 2;

		meshData.indices[3] = 2;
		meshData.indices[4] = 1;
		meshData.indices[5] = 3;

		meshData.UVCoordinates[0] = 0.f;
		meshData.UVCoordinates[1] = 0.f;

		meshData.UVCoordinates[2] = 1.f;
		meshData.UVCoordinates[3] = 0.f;

		meshData.UVCoordinates[4] = 0.f;
		meshData.UVCoordinates[5] = 1.f;

		meshData.UVCoordinates[6] = 1.f;
		meshData.UVCoordinates[7] = 1.f;

		// These are not expected to be used, but included for sake of completeness
		for(uint32_t i = 0; i < meshData.vertexCount * 3; i++)
		{
			meshData.normals[i] = ((i + 1) % 3 == 0) ? 1.f : 0.f; // <0.f, 0.f, 1.f>
			meshData.tangents[i] = (i % 3 == 0) ? 1.f : 0.f; // <1.f, 0.f, 0.f>
			meshData.bitangents[i] = ((i + 2) % 3 == 0)? 1.f : 0.f; // <0.f, 1.f, 0.f>
		}

		Handle<Mesh> quad = loadMeshFromData(meshData);
		if(quad.isLoaded())
			m_resourceCache.addMesh("__vul_quad", quad);
	}

	void ResourceLoader::createIBLLUT()
	{
		RenderTarget envLUT(512, 512);
		CustomRenderer cr(*this);
        auto shader = loadShaderFromFile("data/envlut.vs", "data/envlut.fs");
		cr.setShader(shader);
		cr.render(&envLUT);
		envLUT.cacheTexture("__vul_IBLLUT", m_resourceCache);
	}

	std::vector<uint8_t> ResourceLoader::readFile(const std::string& path)
	{
		// I chose to represent this data with uint8_t as
		// most file formats are expected to be binary
		std::ifstream f(path, std::ios::binary);
		if(!f)
		{
			Logger::log("vul::ResourceLoader::readFile: Unable to open file '%s'", path.c_str());
			return std::vector<uint8_t>();
		}

		// Read in chunks to reduce number of reallocations
		// and ensure the correct amount of data is read.
		// Seeking to the end of the file then using ftell()
		// is not guaranteed to be correct in binary mode
		// (and doesn't count certain characters in text mode)
		std::vector<uint8_t> data;
		const uint32_t chunkSize = 4096; // Pagefiles are generally 4K
		uint8_t buffer[chunkSize];

		while(!f.eof())
		{
			f.read(reinterpret_cast<char*>(buffer), chunkSize);

			std::streamsize bytesRead = f.gcount();
			if(bytesRead < chunkSize)
			{
				data.insert(data.end(), buffer, buffer + bytesRead);
				break;
			}

			data.insert(data.end(), buffer, buffer + chunkSize);
		}
		f.close();

		data.push_back(0); // Add terminating character in case used as C-string

		return data;
	}

	bool ResourceLoader::loadCubeMapSide(const std::string& path, Handle<Texture> texture, uint32_t side, uint32_t* ptrWidth)
	{
		// Read file
		std::vector<uint8_t> fileData = readFile(path);
		if(fileData.size() == 0)
		{
			Logger::log("vul::ResourceLoader::loadCubeMapSide: Returned empty '%s'", path.c_str());
			return false;
		}

		// Parse data
		const uint8_t* buffer = reinterpret_cast<const uint8_t*>(fileData.data());
		if(!m_imageParser.parse(buffer, fileData.size()))
		{
			Logger::log("vul::ResourceLoader::loadCubeMapSide: Unable to load '%s'", path.c_str());
			return false;
		}

		// Query for image information
		ImageInfo info = m_imageParser.getImageInfo();

		// Pass width if requested
		if(ptrWidth) *ptrWidth = info.width;

		if(info.numMipMaps > 1)
		{
			// Assumes number of mipmaps is same for all sides
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, info.numMipMaps - 1);
		}

		// Upload all mipmaps
		uint32_t width = info.width;
		uint32_t height = info.height;
		for(uint32_t i = 0; i < info.numMipMaps; i++)
		{
			uint32_t size = m_imageParser.getSize(i);
			if(info.s3tc)
			{
				uint8_t* data = new uint8_t[size];
				m_imageParser.getIntImage(data, i);
				glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, i, info.internalFormat, width, height, 0, size, data);
				delete[] data;
			}
			else
			{
				// Since only DDS and HDR are supported, uncompressed images are assumed to be floating point textures
				float* data = new float[size];
				m_imageParser.getFloatImage(data, i);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, i, info.internalFormat, width, height, 0, info.format, info.channelType, data);
				delete[] data;
			}
			width /= 2;
			height /= 2;
		}
		texture.setLoaded();

		return true;
	}

	void ResourceLoader::prefilterCubeMap(Handle<Texture>& texture, uint32_t width)
	{
		// Prefilter is for fast real time IBL
		CustomRenderer cr(*this);
        auto shader = loadShaderFromFile("data/prefilter.vs", "data/prefilter.fs");
		cr.setShader(shader);
		cr.setCubeMap(texture, 0);

		uint32_t numMipMaps = std::log2(width);

		// Mipmaps with lower resolution will get prefiltered with higher roughnesses
		for(uint32_t i = 1; i <= numMipMaps; i++)
		{
			width >>= 1;
			RenderTarget prefilter(width, width, 0);
			for(int j = 0; j < 6; j++)
				prefilter.addTarget(texture, 0x8515 + j, i);
			float roughness = (float)i / (float)numMipMaps;
			cr.copyUniformBufferObject(&roughness, sizeof(float));
			cr.render(&prefilter);
		}
	}
}
