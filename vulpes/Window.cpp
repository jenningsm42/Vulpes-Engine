#define VULPESENGINE_EXPORT
#include "include/Window.h"
#include "Logger.h"
#include <gl/glew.h>
#include <glfw/glfw3.h>

namespace vul
{
	Window::Window(const WindowCreationParameters& param)
		: m_width(param.width), m_height(param.height), m_fullscreen(param.fullscreen), m_initialized(true)
	{
		glfwSetErrorCallback(Window::errorCallback);

		if(!glfwInit())
		{
			Logger::log("vul::Window::Window: Failed to initialize glfw");
			m_initialized = false;
		}

		glfwWindowHint(GLFW_RESIZABLE, param.resizable);

		m_window = glfwCreateWindow(m_width, m_height, param.title.c_str(), param.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
		if(!m_window)
		{
			glfwTerminate();
			Logger::log("vul::Window::Window: Failed to create glfw window");
			m_initialized = false;
		}
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(param.vsync ? 1 : 0);

		// Determine which monitor the window is currently on,
		// then center it appropriately
		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		if(!monitors) return;

		GLFWmonitor *owner = nullptr;
		int windowX, windowY;
		glfwGetWindowPos(m_window, &windowX, &windowY);
		int ownerX, ownerY, ownerWidth, ownerHeight;

		for(int i = 0; i < monitorCount; i++) {
			int monitorX, monitorY;
			glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);

			int monitorWidth, monitorHeight;
			GLFWvidmode *monitorVidmode = (GLFWvidmode*)glfwGetVideoMode(monitors[i]);

			if(monitorVidmode == nullptr)  continue;
			else {
				monitorWidth = monitorVidmode->width;
				monitorHeight = monitorVidmode->height;
			}

			// This is the owner if the center of the window is within the monitor's bounds
			if((windowX > monitorX && windowX < (monitorX + monitorWidth)) && (windowY > monitorY && windowY < (monitorY + monitorHeight))) {
				owner = monitors[i];

				ownerX = monitorX;
				ownerY = monitorY;

				ownerWidth = monitorWidth;
				ownerHeight = monitorHeight;
			}
		}

		if(owner != nullptr) {
			glfwSetWindowPos(m_window, ownerX + (ownerWidth - m_width) / 2, ownerY + (ownerHeight - m_height) / 2);
		}
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	bool Window::isRunning()
	{
		return m_initialized ? !glfwWindowShouldClose(m_window) : false;
	}

	void Window::swapFrameBuffers()
	{
		if(m_initialized)
			glfwSwapBuffers(m_window);
	}

	uint32_t Window::getWidth()
	{
		return m_width;
	}

	uint32_t Window::getHeight()
	{
		return m_height;
	}

	void Window::setTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_window, title.c_str());
	}

	void Window::hideCursor()
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	void Window::showCursor()
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void Window::close()
	{
		glfwSetWindowShouldClose(m_window, true);
	}

	void Window::errorCallback(int error, const char* desc)
	{
		Logger::log("vul::Window::errorCallback: %s (%d)", desc, error);
	}
}