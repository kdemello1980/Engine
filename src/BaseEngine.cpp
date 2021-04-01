#include "BaseEngine.h"
#include "Common.h"
#include "Window.h"
#include <vector>

#include <vulkan/vulkan.h>

//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>

namespace KMDM
{
//    GLFWwindow* BaseEngine::m_window = nullptr;
//    GLFWcursor* BaseEngine::m_cursor = nullptr;

    BaseEngine* BaseEngine::getInstance()
    {
        if (!m_baseEngine)
        {
            m_baseEngine = new BaseEngine();
        }
        return m_baseEngine;
    }

    BaseEngine::BaseEngine()
    {
//        createWindow();
        m_window = Window::getWindow();
//        createSurface();
//        createPhysicalDevice();
//        createInstance();
//        createLogicalDevice();
//        createQueue();
//        createQueue();
    }

    BaseEngine::~BaseEngine()
    {
        //dtor
    }

}
