#define VULPESENGINE_EXPORT
#include "include/Renderer.h"

namespace vul
{
	Renderer::Renderer() : m_scene(nullptr), m_camera(nullptr), m_polycount(0), m_error(false)
	{
	}

	void Renderer::setScene(Scene& scene)
	{
		m_scene = &scene;
	}

	void Renderer::setCamera(Camera& camera)
	{
		m_camera = &camera;
	}

	uint32_t Renderer::getPolygonCount()
	{
		return m_polycount;
	}
}