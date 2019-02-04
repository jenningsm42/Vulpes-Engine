#ifndef _VUL_SCENE_HPP
#define _VUL_SCENE_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "Export.hpp"
#include "Handle.hpp"
#include "PointLight.hpp"
#include "RenderableObject.hpp"

namespace vul
{
	class VEAPI Scene
	{
	public:
		Scene();
		~Scene();

		uint32_t createSceneObject(SceneObjectType);

		Handle<RenderableObject> getRenderableObject(uint32_t id);
		Handle<RenderableObject> getRenderableObjectByIndex(uint32_t index);

		Handle<PointLight> getPointLight(uint32_t id);
		Handle<PointLight> getPointLightByIndex(uint32_t index);

		uint32_t getSceneObjectCount(SceneObjectType);
		void removeSceneObject(SceneObjectType, uint32_t id);

	private:
		std::vector<RenderableObject> m_renderableObjects;
		std::vector<PointLight> m_pointLights;
		std::map<uint32_t, uint32_t> m_sceneIDs; // ID -> index
		uint32_t m_nextID;
	};
}

#endif // _VUL_DEFERREDSCENE_HPP
