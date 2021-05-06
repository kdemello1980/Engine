#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#include "Common.h"
#include "DescriptorPool.h"
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

            VkDescriptorPool getWorldPool();
            VkDescriptorPool getPassPool();
            VkDescriptorPool getMaterialPool();
            VkDescriptorPool getMeshPool();

        protected:
            void createWorldPool();
            void createPassPool();
            void createMaterialPool();
            void createMeshPool();
            void createPool(VkDescriptorPool pool, uint32_t binding, VkDescriptorType type, Renderpass renderPass);

        private:
            // Descriptor pools.
            VkDescriptorPool m_worldPool;
            VkDescriptorPool m_passPool;
            VkDescriptorPool m_materialPool;
            VkDescriptorPool m_meshPool;

            // Renderpass.
            Renderpass m_renderPass;

            // Logical device.
            LogicalDevice* m_logicalDevice;


    };
}
#endif