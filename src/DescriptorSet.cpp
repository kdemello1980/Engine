#include "DescriptorSet.h"
#include "LogicalDevice.h"
#include "Common.h"

#include <vulkan/vulkan.h>
#include <array>
#include <stdexcept>


namespace KMDM
{
    DescriptorSet* DescriptorSet::m_DescriptorSet = nullptr;

    DescriptorSet* DescriptorSet::getInstance()
    {
        if (!m_DescriptorSet)
        {
            m_DescriptorSet = new DescriptorSet();
        }
        return m_DescriptorSet;
    }
    
    DescriptorSet::DescriptorSet()
    {
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

    VkDescriptorSetLayout DescriptorSet::getLayout() { return m_layout; }
}