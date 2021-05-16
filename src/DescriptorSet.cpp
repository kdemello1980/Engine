#include "DescriptorSet.h"
#include "LogicalDevice.h"
#include "Common.h"
#include "SwapChain.h"
#include "Scene.h"
#include "SwapChain.h"

#include <vulkan/vulkan.h>
#include <array>
#include <vector>
#include <stdexcept>
#include <iostream>


namespace KMDM
{
    /**
     * @brief Construct a new Descriptor Set:: Descriptor Set object
     * 
     * @param renderpass 
     */
    DescriptorSet::DescriptorSet(Renderpass* renderpass)
    {
        createDescriptorPool(renderpass);

        VkDescriptorSetLayoutBinding layoutBindings[] = {
            { // Enviornment.
                0,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                nullptr
            },
            { // Vetex.
                1,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                1,
                VK_SHADER_STAGE_VERTEX_BIT,
                nullptr
            },
            { // Texture.
                2,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                nullptr
            }
        };

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = 3;
        layoutCreateInfo.pBindings = layoutBindings;

        // Create the descriptor set layout.
        if (vkCreateDescriptorSetLayout(m_logicalDevice->getLogicalDevice(), &layoutCreateInfo, 
            nullptr, &m_layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor set layout.");
        }        
    }
    
    DescriptorSet::~DescriptorSet()
    {
        destoryDescriptorSet();
    }

    void DescriptorSet::destoryDescriptorSet()
    {
        std::cout << "- Cleaning up descriptor pool." << std::endl;
        vkDestroyDescriptorPool(m_logicalDevice->getLogicalDevice(), m_descriptorPool, nullptr);

        std::cout << "- Cleaning up descriptor set layout." << std::endl;
        vkDestroyDescriptorSetLayout(m_logicalDevice->getLogicalDevice(), m_layout, nullptr);     
    }

    /**
     * @brief 
     * 
     * @return std::vector<VkDescriptorSet> 
     */
    std::vector<VkDescriptorSet> DescriptorSet::getDescriptorSets()
    {
        size_t size = Scene::getInstance()->getMeshes().size();
        size *= SwapChain::getInstance()->getSwapChainImages().size();

        std::vector<VkDescriptorSet> sets;
        sets.resize(size);

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
        allocInfo.pSetLayouts = &m_layout;

        if (vkAllocateDescriptorSets(m_logicalDevice->getLogicalDevice(), &allocInfo, sets.data())
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate descriptor sets.");
        }
        
        return sets;
    }
    
    
    /**
     * @brief 
     * 
     * @param renderpass 
     */
    void DescriptorSet::createDescriptorPool(Renderpass* renderpass)
    {
        m_renderPass = renderpass;
        m_logicalDevice = LogicalDevice::getInstance();

        uint32_t uniformSizes = 2 * (MAX_DESCRIPTORS / 3);
        uint32_t samplerSizes = MAX_DESCRIPTORS / 3;
        VkDescriptorPoolSize sizes[] = 
        {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                uniformSizes
            },
            {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                samplerSizes
            }
        };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = MAX_DESCRIPTORS;
        poolInfo.poolSizeCount = 2;
        poolInfo.pPoolSizes = sizes;

        if (vkCreateDescriptorPool(m_logicalDevice->getLogicalDevice(), &poolInfo, nullptr,
            &m_descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool.");
        }
    }

    void DescriptorSet::resetDescriptorPool()
    {
        if (vkResetDescriptorPool(m_logicalDevice->getLogicalDevice(), m_descriptorPool, 0)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to reset descriptor pool.");
        }
    }

    VkDescriptorSetLayout DescriptorSet::getLayout() { return m_layout; }
    VkDescriptorPool DescriptorSet::getDescriptorPool() { return m_descriptorPool; }
}