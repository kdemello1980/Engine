#include "DescriptorPool.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "Common.h"

#include <vulkan/vulkan.h>
#include <array>
#include <stdexcept>
#include <iostream>

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
        std::array<VkDescriptorPoolSize, DESCRIPTOR_CATEGORIES> poolSizes;

        // Camera buffer.
        poolSizes[DESCRIPTOR_POOL_ENGINE_RESOURCES].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[DESCRIPTOR_POOL_ENGINE_RESOURCES].descriptorCount = 
            static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        // Per renderpass resources.
        poolSizes[DESCRIPTOR_POOL_PER_PASS_RESOURCES].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[DESCRIPTOR_POOL_PER_PASS_RESOURCES].descriptorCount = 
            static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        // Materials.
        poolSizes[DESCRIPTOR_POOL_MATERIAL_RESOURCES].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[DESCRIPTOR_POOL_MATERIAL_RESOURCES].descriptorCount = 
            static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        // Per-object resources.
        poolSizes[DESCRIPTOR_POOL_OBJECT_RESORUCES].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[DESCRIPTOR_POOL_OBJECT_RESORUCES].descriptorCount = 
            static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        // poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // poolSizes[0].descriptorCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());
        // poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // poolSizes[1].descriptorCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = DESCRIPTOR_CATEGORIES;
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
        descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_DESCRIPTOR_SETS);

        if(vkCreateDescriptorPool(LogicalDevice::getInstance()->getLogicalDevice(), &descriptorPoolCreateInfo,
            nullptr, &m_descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create DescriptorPool.");
        }
        std::cout << "Created descriptor pool." << std::endl;

        createPerFrameSet();
        createPerPassSet();
        createPerMaterialSet();
        createPerObjectSet();
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
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = DESCRIPTOR_POOL_ENGINE_RESOURCES;
        // binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = 1;
        info.pBindings = &binding;

        if (vkCreateDescriptorSetLayout(LogicalDevice::getInstance()->getLogicalDevice(), &info,
            nullptr, &m_perFrameLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create per frame descriptor set layout.");
        }

#ifndef NDEBUG
        std::cout << "Created per frame descriptor set layout." << std::endl;
#endif

        m_perFrameSet.resize(SwapChain::getInstance()->getSwapChainImages().size());
        
        std::vector<VkDescriptorSetLayout> layouts(SwapChain::getInstance()->getSwapChainImages().size(), m_perFrameLayout);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(LogicalDevice::getInstance()->getLogicalDevice(), &allocInfo, m_perFrameSet.data())
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate per frame descriptor sets.");
        }

#ifndef NDEBUG
        std::cout << "Created per frame descriptor set." << std::endl;
#endif
    }
    std::vector<VkDescriptorSet> DescriptorPool::getPerFrameSet()
    {
        return m_perFrameSet;
    }

    /**
    * @brief Create a Per Pass Set object
    * 
    */
    void DescriptorPool::createPerPassSet()
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = DESCRIPTOR_POOL_PER_PASS_RESOURCES;
        // binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = 1;
        info.pBindings = &binding;

        if (vkCreateDescriptorSetLayout(LogicalDevice::getInstance()->getLogicalDevice(), &info,
            nullptr, &m_perPassLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create per frame descriptor set layout.");
        }
#ifndef NDEBUG
        std::cout << "Created per pass descriptor layout." << std::endl;
#endif

        m_perPassSet.resize(SwapChain::getInstance()->getSwapChainImages().size());
        size_t count = SwapChain::getInstance()->getSwapChainImages().size();
        std::vector<VkDescriptorSetLayout> layouts(count, m_perPassLayout);
        
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(LogicalDevice::getInstance()->getLogicalDevice(), &allocInfo, m_perPassSet.data())
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate per pass descriptor sets.");
        }
#ifndef NDEBUG
        std::cout << "Created per pass descriptor set." << std::endl;
#endif
    }
    std::vector<VkDescriptorSet> DescriptorPool::getPerPassSet()
    {
        return m_perPassSet;
    }

    /**
    * @brief Create a Per Material Set object
    * 
    */
    void DescriptorPool::createPerMaterialSet()
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = DESCRIPTOR_POOL_MATERIAL_RESOURCES;
        // binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = 1;
        info.pBindings = &binding;

        if (vkCreateDescriptorSetLayout(LogicalDevice::getInstance()->getLogicalDevice(), &info,
            nullptr, &m_materialLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create per frame descriptor set layout.");
        }
#ifndef NDEBUG
        std::cout << "Created per material descriptor layout." << std::endl;
#endif

        m_materialSet.resize(SwapChain::getInstance()->getSwapChainImages().size());

        std::vector<VkDescriptorSetLayout> layouts(SwapChain::getInstance()->getSwapChainImages().size(), m_materialLayout);
        
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(LogicalDevice::getInstance()->getLogicalDevice(), &allocInfo, m_materialSet.data())
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate per pass descriptor sets.");
        }
#ifndef NDEBUG
        std::cout << "Created per material descriptor set." << std::endl;
#endif
    }
    std::vector<VkDescriptorSet> DescriptorPool::getPerMaterialSet()
    {
        return m_materialSet;
    }

    /**
    * @brief Create a Per Object Set object
    * 
    */
    void DescriptorPool::createPerObjectSet()
    {
        VkDescriptorSetLayoutBinding binding = {};
        // binding.binding = 0;
        binding.binding = DESCRIPTOR_POOL_OBJECT_RESORUCES;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = 1;
        info.pBindings = &binding;

        if (vkCreateDescriptorSetLayout(LogicalDevice::getInstance()->getLogicalDevice(), &info,
            nullptr, &m_objectLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create per frame descriptor set layout.");
        }  
#ifndef NDEBUG
        std::cout << "Created per object descriptor layout." << std::endl;
#endif   

        m_objectSet.resize(SwapChain::getInstance()->getSwapChainImages().size());

        std::vector<VkDescriptorSetLayout> layouts(SwapChain::getInstance()->getSwapChainImages().size(), m_objectLayout);
        
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(LogicalDevice::getInstance()->getLogicalDevice(), &allocInfo, m_objectSet.data())
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate per pass descriptor sets.");
        }
#ifndef NDEBUG
        std::cout << "Created per object descriptor set." << std::endl;
#endif
    }


    std::vector<VkDescriptorSet> DescriptorPool::getPerObjectSet()
    {
        return m_objectSet;
    }

    VkDescriptorSetLayout DescriptorPool::getPerFrameLayout()
    {
        return m_perFrameLayout;
    }

    VkDescriptorSetLayout DescriptorPool::getPerPassLayout()
    {
        return m_perPassLayout;
    }

    VkDescriptorSetLayout DescriptorPool::getPerMaterialLayout()
    {
        return m_materialLayout;
    }
    
    VkDescriptorSetLayout DescriptorPool::getPerObjectLayout()
    {
        return m_objectLayout;
    }
}