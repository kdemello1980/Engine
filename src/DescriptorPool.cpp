#include "DescriptorPool.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "Common.h"
#include "Renderpass.h"

#include <vulkan/vulkan.h>
#include <array>
#include <stdexcept>
#include <iostream>

namespace KMDM
{
    DescriptorPool::DescriptorPool(Renderpass renderPass)
    {
        m_renderPass = renderPass;
        m_logicalDevice = LogicalDevice::getInstance();

        VkDescriptorPoolSize sizes[] = 
        {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                2 * (MAX_DESCRIPTORS / 3)
            },
            {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                MAX_DESCRIPTORS / 3
            }
        };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = MAX_DESCRIPTORS;
        poolInfo.poolSizeCount = MAX_DESCRIPTORS;
        poolInfo.pPoolSizes = sizes;

        if (vkCreateDescriptorPool(m_logicalDevice->getLogicalDevice(), &poolInfo, nullptr,
            &m_descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool.");
        }

    }
    DescriptorPool::~DescriptorPool()
    {

    }

    // Getters.
    VkDescriptorPool DescriptorPool::getPool() { return m_descriptorPool; }
}