
#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H

#include "LogicalDevice.h"
#include "DescriptorPool.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace KMDM
{
    class DescriptorSet
    {
        public:
            DescriptorSet(Renderpass *renderpass);
            ~DescriptorSet();

            VkDescriptorSetLayout getLayout();
            std::vector<VkDescriptorSet> getDescriptorSets();
            VkDescriptorPool getDescriptorPool();
            void resetDescriptorPool(); // Do this after every frame.

        protected:
            void createDescriptorPool(Renderpass *renderpass);

        private:

            LogicalDevice* m_logicalDevice;
            VkDescriptorSetLayout m_layout;
            Renderpass* m_renderPass;    


            // Descriptor pool.
            VkDescriptorPool m_descriptorPool;        
    };
}
#endif