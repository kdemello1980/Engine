#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <vulkan/vulkan.h>
#include <vector>

#include "types.h"

namespace KMDM
{
    class PhysicalDevice
    {
        public:
            static PhysicalDevice* getInstance();
            VkPhysicalDevice getPhysicalDevice();
            SwapChainSupportDetails getSwapChainSupportDetails();

            VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                VkImageTiling tiling, VkFormatFeatureFlags features);
            
            VkPhysicalDeviceMemoryProperties getMemoryProperties();

        protected:
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
            bool isDeviceSuitable(VkPhysicalDevice device);


        private:
            PhysicalDevice();
            virtual ~PhysicalDevice();

            static PhysicalDevice* m_physicalDevice;
            static VkPhysicalDevice m_VKphysicalDevice;
            SwapChainSupportDetails m_SwapChainDetails;
            VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
    };
}
#endif // PHYSICALDEVICE_H
