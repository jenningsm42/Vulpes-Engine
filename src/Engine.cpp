#define VULPESENGINE_EXPORT
#include "include/Engine.h"
#include "include/Window.h"
#include "include/InputHandler.h"
#include "Logger.h"
#include <GL/glew.h>
#include <chrono>

namespace vul
{
	Engine::Engine(const WindowCreationParameters& param) : m_deltaTime(1.f), m_deltaTime64(1.0),
		m_fps(0), m_window(param), m_inputHandler(m_window.m_window)
	{
		if(initializeOpenGL())
			Logger::log("Vulpes Engine initialized at %dx%d, OpenGL version %s",
			m_window.getWidth(), m_window.getHeight(), glGetString(GL_VERSION));
	}

	Engine::~Engine()
	{
	}

	bool Engine::isRunning()
	{
		return m_window.isRunning();
	}

	void Engine::close()
	{
		m_window.close();
	}

	float Engine::getDeltaTime()
	{
		return m_deltaTime;
	}

	double Engine::getDeltaTime64()
	{
		return m_deltaTime64;
	}

	uint32_t Engine::getFPS()
	{
		return m_fps;
	}

	Handle<Window> Engine::getWindow()
	{
		return Handle<Window>(m_window);
	}

	Handle<InputHandler> Engine::getInputHandler()
	{
		return Handle<InputHandler>(m_inputHandler);
	}

	void Engine::swapFrameBuffers()
	{
		// Track delta time and FPS
		static auto curtime = std::chrono::system_clock::now(),
			prevtime = std::chrono::system_clock::now(),
			sprevtime = std::chrono::system_clock::now();
		static uint32_t frames = 0;
		++frames;
		curtime = std::chrono::system_clock::now();
		m_deltaTime64 = (double)std::chrono::duration_cast<std::chrono::microseconds>(curtime - prevtime).count() / 1000000.0;
		m_deltaTime = (float)m_deltaTime64;
		prevtime = curtime;
		if(std::chrono::duration_cast<std::chrono::seconds>(curtime - sprevtime).count() >= 1)
		{
			m_fps = frames;
			frames = 0;
			sprevtime = curtime;
		}

		m_window.swapFrameBuffers();

#if _DEBUG
		GLenum err;
		while(err = glGetError())
			Logger::log("OpenGL Error: 0x%x", err);
#endif // _DEBUG
	}

	bool Engine::initializeOpenGL()
	{
		uint32_t res = glewInit();
		if(res)
		{
			Logger::log("vul::Engine::initializeOpenGL: Failed to initialize glew - %s", glewGetErrorString(res));
			return false;
		}

		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.f);
		glDepthFunc(GL_LESS);
		
		return true;
	}
}
