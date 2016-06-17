#ifndef _VUL_RENDERER_H
#define _VUL_RENDERER_H
#include <cstdint>
#include "Export.h"
#include "Scene.h"
#include "Camera.h"

namespace vul
{
	class VEAPI Renderer
	{
	public:
		Renderer();
		virtual ~Renderer() {}

		virtual void setScene(Scene&);
		virtual void setActiveCamera(Camera&);

		virtual void render() = 0;

		virtual void setWireframeMode(bool) = 0;
		uint32_t getPolygonCount();

	protected:
		Scene* m_scene;
		Camera* m_camera;
		uint32_t m_polycount;
		bool m_error; // So that error messages aren't logged for every attempted frame
	};
}

#endif // _VUL_RENDERER_H