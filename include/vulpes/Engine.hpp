#ifndef _VUL_ENGINE_HPP
#define _VUL_ENGINE_HPP

#include "Export.hpp"
#include "Handle.hpp"
#include "InputHandler.hpp"
#include "Window.hpp"
#include "WindowCreationParameters.hpp"

namespace vul {
    class VEAPI Engine {
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

#endif // _VUL_ENGINE_HPP
