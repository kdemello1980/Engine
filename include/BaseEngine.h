#ifndef BASEENGINE_H
#define BASEENGINE_H



#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace KMDM
{

    class BaseEngine
    {
        public:
            static BaseEngine* getInstance();

//            void createWindow();
//            void createSurface();
//            void createPhysicalDevice();
//            void createInstance();
//            void createLogicalDevice();
//            void createQueue();

//            GLFWwindow* window();
//            GLFWcursor* cursor();
//            VkSurfaceKHR surface();
//            VkPhysicalDevice physicalDevice();
//            VkInstance instance();
//            VkQueue graphicsQueue();
//            VkQueue presentationQueue();

        protected:
            static GLFWwindow* m_window;
            static GLFWcursor* m_cursor;
            static VkSurfaceKHR* m_surface;
            static VkPhysicalDevice* m_physicalDevice;
            static VkInstance* m_vulkanInstance;
            static VkDevice* m_logicalDevice;
            static VkQueue* m_graphicsQueue;
            static VkQueue* m_presentationQueue;

        private:
            BaseEngine();
            virtual ~BaseEngine();

            static BaseEngine* m_baseEngine;
            bool frameBufferResized = false;
    };
}
#endif // BASEENGINE_H
