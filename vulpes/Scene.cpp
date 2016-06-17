#define VULPESENGINE_EXPORT
#include "include/Scene.h"
#include <algorithm>

namespace vul
{
	Scene::Scene() : m_nextID(0)
	{
	}

	Scene::~Scene()
	{
	}

	uint32_t Scene::createSceneObject(SceneObjectType type)
	{
		switch(type)
		{
		case SceneObjectType::Renderable:
		{
			m_renderableObjects.push_back(RenderableObject(m_nextID));
			m_sceneIDs[m_nextID] = m_renderableObjects.size() - 1;
		} break;
		case SceneObjectType::PointLight:
		{
			m_pointLights.push_back(PointLight(m_nextID));
			m_sceneIDs[m_nextID] = m_pointLights.size() - 1;
		} break;
		default: return 0;
		}
		
		return m_nextID++;
	}

	Handle<RenderableObject> Scene::getRenderableObject(uint32_t id)
	{
		return Handle<RenderableObject>(m_renderableObjects[m_sceneIDs[id]]);
	}

	Handle<RenderableObject> Scene::getRenderableObjectByIndex(uint32_t index)
	{
		return Handle<RenderableObject>(m_renderableObjects[index]);
	}

	Handle<PointLight> Scene::getPointLight(uint32_t id)
	{
		return Handle<PointLight>(m_pointLights[m_sceneIDs[id]]);
	}

	Handle<PointLight> Scene::getPointLightByIndex(uint32_t index)
	{
		return Handle<PointLight>(m_pointLights[index]);
	}

	uint32_t Scene::getSceneObjectCount(SceneObjectType type)
	{
		switch(type)
		{
		case SceneObjectType::Renderable: return m_renderableObjects.size();
		case SceneObjectType::PointLight: return m_pointLights.size();
		default: return 0;
		}
	}

	void Scene::removeSceneObject(SceneObjectType type, uint32_t id)
	{
		uint32_t index = m_sceneIDs[id];
		uint32_t modifiedID;

		switch(type)
		{
		case SceneObjectType::Renderable:
		{
			modifiedID = m_renderableObjects.back().getID();
			std::swap(m_renderableObjects[index], m_renderableObjects.back());
			m_renderableObjects.pop_back();
		} break;
		case SceneObjectType::PointLight:
		{
			modifiedID = m_pointLights.back().getID();
			std::swap(m_pointLights[index], m_pointLights.back());
			m_pointLights.pop_back();
		} break;
		}
		
		m_sceneIDs[modifiedID] = index;
		m_sceneIDs.erase(m_sceneIDs.find(id));
	}
}