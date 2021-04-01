#include "../include/LogicalDevice.h"
#include "../include/PhysicalDevice.h"
#include "../include/Surface.h"
#include "../include/Common.h"
#include "../include/types.h"

#include <vulkan/vulkan.h>
#include <set>
#include <stdexcept>
#include <iostream>

namespace KMDM
{
    LogicalDevice* LogicalDevice::m_logicalDevice = nullptr;
    VkDevice LogicalDevice::m_VKDevice = VK_NULL_HANDLE;

    LogicalDevice* LogicalDevice::getInstance()
    {
        if (!m_logicalDevice)
        {
            m_logicalDevice = new LogicalDevice();
        }
        return m_logicalDevice;
    }


    VkDevice LogicalDevice::getLogicalDevice()
    {
        return m_logicalDevice->m_VKDevice;
    }

    VkQueue LogicalDevice::getGraphicsQueue()
    {
        return m_logicalDevice->m_graphicsQueue;
    }

    VkQueue LogicalDevice::getPresentationQueue()
    {
        return m_logicalDevice->m_presentationQueue;
    }

    QueueFamilyInfo LogicalDevice::getQueueFamilyInfo()
    {
        return m_logicalDevice->m_queueFamilyInfo;
    }

    LogicalDevice::LogicalDevice()
    {
        // Remember to initialize optional fields before accessing them.
        if (!m_queueFamilyInfo.isComplete())
        {
            findSuitableQueueFamily(QUEUE_FLAGS);
        }
        std::set<uint32_t> queue_indices =
        {
            m_queueFamilyInfo.graphicsFamilyIndex.value(),
            m_queueFamilyInfo.presentationFamilyIndex.value()
        };
        std::vector<VkDeviceQueueCreateInfo> queues;
        float priority = 1.0f;
        uint32_t num_indices = static_cast<uint32_t>(queue_indices.size());
        std::vector<float> qpriorities(num_indices);
        for (size_t i = 0; i < qpriorities.size(); i++)
        {
            qpriorities[i] = priority;
        }

        for (uint32_t q : queue_indices)
        {
            VkDeviceQueueCreateInfo qCreateInfo = {};
            qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qCreateInfo.queueFamilyIndex = q; //need to dereference optional variables.
            qCreateInfo.queueCount = num_indices;
            qCreateInfo.pQueuePriorities = qpriorities.data();
            qCreateInfo.pNext = nullptr;
            queues.push_back(qCreateInfo);
        }

        // For now we will only set the priority for 1 queue.

        // Device features.
        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.geometryShader = VK_TRUE;
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        // VkDeviceCreateInfo
        VkDeviceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = queues.data();
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
        create_info.pEnabledFeatures = &deviceFeatures;
        create_info.enabledExtensionCount = static_cast<uint32_t>(REQUIRED_DEVICE_EXTENSIONS.size());
        create_info.ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data();
        create_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        create_info.pNext = nullptr;

        // Create the device.
        VkPhysicalDevice phys_dev = PhysicalDevice::getInstance()->getPhysicalDevice();
        if (vkCreateDevice(phys_dev, &create_info, nullptr, &m_VKDevice)
                != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to initialize logical device.");
        }
        std::cout << "Created logical device." << std::endl;

        getQueueHandle();
        std::cout << "-> Got queue handles." << std::endl;
    }

    void LogicalDevice::destroyLogicalDevice()
    {
        vkDestroyDevice(m_VKDevice, nullptr);
        m_logicalDevice = nullptr;
    }

    LogicalDevice::~LogicalDevice()
    {
        destroyLogicalDevice();
    }

    /******************************************************************
        Find the suitable queue family.
    *******************************************************************/
    void LogicalDevice::findSuitableQueueFamily(VkQueueFlags QUEUE_FLAGS)
    {
        VkPhysicalDevice device = PhysicalDevice::getInstance()->getPhysicalDevice();
        VkSurfaceKHR surface = Surface::getInstance()->getSurface();

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        if (queue_family_count == 0)
        {
            throw std::runtime_error("Unable to enumerate queue families.");
        }

        std::vector<VkQueueFamilyProperties> families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

        for (uint32_t i = 0; i < static_cast<uint32_t>(families.size()); i++)
        {
            if (families[i].queueFlags & QUEUE_FLAGS)
            {
                m_queueFamilyInfo.graphicsFamilyIndex = i;
                m_queueFamilyInfo.qProperties = families[i];
            }
            // Make sure the queue family supports presentation to a surface.
            VkBool32 presentation = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentation);
            if (presentation)
            {
                m_queueFamilyInfo.presentationFamilyIndex = i;
            }
        }
        // Check the info struct again to make sure we actually found a suitable
        // family.
        if(!m_queueFamilyInfo.isComplete())
        {
            throw std::runtime_error("Could not find a suitable queue.");
        }
    } /// findSuitableQueueFamily


    /******************************************************************
        void getQueueHandle(void): Get a queue handle.
    *******************************************************************/
    void LogicalDevice::getQueueHandle()
    {
        QueueFamilyInfo q = m_queueFamilyInfo;
        vkGetDeviceQueue(m_VKDevice, m_queueFamilyInfo.graphicsFamilyIndex.value(),
            m_queueFamilyInfo.graphicsFamilyIndex.value(), &m_graphicsQueue);
        vkGetDeviceQueue(m_VKDevice, m_queueFamilyInfo.presentationFamilyIndex.value(),
            m_queueFamilyInfo.presentationFamilyIndex.value(), &m_presentationQueue);
    } // getQueueHandle


/******************************************************************************/
    VkShaderModule LogicalDevice::createShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_VKDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module.");
        }
        return shaderModule;
    }
}
