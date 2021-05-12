#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#include "Common.h"
#include "Renderpass.h"
#include "LogicalDevice.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace KMDM
{
    class DescriptorPool
    {
        public:
            DescriptorPool(Renderpass renderPass);
            virtual ~DescriptorPool();
            VkDescriptorPool getPool();

        protected:

        private:
            // Descriptor pool.
            VkDescriptorPool m_descriptorPool;
            // Renderpass.
            Renderpass m_renderPass;

            // Logical device.
            LogicalDevice* m_logicalDevice;
    };
}
#endif