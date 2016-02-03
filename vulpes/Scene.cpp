#define VULPESENGINE_EXPORT
#include "include/Scene.h"
#include "include/Material.h"

namespace vul
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::addMaterial(Material* m)
	{
		bool found = false;
		for(int32_t i = 0; i < m_materials.size(); i++)
		{
			if(m_materials[i] == m)
				return;
		}
		
		m_materials.push_back(m);
	}

	/*uint32_t Scene::addRenderable(const RenderableObject& renderable)
	{
		// If the material isn't in the scene, then add it
		bool found = false;
		for(int32_t i = 0; i < m_materials.size(); i++)
		{
			if(m_materials[i] == renderable.getMaterial())
				found = true;
		}
		
		if(!found)
			m_materials.push_back(renderable.getMaterial());

		return renderable.getID();
	}

	void Scene::removeRenderable(uint32_t id)
	{
		for(int32_t i = 0; i < m_materials.size(); i++)
		{
			if(m_materials[i]->containsRenderable(id))
			{
				m_materials[i]->removeRenderable(id);
				return;
			}
		}
	}
	
	RenderableObject& Scene::getRenderable(uint32_t id)
	{
		for(int32_t i = 0; i < m_materials.size(); i++)
		{
			if(m_materials[i]->containsRenderable(id))
				return m_materials[i]->getRenderableFromId(id);
		}
	}*/

	inline Material* Scene::getMaterial(uint32_t index)
	{
		return m_materials[index];
	}

	inline uint32_t Scene::getMaterialsSize() const
	{
		return m_materials.size();
	}
}