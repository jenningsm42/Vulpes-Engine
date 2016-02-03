#define VULPESENGINE_EXPORT
#include "include/Engine.h"
#include "include/Window.h"
#include "include/InputHandler.h"
#include "Logger.h"
#include <gl/glew.h>
#include <chrono>

namespace vul
{
	Engine::Engine(const WindowCreationParameters& param) : m_deltaTime(1.f), m_deltaTime64(1.0),
		m_fps(0)
	{
		m_window = new Window(param);
		m_inputHandler = new InputHandler(m_window->m_window);

		for(int i = 0; i < 4; i++) m_color[i] = 1.f;

		if(initializeOpenGL())
			Logger::log("Vulpes Engine initialized at %dx%d, OpenGL version %s",
			m_window->getWidth(), m_window->getHeight(), glGetString(GL_VERSION));
	}

	Engine::~Engine()
	{
		delete m_window;
	}

	inline bool Engine::isRunning()
	{
		return m_window->isRunning();
	}

	inline void Engine::close()
	{
		m_window->close();
	}

	inline float Engine::getDeltaTime()
	{
		return m_deltaTime;
	}

	inline double Engine::getDeltaTime64()
	{
		return m_deltaTime64;
	}

	inline uint32_t Engine::getFPS()
	{
		return m_fps;
	}

	Window* Engine::getWindow()
	{
		return m_window;
	}

	InputHandler* Engine::getInputHandler()
	{
		return m_inputHandler;
	}

	inline void Engine::setClearColor(float r, float g, float b, float a)
	{
		m_color[0] = r;
		m_color[1] = g;
		m_color[2] = b;
		m_color[3] = a;
	}

	inline void Engine::swapFrameBuffers()
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

		m_window->swapFrameBuffers();

		glClearColor(m_color[0], m_color[1], m_color[2], m_color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

#if _DEBUG
		GLenum err;
		while(err = glGetError())
			Logger::log("OpenGL Error: %d", err);
#endif // _DEBUG
	}

	bool Engine::initializeOpenGL()
	{
		uint32_t res = glewInit();
		if(res)
		{
			delete m_window;
			Logger::log("vul::Engine::initializeOpenGL: Failed to initialize glew - %s", glewGetErrorString(res));
			return false;
		}

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		return true;
	}
}