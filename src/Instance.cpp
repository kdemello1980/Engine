#include "../include/Instance.h"
#include "../include/Window.h"
#include "../include/Common.h"

#include <stdexcept>
#include <iostream>
#include <string.h>

#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>


namespace KMDM
{
    VkInstance Instance::m_vkInstance = VK_NULL_HANDLE;
    Instance* Instance::m_instance = nullptr;

    Instance* Instance::getInstance()
    {
        if (!m_instance)
        {
            m_instance = new Instance();
        }
        return m_instance;
    }


    Instance::Instance()
    {
        // Check validation layers.
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available.");
        }
        if (enableValidationLayers && checkValidationLayerSupport())
            std::cout << "Validation layers supported." << std::endl;

        // Application info
        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = ENGINE;
        app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
        app_info.apiVersion = VK_API_VERSION_1_2;
        app_info.pEngineName = ENGINE;
        app_info.engineVersion = VK_MAKE_VERSION(1,0,0);
        app_info.pNext = nullptr;

        // Create info.
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.pNext = nullptr;

        // SDL extensions.
        uint32_t SDLextensionCount;
        SDL_Vulkan_GetInstanceExtensions(Window::getInstance()->getWindow(), &SDLextensionCount, nullptr);
        const char* SDLextensions[SDLextensionCount];
        if (SDL_Vulkan_GetInstanceExtensions(Window::getInstance()->getWindow(), &SDLextensionCount, SDLextensions)
            != SDL_TRUE)
        {
            throw std::runtime_error("Failed to determine SDL extensions.");
        }

        create_info.enabledExtensionCount = SDLextensionCount;
        create_info.ppEnabledExtensionNames = SDLextensions;

        // Validation layers.
        create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        // Verify extension support.
        uint32_t instance_extension_count = 0;
        std::vector<VkExtensionProperties> available_extensions;
        if (vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not enumerate supported instance extensions.");
        }
        available_extensions.resize(instance_extension_count);
        if (vkEnumerateInstanceExtensionProperties(nullptr,&instance_extension_count,available_extensions.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not enumerate supported instance extensions.");
        }
        for (uint32_t i = 0; i < SDLextensionCount; i++)
        {
            if (!verifyExtensionSupport(available_extensions, SDLextensions[i]))
                throw std::runtime_error("Extension not found.");
        }
        /**
            Create the instance.
        */
        if(vkCreateInstance(&create_info, nullptr, &m_vkInstance) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create instance.");
        }
        std::cout << "Created instance." << std::endl;
    }

    void Instance::destoryInstance()
    {
        vkDestroyInstance(m_vkInstance, nullptr);
        m_instance = nullptr;
    }

    Instance::~Instance()
    {
        Instance::destoryInstance();
    }

    /**
        Get the vk instance.
    */
    VkInstance Instance::getVulkanInstance()
    {
        return m_instance->m_vkInstance;
    }

    /******************************************************************
        Check validation layer support.
    *******************************************************************/
    bool Instance::checkValidationLayerSupport()
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count,nullptr);
        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char *layerName : VALIDATION_LAYERS)
        {
            bool layer_found = false;
            for (const auto & layer_properties : available_layers)
            {
                if (strcmp(layerName, layer_properties.layerName) == 0)
                {
                    layer_found = true;
                    break;
                }
            }
            if (!layer_found)
            {
                return false;
            }
        }
        return true;
    } /// checkValidationLayerSupport

    /******************************************************************
        Verify that a given extension name is supported by the underlying
        device or instance.
    *******************************************************************/
    bool Instance::verifyExtensionSupport(std::vector<VkExtensionProperties> const & available_extensions,
                                           char const * desired_extension)
    {
        for (auto & available_extension : available_extensions)
        {
            if( strstr( available_extension.extensionName, desired_extension ) )
            {
                return true;
            }
        }
        return false;
    } ///verifyExtensionSupport
}
