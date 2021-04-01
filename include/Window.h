#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <SDL_vulkan.h>

#include "types.h"

namespace KMDM
{
    class Window
    {
        public:
            SDL_Window* getWindow();
            static Window* getInstance();

            virtual ~Window();
            void destoryWindow();
            ExtentSize getWindowSize();

        protected:

        private:
            Window();
            static SDL_Window* m_SDLwindow;
            static Window* m_window;
    };
}
#endif // WINDOW_H
