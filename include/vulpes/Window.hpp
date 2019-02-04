#ifndef _VUL_WINDOW_HPP
#define _VUL_WINDOW_HPP

#include <cstdint>
#include <string>

#include "Export.hpp"
#include "Handle.hpp"
#include "WindowCreationParameters.hpp"

struct GLFWwindow;

namespace vul
{
	class VEAPI Window
	{
	public:
		uint32_t getWidth();
		uint32_t getHeight();

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

		bool isRunning();
		void swapFrameBuffers();
		void close();

		static void errorCallback(int error, const char* desc);

		friend class Engine;
		friend class Handle<Window>;
	};
}

#endif // _VUL_WINDOW_HPP
