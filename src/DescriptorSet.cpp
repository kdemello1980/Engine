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
#include <optional>


namespace KMDM
{
    /**
     * @brief Construct a new Descriptor Set:: Descriptor Set object
     * 
     * @param renderpass 
     */
    DescriptorSet::DescriptorSet(Renderpass* renderpass)
    {
        m_renderPass = renderpass;
        createScenePool();
        createModelPool();

        VkDescriptorSetLayoutBinding sceneLayoutBindings[] = {
            { // Enviornment.
                0,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                1,
                VK_SHADER_STAGE_VERTEX_BIT,
                nullptr
            },
            { // UniformBufferObject.
                1,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                1,
                VK_SHADER_STAGE_VERTEX_BIT,
                nullptr
            },
        };

        VkDescriptorSetLayoutCreateInfo sceneLayoutCreateInfo = {};
        sceneLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        sceneLayoutCreateInfo.bindingCount = 2;
        sceneLayoutCreateInfo.pBindings = sceneLayoutBindings;

        // Create the descriptor set layout.
        if (vkCreateDescriptorSetLayout(m_logicalDevice->getLogicalDevice(), &sceneLayoutCreateInfo, 
            nullptr, &m_sceneLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create scene descriptor set layout.");
        }  
        std::cout << "Created scene descriptor set layout." << std::endl;   

        VkDescriptorSetLayoutBinding modelLayoutBindings[] = {
            {
                0,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                1,
                VK_SHADER_STAGE_VERTEX_BIT,
                nullptr
            },
            {
                1,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                nullptr
            }
        };
        VkDescriptorSetLayoutCreateInfo modelLayoutCreateInfo{};
        modelLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        modelLayoutCreateInfo.bindingCount = 2;
        modelLayoutCreateInfo.pBindings = modelLayoutBindings;

        if (vkCreateDescriptorSetLayout(m_logicalDevice->getLogicalDevice(), &modelLayoutCreateInfo, 
            nullptr, &m_modelLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create model descriptor set layout.");
        }  
        std::cout << "Created model descriptor set layout." << std::endl;  

        // Create the descriptor pools.
        createModelPool();
        createScenePool();
    }
    
    /**
     * @brief Destroy the Descriptor Set:: Descriptor Set object
     * 
     */
    DescriptorSet::~DescriptorSet()
    {
        destoryDescriptorSet();
    }

    /**
     * @brief Destroy the DescriptorSet object.
     * 
     */
    void DescriptorSet::destoryDescriptorSet()
    {
        std::cout << "- Cleaning up scene descriptor pool." << std::endl;
        vkDestroyDescriptorPool(m_logicalDevice->getLogicalDevice(), m_sceneDescriptorPool, nullptr);
        std::cout << "- Cleaning up model descriptor pool." << std::endl;
        vkDestroyDescriptorPool(m_logicalDevice->getLogicalDevice(), m_modelDescriptorPool, nullptr);


        std::cout << "- Cleaning up scene descriptor set layout." << std::endl;
        vkDestroyDescriptorSetLayout(m_logicalDevice->getLogicalDevice(), m_sceneLayout, nullptr); 
        std::cout << "- Cleaning up model descriptor set layout." << std::endl;
        vkDestroyDescriptorSetLayout(m_logicalDevice->getLogicalDevice(), m_modelLayout, nullptr);         
    }

  

    /**
     * @brief Create the descriptor pool for per-frame environment objects.
     * 
     */
    void DescriptorSet::createScenePool()
    {
        std::vector<VkDescriptorPoolSize> sizes;
        sizes.resize(2);

        uint32_t count = static_cast<uint32_t>(SwapChain::getInstance()->getSwapChainImages().size());

        sizes[0].descriptorCount = count;
        sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        sizes[1].descriptorCount = count;
        sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        createDescriptorPool(count, m_renderPass, &m_sceneDescriptorPool, sizes);
    }

    /**
     * @brief Create the per model descriptor pool.
     * 
     */
    void DescriptorSet::createModelPool()
    {
        std::vector<VkDescriptorPoolSize> sizes;
        sizes.resize(2);

        sizes[0].descriptorCount = MAX_DESCRIPTORS / 2;
        sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        sizes[1].descriptorCount = MAX_DESCRIPTORS / 2;
        sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        createDescriptorPool(MAX_DESCRIPTORS, m_renderPass, &m_modelDescriptorPool, sizes);
    }
    
    
    /**
     * @brief Create a descriptor pool
     * 
     * @param renderpass 
     */
    void DescriptorSet::createDescriptorPool(uint32_t numDescriptors, Renderpass* renderpass, 
                VkDescriptorPool* pool, std::vector<VkDescriptorPoolSize> sizes)
    {
        m_logicalDevice = LogicalDevice::getInstance();

        // uint32_t uniformSizes = 2 * (MAX_DESCRIPTORS / 3);
        // uint32_t samplerSizes = MAX_DESCRIPTORS / 3;
        // VkDescriptorPoolSize sizes[] = 
        // {
        //     {
        //         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        //         uniformSizes
        //     },
        //     {
        //         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        //         samplerSizes
        //     }
        // };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = numDescriptors;
        poolInfo.poolSizeCount = sizes.size();
        poolInfo.pPoolSizes = sizes.data();

        if (vkCreateDescriptorPool(m_logicalDevice->getLogicalDevice(), &poolInfo, nullptr,
            pool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool.");
        }
        std::cout << "Created descriptor pool." << std::endl;
    }

    /**
     * @brief Get the Scene Descriptor Sets object
     * 
     * @param count 
     * @param gpuSceneBuffers 
     * @param uniformBuffers 
     * @return std::vector<VkDescriptorSet> 
     */
    std::vector<VkDescriptorSet> DescriptorSet::getSceneDescriptorSets(uint32_t count, std::vector<VkBuffer> gpuSceneBuffers,
        std::vector<VkBuffer> uniformBuffers)
    {
        // Allocate the descriptor sets.
        std::vector<VkDescriptorSet> sets;
        sets.resize(count);

        std::vector<VkDescriptorSetLayout> layouts(count, m_sceneLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_sceneDescriptorPool;
        allocInfo.descriptorSetCount = count;
        allocInfo.pSetLayouts = layouts.data();

        if(vkAllocateDescriptorSets(m_logicalDevice->getLogicalDevice(), &allocInfo, sets.data()) 
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate scene descriptor sets.");
        }

        // Update the descriptor sets. There are 2 uniform buffers. 
        // GPUSceneData and Uniform.
        for (uint32_t i = 0; i < count; i++)
        {
            VkDescriptorBufferInfo sceneBufferInfo{};
            sceneBufferInfo.buffer = gpuSceneBuffers[i];
            sceneBufferInfo.offset = 0;
            sceneBufferInfo.range = sizeof(GPUSceneData);

            VkDescriptorBufferInfo uniformBufferInfo{};
            uniformBufferInfo.buffer = uniformBuffers[i];
            uniformBufferInfo.offset = 0;
            uniformBufferInfo.range = sizeof(UniformBufferObject);

            std::array<VkWriteDescriptorSet, 2> writes{};
            writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[0].dstSet = sets[i];
            writes[0].dstBinding = 0;
            writes[0].descriptorCount = 1;
            writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[0].pBufferInfo = &sceneBufferInfo;

            writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[1].dstSet = sets[i];
            writes[1].dstBinding = 0;
            writes[1].descriptorCount = 1;
            writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[1].pBufferInfo = &sceneBufferInfo;

            vkUpdateDescriptorSets(m_logicalDevice->getLogicalDevice(), 
                static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);        
        }
        return sets;
    }

    /**
     * @brief Get the Model Descriptor Sets object
     * 
     * @param count 
     * @param translationBuffer 
     * @param textureImageView 
     * @param textureSampler 
     * @return std::vector<VkDescriptorSet> 
     */
    std::vector<VkDescriptorSet> DescriptorSet::getModelDescriptorSets(std::vector<Model> models)
    {
        // Allocate the descriptor sets.
        std::vector<VkDescriptorSet> sets;
        sets.resize(models.size());

        std::vector<VkDescriptorSetLayout> layouts(models.size(), m_sceneLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_modelDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(models.size());
        allocInfo.pSetLayouts = layouts.data();

        if(vkAllocateDescriptorSets(m_logicalDevice->getLogicalDevice(), &allocInfo, sets.data()) 
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate scene descriptor sets.");
        }

        // Update the descriptor sets. There are 2 uniform buffers. 
        // GPUSceneData and Uniform.
        for (size_t i = 0; i < models.size(); i++)
        {
            VkBuffer matrix = models[i].getTranslationMatrix();
            VkDescriptorBufferInfo modelBufferInfo{};
            modelBufferInfo.buffer = matrix;
            modelBufferInfo.offset = 0;
            modelBufferInfo.range = sizeof(matrix);

            VkDescriptorImageInfo modelImageInfo{};
            modelImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            modelImageInfo.imageView = models[i].getTextureImageView();
            modelImageInfo.sampler = models[i].getTextureSampler();

            std::array<VkWriteDescriptorSet, 2> writes{};
            writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[0].dstSet = sets[i];
            writes[0].dstBinding = 0;
            writes[0].descriptorCount = 1;
            writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[0].pBufferInfo = &modelBufferInfo;

            writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[1].dstSet = sets[i];
            writes[1].dstBinding = 1;
            writes[1].descriptorCount = 1;
            writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[1].pImageInfo = &modelImageInfo;

            vkUpdateDescriptorSets(m_logicalDevice->getLogicalDevice(), 
                static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);        
        }
        return sets;
    }

    /**
     * @brief Reset the descriptor pool.
     * 
     */
    void DescriptorSet::resetDescriptorPools()
    {
        if (vkResetDescriptorPool(m_logicalDevice->getLogicalDevice(), m_modelDescriptorPool, 0)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to reset model descriptor pool.");
        }
        if (vkResetDescriptorPool(m_logicalDevice->getLogicalDevice(), m_sceneDescriptorPool, 0)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to reset scene descriptor pool.");
        }
    }

    VkDescriptorSetLayout DescriptorSet::getSceneLayout() { return m_sceneLayout; }
    VkDescriptorSetLayout DescriptorSet::getModelLayout() { return m_modelLayout; }
    VkDescriptorPool DescriptorSet::getSceneDescriptorPool() { return m_sceneDescriptorPool; }
    VkDescriptorPool DescriptorSet::getModelDescriptorPool() { return m_modelDescriptorPool; }
}