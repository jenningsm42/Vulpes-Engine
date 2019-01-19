#ifndef _VUL_ENGINE_H
#define _VUL_ENGINE_H
#include "Export.h"
#include "WindowCreationParameters.h"
#include "Handle.h"
#include "Window.h"
#include "InputHandler.h"

namespace vul
{
	class VEAPI Engine
	{
	public:
		Engine(const WindowCreationParameters&);
		~Engine();

		bool isRunning();
		void close();

		float getDeltaTime();
		double getDeltaTime64();
		uint32_t getFPS();

		void swapFrameBuffers();

		Handle<Window> getWindow();
		Handle<InputHandler> getInputHandler();

	private:
		Window m_window;
		InputHandler m_inputHandler;
		double m_deltaTime64;
		float m_deltaTime;
		uint32_t m_fps;

		bool initializeOpenGL();
	};
}

#endif // _VUL_ENGINE_H
