#ifndef _VUL_RENDERER_H
#define _VUL_RENDERER_H
#include "Export.h"
#include <cstdint>
#include "Material.h"

namespace vul
{
	class Scene;
	class Camera;

	class VEAPI Renderer
	{
	public:
		Renderer();
		~Renderer();

		void setScene(Scene*);
		void setActiveCamera(Camera*);

		void render();

		void setWireframeMode(bool);
		uint32_t getPolygonCount();

	private:
		Scene* m_scene;
		Camera* m_camera;
		uint32_t m_polycount;

		void handleUniforms(Material*);
	};
}

#endif // _VUL_RENDERER_H