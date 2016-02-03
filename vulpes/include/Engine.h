#ifndef _VUL_ENGINE_H
#define _VUL_ENGINE_H
#include "Export.h"
#include "WindowCreationParameters.h"

namespace vul
{
	class Window;
	class InputHandler;

	class VEAPI Engine
	{
	public:
		Engine(const WindowCreationParameters&);
		~Engine();

		inline bool isRunning();
		inline void close();

		inline float getDeltaTime();
		inline double getDeltaTime64();
		inline uint32_t getFPS();

		inline void setClearColor(float r, float g, float b, float a = 1.f);
		inline void swapFrameBuffers();

		Window* getWindow();
		InputHandler* getInputHandler();

	private:
		Window* m_window;
		InputHandler* m_inputHandler;
		double m_deltaTime64;
		float m_deltaTime;
		uint32_t m_fps;
		float m_color[4];

		bool initializeOpenGL();
	};
}

#endif // _VUL_ENGINE_H