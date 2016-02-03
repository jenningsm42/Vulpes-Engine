#ifndef _VUL_MATERIAL_H
#define _VUL_MATERIAL_H
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "Export.h"
#include <cstdint>
#include "MaterialUniformValues.h"
#include "MaterialResource.h"
#include "RenderableObject.h"

namespace vul
{
	class VEAPI Material
	{
	public:
		Material(MaterialResource*);
		~Material();

		MaterialResource* getMaterialResource();

		MaterialUniform* getUniform(uint32_t index);
		inline uint32_t getSize() const;

		void setFloat(const std::string& name, float value);
		void setFloat2(const std::string& name, float value[2]);
		void setFloat3(const std::string& name, float value[3]);
		void setFloat4(const std::string& name, float value[4]);

		void setFloat2x2(const std::string& name, float value[2][2]);
		void setFloat3x3(const std::string& name, float value[3][3]);
		void setFloat4x4(const std::string& name, float value[4][4]);

		void setInt(const std::string& name, int32_t value);
		void setInt2(const std::string& name, int32_t value[2]);
		void setInt3(const std::string& name, int32_t value[3]);
		void setInt4(const std::string& name, int32_t value[4]);

		void addRenderable(RenderableObject&);
		bool containsRenderable(uint32_t id);
		void removeRenderable(uint32_t id);
		RenderableObject* getRenderableFromIndex(uint32_t index);
		RenderableObject* getRenderable(uint32_t id);
		inline uint32_t getRenderablesSize() const;

	private:
		MaterialResource m_materialResource;
		std::vector<RenderableObject> m_renderableObjects;

		const static uint32_t m_maxUniformSize = 8;
		uint32_t m_currentUniformSize;
		MaterialUniform m_materialUniforms[m_maxUniformSize]; // Material-wide uniform values


		friend class RenderableFactory;
		friend class Scene;
	};
}

#endif // _VUL_MATERIAL_H