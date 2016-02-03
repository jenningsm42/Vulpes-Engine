#define VULPESENGINE_EXPORT
#include "include/Material.h"
#include "Logger.h"

namespace vul
{
	Material::Material(MaterialResource* materialResource) : m_materialResource(*materialResource),
		m_currentUniformSize(0)
	{
	}

	Material::~Material()
	{
	}

	MaterialResource* Material::getMaterialResource()
	{
		return &m_materialResource;
	}

	MaterialUniform* Material::getUniform(uint32_t index)
	{
		return &m_materialUniforms[index];
	}

	inline uint32_t Material::getSize() const
	{
		return m_currentUniformSize;
	}

	void Material::setFloat(const std::string& name, float value)
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				m_materialUniforms[i].value.float1 = value;
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		uniform.value.float1 = value;
		uniform.type = MaterialUniformType::Float1;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setFloat2(const std::string& name, float value[2])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat2: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.float2, value, sizeof(float) * 2);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.float2, value, sizeof(float) * 2);
		uniform.type = MaterialUniformType::Float2;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setFloat3(const std::string& name, float value[3])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat3: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.float3, value, sizeof(float) * 3);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.float3, value, sizeof(float) * 3);
		uniform.type = MaterialUniformType::Float3;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setFloat4(const std::string& name, float value[4])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat4: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.float4, value, sizeof(float) * 4);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.float4, value, sizeof(float) * 4);
		uniform.type = MaterialUniformType::Float4;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setFloat2x2(const std::string& name, float value[2][2])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat2x2: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.float2x2, value, sizeof(float) * 2 * 2);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.float2x2, value, sizeof(float) * 2 * 2);
		uniform.type = MaterialUniformType::Float2x2;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setFloat3x3(const std::string& name, float value[3][3])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat3x3: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.float3x3, value, sizeof(float) * 3 * 3);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.float3x3, value, sizeof(float) * 3 * 3);
		uniform.type = MaterialUniformType::Float3x3;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setFloat4x4(const std::string& name, float value[4][4])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setFloat4x4: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.float4x4, value, sizeof(float) * 4 * 4);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.float4x4, value, sizeof(float) * 4 * 4);
		uniform.type = MaterialUniformType::Float4x4;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setInt(const std::string& name, int32_t value)
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setInt: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				m_materialUniforms[i].value.int1 = value;
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		uniform.value.int1 = value;
		uniform.type = MaterialUniformType::Int1;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setInt2(const std::string& name, int32_t value[2])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setInt2: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.int2, value, sizeof(int32_t) * 2);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.int2, value, sizeof(int32_t) * 2);
		uniform.type = MaterialUniformType::Int2;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setInt3(const std::string& name, int32_t value[3])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setInt3: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.int3, value, sizeof(int32_t) * 3);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.int3, value, sizeof(int32_t) * 3);
		uniform.type = MaterialUniformType::Int3;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::setInt4(const std::string& name, int32_t value[4])
	{
		if(m_currentUniformSize >= m_maxUniformSize)
		{
			Logger::log("vul::Material::setInt4: Reached maximum number of uniform slots already");
			return;
		}

		for(int32_t i = 0; i < m_currentUniformSize; i++)
		{
			if(m_materialUniforms[i].name == name)
			{
				memcpy(m_materialUniforms[i].value.int4, value, sizeof(int32_t) * 4);
				return;
			}
		}

		MaterialUniform uniform;
		strcpy(uniform.name, name.c_str());
		memcpy(uniform.value.int4, value, sizeof(int32_t) * 4);
		uniform.type = MaterialUniformType::Int4;
		m_materialUniforms[m_currentUniformSize++] = uniform;
	}

	void Material::addRenderable(RenderableObject& r)
	{
		m_renderableObjects.push_back(r);
	}

	bool Material::containsRenderable(uint32_t id)
	{
		for(int32_t i = 0; i < m_renderableObjects.size(); i++)
		{
			if(m_renderableObjects[i].getID() == id)
				return true;
		}
		return false;
	}

	void Material::removeRenderable(uint32_t id)
	{
		for(int32_t i = 0; i < m_renderableObjects.size(); i++)
		{
			if(m_renderableObjects[i].getID() == id)
			{
				std::swap(m_renderableObjects[i], m_renderableObjects.back());
				m_renderableObjects.pop_back();
				return;
			}
		}
	}

	RenderableObject* Material::getRenderableFromIndex(uint32_t index)
	{
		return &m_renderableObjects[index];
	}

	RenderableObject* Material::getRenderable(uint32_t id)
	{
		for(int32_t i = 0; i < m_renderableObjects.size(); i++)
		{
			if(m_renderableObjects[i].getID() == id)
				return &m_renderableObjects[i];
		}
	}

	inline uint32_t Material::getRenderablesSize() const
	{
		return m_renderableObjects.size();
	}
}