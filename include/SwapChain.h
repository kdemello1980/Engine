#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Surface.h"

namespace KMDM
{
    class SwapChain
    {
        public:
            static SwapChain* getInstance();
            VkSwapchainKHR getSwapChain();
            void destroySwapChain();
            VkFormat getSwapChainImageFormat();

            std::vector<VkImage> getSwapChainImages();
            std::vector<VkImageView> getSwapChainImageViews();


            VkExtent2D getSwapChainExtent();

        protected:
            SwapChainSupportDetails querySwapChainSupport();
            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
            VkPresentModeKHR chooseSwapPresentationMode(const std::vector<VkPresentModeKHR>& available_modes);
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        private:
            SwapChain();
            virtual ~SwapChain();

            /**
                Member variables.
            */
            static VkSwapchainKHR m_VKswapChain;
            static SwapChain* m_swapChain;
            VkFormat m_swapChainImageFormat;
            std::vector<VkImageView> m_swapChainImageViews;
            std::vector<VkImage> m_swapChainImages;
            std::vector<VkFramebuffer> m_swapChainFramebuffers;
            VkExtent2D m_swapChainExtent;
            SwapChainSupportDetails m_swapChainDetails;

            PhysicalDevice* m_physicalDevice;
            LogicalDevice* m_logicalDevice;
            Surface* m_surface;
    };
}
#endif // SWAPCHAIN_H
