#ifndef _VUL_RENDERABLEOBJECT_H
#define _VUL_RENDERABLEOBJECT_H
#include <string>
#include "Export.h"
#include "SceneObject.h"
#include "RenderableObjectParameters.h"
#include "MaterialUniformValues.h"

namespace vul
{
	class Material;

	class VEAPI RenderableObject : public SceneObject
	{
	public:
		RenderableObject(const RenderableObjectParameters&);
		RenderableObject(const RenderableObject&);

		const MeshResource& getMeshResource() const;
		const TextureResource& getTextureResource() const;
		Material* getMaterial();

		const MaterialUniform& getUniform(uint32_t index) const;
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

	private:
		// The following shouldn't be pointers (except m_material) to be contiguous in memory
		MeshResource m_meshResource;
		TextureResource m_textureResource;
		Material* m_material;
		MaterialUniform m_materialUniforms[8]; // Object-specific uniform values
	};
}

#endif // _VUL_RENDERABLEOBJECT_H