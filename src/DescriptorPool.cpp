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
        createWorldPool();
        createPassPool();
        createMaterialPool();
        createMeshPool();
    }
    DescriptorPool::~DescriptorPool()
    {

    }

    // Getters.
    VkDescriptorPool DescriptorPool::getWorldPool() { return m_worldPool; }
    VkDescriptorPool DescriptorPool::getPassPool() { return m_passPool; }
    VkDescriptorPool DescriptorPool::getMaterialPool() { return m_materialPool; }
    VkDescriptorPool DescriptorPool::getMeshPool() { return m_meshPool; }

    void DescriptorPool::createWorldPool()
    {
        createPool(m_worldPool, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_renderPass);
    }

    void DescriptorPool::createPassPool()
    {
        createPool(m_passPool, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_renderPass);
    }

    void DescriptorPool::createMaterialPool()
    {
        createPool(m_materialPool, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_renderPass);
    }

    void DescriptorPool::createMeshPool()
    {
        createPool(m_meshPool, 3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_renderPass);
    }

    void DescriptorPool::createPool(VkDescriptorPool pool, uint32_t binding, VkDescriptorType type, Renderpass renderPass)
    {
        // uint32_t maxSets = m_logicalDevice->getPhysicalDeviceProperties().maxPerSetDescriptors;
    }
}