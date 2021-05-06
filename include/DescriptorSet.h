

#include <vulkan/vulkan.h>
#include <vector>

namespace KMDM
{
    class DescriptorSet
    {
        public:
            ~DescriptorSet();
            static DescriptorSet* getInstance();

        protected:

        private:
            DescriptorSet();
            static DescriptorSet* m_DescriptorSet;

            VkDescriptorSetLayout m_layout;
            std::vector<VkDescriptorSetLayoutBinding> m_bindings;
    };
}