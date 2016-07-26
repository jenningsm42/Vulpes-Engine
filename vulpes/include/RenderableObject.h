#ifndef _VUL_RENDERABLEOBJECT_H
#define _VUL_RENDERABLEOBJECT_H
#include "Export.h"
#include "SceneObject.h"
#include "Handle.h"
#include "Mesh.h"
#include "Texture.h"

namespace vul
{
	class VEAPI RenderableObject : public SceneObject
	{
	public:
		RenderableObject(uint32_t id);
		RenderableObject(const RenderableObject&);

		void attachMesh(const Handle<Mesh>&);
		void attachColorMap(const Handle<Texture>&);
		void attachNormalMap(const Handle<Texture>&);
		void attachRoughnessMap(const Handle<Texture>&);
		void setRefractiveIndex(float n);

		Handle<Mesh> getMesh();
		Handle<Texture> getColorMap();
		Handle<Texture> getNormalMap();
		Handle<Texture> getRoughnessMap();
		float getReflectionCoefficient();

	private:
		Mesh m_mesh;
		Texture m_colorMap; // Analogous with albedo map
		Texture m_normalMap;
		Texture m_roughnessMap;
		float m_reflectionCoefficient; // f0 -- for fresnel term (can be mapped)
	};
}

#endif // _VUL_RENDERABLEOBJECT_H