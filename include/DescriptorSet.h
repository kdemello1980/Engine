
#include "LogicalDevice.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace KMDM
{
    class DescriptorSet
    {
        public:
            ~DescriptorSet();
            static DescriptorSet* getInstance();

            VkDescriptorSetLayout getLayout();

        protected:

        private:
            DescriptorSet();
            static DescriptorSet* m_DescriptorSet;

            LogicalDevice* m_logicalDevice;

            VkDescriptorSetLayout m_layout;
            
    };
}