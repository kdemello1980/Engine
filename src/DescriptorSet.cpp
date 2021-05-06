#include "DescriptorSet.h"

#include <vulkan/vulkan.h>


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

    }
}