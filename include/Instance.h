#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>

#include <vulkan/vulkan.h>

namespace KMDM
{
    class Instance
    {
        public:
            static Instance* getInstance();
            virtual ~Instance();
            void destoryInstance();


            VkInstance getVulkanInstance();
            bool verifyExtensionSupport(std::vector<VkExtensionProperties> const & available_extensions,
                                           char const * desired_extension);
        protected:
            bool checkValidationLayerSupport();

        private:
            Instance();

            //
            static Instance* m_instance;
            static VkInstance m_vkInstance;
    };
}
#endif // INSTANCE_H
