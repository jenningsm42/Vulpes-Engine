#define VULPESENGINE_EXPORT

#include <cstdint>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vulpes/InputHandler.hpp>

namespace vul
{
	static bool keys[(uint32_t)Key::Size];
	static bool prevKeys[(uint32_t)Key::Size];

	InputHandler::InputHandler(GLFWwindow* window) : m_window(window)
	{
		glfwSetKeyCallback(window, InputHandler::eventCallback);

		for(int i = 0; i < (uint32_t)Key::Size; i++)
			keys[i] = prevKeys[i] = false;
	}

	InputHandler::~InputHandler()
	{

	}

	void InputHandler::pollEvents()
	{
		glfwPollEvents();
	}

	bool InputHandler::getKeyDown(Key key)
	{
		return keys[(uint32_t)key];
	}

	bool InputHandler::getKeyTapped(Key key)
	{
		if(keys[(uint32_t)key] && !prevKeys[(uint32_t)key])
		{
			prevKeys[(uint32_t)key] = true;
			return true;
		}
		return false;
	}

	uint32_t InputHandler::getCursorX()
	{
		glfwGetCursorPos(m_window, &m_curX, &m_curY);
		return static_cast<uint32_t>(m_curX);
	}

	uint32_t InputHandler::getCursorY()
	{
		glfwGetCursorPos(m_window, &m_curX, &m_curY);
		return static_cast<uint32_t>(m_curY);
	}

	void InputHandler::setCursorPos(uint32_t x, uint32_t y)
	{
		glfwSetCursorPos(m_window, x, y);
		m_curX = x;
		m_curY = y;
	}

	void InputHandler::eventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if(key == -1) return; // Occurs when pressing ALT + PrntScrn
		if(action == GLFW_PRESS)
			keys[key] = true;
		else if(action == GLFW_RELEASE)
		{
			keys[key] = false;
			prevKeys[key] = false;
		}
	}
}
