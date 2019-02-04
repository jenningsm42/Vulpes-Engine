#ifndef _VUL_WINDOWCREATIONPARAMETERS_HPP
#define _VUL_WINDOWCREATIONPARAMETERS_HPP

#include <cstdint>
#include <string>

namespace vul
{
	struct WindowCreationParameters
	{
		uint32_t width = 800;
		uint32_t height = 600;
		bool fullscreen = false;
		bool vsync = false;
		bool resizable = false;
		uint8_t samples = 0;
		std::string title = "Vulpes Engine";
	};
}

#endif // _VUL_WINDOWCREATIONPARAMETERS_HPP
