#include "DescriptorPool.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "Common.h"

#include <vulkan/vulkan.h>
#include <array>
#include <stdexcept>

namespace KMDM
{
    /**
     * Here I am creating 4 descriptors. Based on binding frequency.
     * 
     * 1) Set 0: Engine global resources, bound once per frame.
     * 2) Set 1: Per-pass resources, bound once per pass.
     * 3) Set 2: Material resources, bound per object.
     * 4) Set 3: Per object resources. 
     *
     * Sets 2 & 3 will run in the inner render loop where each object is drawn.
    */
    DescriptorPool::DescriptorPool()
    {
        std::array<VkDescriptorPoolSize, MAX_DESCRIPTOR_SETS> poolSizes;

        // poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // poolSizes[0].descriptorCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());
        // poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // poolSizes[1].descriptorCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = MAX_DESCRIPTOR_SETS;
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
        descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_DESCRIPTOR_SETS);

        if(vkCreateDescriptorPool(LogicalDevice::getInstance()->getLogicalDevice(), &descriptorPoolCreateInfo,
            nullptr, &m_descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create DescriptorPool.");
        }
    }

    /**
     * @brief Destroy the Descriptor Pool:: Descriptor Pool object
     * 
     */
    DescriptorPool::~DescriptorPool()
    {
        vkDestroyDescriptorPool(LogicalDevice::getInstance()->getLogicalDevice(), m_descriptorPool, nullptr);
    }

    
    /**
     * @brief 
     * 
     * @return VkDescriptorPool 
     */
    VkDescriptorPool DescriptorPool::getPool()
    {
        return m_descriptorPool;
    }


    /**
    * @brief Create a Per Frame Set object
    * 
    */
    void DescriptorPool::createPerFrameSet()
    {

    }

    /**
    * @brief Create a Per Pass Set object
    * 
    */
    void DescriptorPool::createPerPassSet()
    {

    }

    /**
    * @brief Create a Per Material Set object
    * 
    */
    void Descriptorpool::createPerMaterialSet()
    {

    }

    /**
    * @brief Create a Per Object Set object
    * 
    */
    void Descriptorpool::createPerObjectSet()
    {
        
    }
}