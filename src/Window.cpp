#include "../include/Window.h"
#include "../include/Common.h"
#include "../include/Renderer.h"
#include "../include/types.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_video.h>

#include <iostream>

namespace KMDM
{
    Window* Window::m_window = nullptr;
    SDL_Window* Window::m_SDLwindow = nullptr;

    Window* Window::getInstance()
    {
        if(!m_window)
        {
            m_window = new Window();
        }
        return m_window;
    }

    SDL_Window* Window::getWindow()
    {
        return m_SDLwindow;
    }

    ExtentSize Window::getWindowSize()
    {
        ExtentSize e;
        SDL_Vulkan_GetDrawableSize(m_SDLwindow, &e.width, &e.height);
        return e;
    }

    void Window::destoryWindow()
    {
        if(m_SDLwindow)
        {
            SDL_DestroyWindow(m_SDLwindow);
            m_SDLwindow = nullptr;
            SDL_Quit();
            std::cout << "- Cleaning up window." << std::endl;
        }
    }

    Window::Window()
    {
        // Initialize the SDL window.
        SDL_Init(SDL_INIT_VIDEO);
        SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

        // Create a blank SDL window.
        m_SDLwindow = SDL_CreateWindow(
            ENGINE,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            windowFlags
        );


        if (!m_SDLwindow)
        {
            throw std::runtime_error("Failed to create SDL window.");
        }
    }

    Window::~Window()
    {
        destoryWindow();
    }
}
