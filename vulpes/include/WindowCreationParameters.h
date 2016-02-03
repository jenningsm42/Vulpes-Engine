#ifndef _VUL_WINDOWCREATIONPARAMETERS_H
#define _VUL_WINDOWCREATIONPARAMETERS_H
#include <string>
#include <cstdint>

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

#endif // _VUL_WINDOWCREATIONPARAMETERS_H