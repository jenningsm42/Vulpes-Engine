#ifndef _VUL_WINDOW_H
#define _VUL_WINDOW_H
#include <string>
#include "Export.h"
#include <cstdint>
#include "WindowCreationParameters.h"
struct GLFWwindow;

namespace vul
{
	class VEAPI Window
	{
	public:
		inline uint32_t getWidth();
		inline uint32_t getHeight();

		void setTitle(const std::string& title);

		void hideCursor();
		void showCursor();

	private:
		uint32_t m_width, m_height;
		bool m_fullscreen;
		bool m_initialized;
		GLFWwindow* m_window;

		Window(const WindowCreationParameters&);
		~Window();

		inline bool isRunning();
		inline void swapFrameBuffers();
		void close();

		static void errorCallback(int error, const char* desc);

		friend class Engine;
	};
}

#endif // _VUL_WINDOW_H