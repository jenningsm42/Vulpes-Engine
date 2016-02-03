#ifndef _VUL_SCENE_H
#define _VUL_SCENE_H
#include <vector>
#include "Export.h"
#include <cstdint>
#include "RenderableObject.h"

namespace vul
{
	class VEAPI Scene
	{
	public:
		Scene();
		~Scene();

		void addMaterial(Material*);

		// Returns ID of added renderable
		// Invalidates any current references to a RenderableObject
		/*uint32_t addRenderable(const RenderableObject&);
		void removeRenderable(uint32_t id);
		RenderableObject& getRenderable(uint32_t id);*/

		inline Material* getMaterial(uint32_t index);
		inline uint32_t getMaterialsSize() const;

	private:
		std::vector<Material*> m_materials;
	};
}

#endif // _VUL_SCENE_H