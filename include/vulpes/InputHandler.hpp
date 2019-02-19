#ifndef _VUL_INPUTHANDLER_HPP
#define _VUL_INPUTHANDLER_HPP

#include <cstdint>

#include "Export.hpp"
#include "Handle.hpp"

struct GLFWwindow;

namespace vul {
    enum struct Key {
        // Printable keys
        Space = 32,
        Apostrophe = 39,  // '
        Comma = 44,  // ,
        Minus = 45,  // -
        Period = 46,  // .
        Slash = 47,  // /
        Num0 = 48,
        Num1 = 49,
        Num2 = 50,
        Num3 = 51,
        Num4 = 52,
        Num5 = 53,
        Num6 = 54,
        Num7 = 55,
        Num8 = 56,
        Num9 = 57,
        Semicolon = 59,  // ;
        Equals = 61,  // =
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,  // [
        Backslash = 92,  /* \ */
        RightBracket = 93,  // ]
        GraveAccent = 96,  // `
        World1 = 161,
        World2 = 162,

        // Function keys
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PagerDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        NumPad0 = 320,
        NumPad1 = 321,
        NumPad2 = 322,
        NumPad3 = 323,
        NumPad4 = 324,
        NumPad5 = 325,
        NumPad6 = 326,
        NumPad7 = 327,
        NumPad8 = 328,
        NumPad9 = 329,
        NumPadDecimal = 330,
        NumPadDivide = 331,
        NumPadMultiply = 332,
        NumPadSubtract = 333,
        NumPadAdd = 334,
        NumPadEnter = 335,
        NumPadEquals = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348,
        Size = 349
    };

    class VEAPI InputHandler {
    public:
        bool getKeyDown(Key key);
        bool getKeyTapped(Key key);

        uint32_t getCursorX();
        uint32_t getCursorY();
        void setCursorPos(uint32_t x, uint32_t y);

        void pollEvents();

    private:
        GLFWwindow* m_window;
        double m_curX;
        double m_curY;

        InputHandler(GLFWwindow*);
        ~InputHandler();

        static void eventCallback(GLFWwindow*, int key, int scancode, int action, int mods);

        friend class Engine;
        friend class Handle<InputHandler>;
    };
}

#endif // _VUL_INPUTHANDLER_HPP
