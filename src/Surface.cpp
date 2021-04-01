#include "../include/Surface.h"
#include "../include/Instance.h"
#include "../include/Window.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>
#include <SDL.h>
#include <SDL_vulkan.h>

namespace KMDM
{
    Surface* Surface::m_surface = nullptr;
//    VkSurfaceKHR Surface::m_VKsurface = VK_NULL_HANDLE;

    Surface* Surface::getInstance()
    {
        if(!m_surface)
        {
            m_surface = new Surface();
        }
        return m_surface;
    }

    VkSurfaceKHR Surface::getSurface()
    {
        return m_surface->m_VKsurface;
    }

    Surface::Surface()
    {
        if (SDL_Vulkan_CreateSurface(Window::getInstance()->getWindow(),
            Instance::getInstance()->getVulkanInstance(), &m_VKsurface) != SDL_TRUE)
        {
            throw std::runtime_error("Failed to create window surface.");
        }
        std::cout << "Created surface." << std::endl;
    }

    Surface::~Surface()
    {
        destroySurface();
    }

    void Surface::destroySurface()
    {
        vkDestroySurfaceKHR(Instance::getInstance()->getVulkanInstance(), m_VKsurface, nullptr);
        m_surface = nullptr;
    }
}
