#include "../include/PhysicalDevice.h"
#include "../include/Common.h"
#include "../include/Instance.h"
#include "../include/Surface.h"
#include "../include/types.h"

#include <vector>
#include <stdexcept>
#include <iostream>

#include <vulkan/vulkan.h>

namespace KMDM
{
    PhysicalDevice* PhysicalDevice::m_physicalDevice = nullptr;
    VkPhysicalDevice PhysicalDevice::m_VKphysicalDevice = VK_NULL_HANDLE;

    PhysicalDevice* PhysicalDevice::getInstance()
    {
        if(!m_physicalDevice)
        {
            m_physicalDevice = new PhysicalDevice();
        }
        return m_physicalDevice;
    }

    VkPhysicalDevice PhysicalDevice::getPhysicalDevice()
    {
        return m_physicalDevice->m_VKphysicalDevice;
    }


    PhysicalDevice::PhysicalDevice()
    {
        // Get an instance.
        Instance* instance = Instance::getInstance();

        // Get the physical device count.
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance->getVulkanInstance(), &device_count, nullptr);
        if(device_count == 0)
        {
            throw std::runtime_error("Could not find any devices that support Vulkan.");
        }

        // Enumerate the physical devices.
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(instance->getVulkanInstance(), &device_count, devices.data());

        // Check device capabilities & choose a suitable device.
        // This chooses the first suitable device, not necessarily the best one.
        for (size_t i = 0; i < devices.size(); i++)
        {
            if(isDeviceSuitable(devices[i]))
            {
                m_VKphysicalDevice = devices[i];
                break;
            }
        }
        if (m_VKphysicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Failed to find a suitable GPU.");
        }
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(m_VKphysicalDevice, &props);
        vkGetPhysicalDeviceMemoryProperties(m_VKphysicalDevice, &m_physicalDeviceMemoryProperties);
//        std::cout << "Found physical device: " << props.deviceName << std::endl;
    }

    PhysicalDevice::~PhysicalDevice()
    {
        //dtor
    }

    SwapChainSupportDetails PhysicalDevice::getSwapChainSupportDetails()
    {
        return m_physicalDevice->m_SwapChainDetails;
    }

    /******************************************************************
        Verify device is suitable.
    *******************************************************************/
    bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);
        std::cout << "Found " << device_properties.deviceName << std::endl;

        // Check the device extension properties
        uint32_t device_extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count,
                                             nullptr);
        if(device_extension_count == 0)
        {
            throw std::runtime_error("Failed to enumerate device extensions.");
        }

        std::vector<VkExtensionProperties> device_extension_properties(device_extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, device_extension_properties.data());

        // Verify the desired extensions are supported.
        bool extension_supported = true;
        Instance* instance = Instance::getInstance();
        for (auto & extension : REQUIRED_DEVICE_EXTENSIONS)
        {
            if (!instance->verifyExtensionSupport(device_extension_properties, extension))
            {
                extension_supported = false;
            }
        }

        // Verify the swapchain is adequate.
        bool swapchain_adequate = true;
        if (extension_supported)
        {
            m_SwapChainDetails = querySwapChainSupport(device);
            swapchain_adequate = !m_SwapChainDetails.formats.empty() &&
                                 !m_SwapChainDetails.presentModes.empty();
        }

        bool is_gpu = false;
        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                device_features.geometryShader && device_features.samplerAnisotropy)
        {
            is_gpu = true;
        }

        return (is_gpu && extension_supported && swapchain_adequate);
    } /// isDeviceSuitable


    /******************************************************************
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice):
            Query the details of swap chain support.
    *******************************************************************/
    SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice device)
    {
        VkSurfaceKHR surface = Surface::getInstance()->getSurface();
        SwapChainSupportDetails details;
        // Get surface capabilities.
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                &details.capabilities) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to get surface capabilities.");
        }
        // Get supported surface formats.
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                                 details.formats.data());
        }
        // Get the supported presentation modes.
        uint32_t mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count, nullptr);
        if (mode_count != 0)
        {
            details.presentModes.resize(mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count,
                    details.presentModes.data());
        }
        return details;
    } /// querySwapChainSupport

    VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_VKphysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("Failed to find suitable format.");
    }

    VkPhysicalDeviceMemoryProperties PhysicalDevice::getMemoryProperties() { return m_physicalDeviceMemoryProperties; }
}
