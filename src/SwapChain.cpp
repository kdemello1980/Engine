#include "../include/SwapChain.h"
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>

#include "../include/LogicalDevice.h"
#include "../include/PhysicalDevice.h"
#include "../include/Surface.h"
#include "../include/types.h"
#include "Util.h"

namespace KMDM
{
    VkSwapchainKHR SwapChain::m_VKswapChain = VK_NULL_HANDLE;
    SwapChain* SwapChain::m_swapChain = nullptr;
/******************************************************************************/
    SwapChain* SwapChain::getInstance()
    {
        if (!m_swapChain)
        {
            m_swapChain = new SwapChain();
        }
        return m_swapChain;
    }
/******************************************************************************/
    VkSwapchainKHR SwapChain::getSwapChain()
    {
        return m_VKswapChain;
    }

    /**
     * @brief Construct a new Swap Chain:: Swap Chain object
     * 
     */
    SwapChain::SwapChain()
    {
        // Singletons.
        m_physicalDevice = PhysicalDevice::getInstance();
        m_logicalDevice = LogicalDevice::getInstance();
        m_surface = Surface::getInstance();

        m_swapChainDetails = m_physicalDevice->getSwapChainSupportDetails();
        VkSurfaceFormatKHR swformat = chooseSwapSurfaceFormat(m_swapChainDetails.formats);
        VkPresentModeKHR present_mode = chooseSwapPresentationMode(m_swapChainDetails.presentModes);
        m_swapChainExtent = chooseSwapExtent(m_swapChainDetails.capabilities);

        // It is better to use 1 more than the minimum image count in the swapchain. If possible.
        uint32_t image_count = m_swapChainDetails.capabilities.minImageCount + 1;
        if (m_swapChainDetails.capabilities.maxImageCount > 0 &&
                image_count > m_swapChainDetails.capabilities.maxImageCount)
        {
            image_count = m_swapChainDetails.capabilities.maxImageCount;
        }
        // Populate another giant struct.
        VkSwapchainCreateInfoKHR create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.pNext = nullptr;
        create_info.surface = m_surface->getSurface();
        create_info.minImageCount = image_count;
        create_info.imageFormat = swformat.format;
        create_info.imageColorSpace = swformat.colorSpace;
        create_info.imageExtent = m_swapChainExtent;
        create_info.imageArrayLayers = 1; // always 1 unless stereoscopic
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Handle the queue family indices for the case where graphics and presentation
        // are different, and then when they are the same.
        QueueFamilyInfo queueFamily = m_logicalDevice->getQueueFamilyInfo();
        if (queueFamily.graphicsFamilyIndex.value() != queueFamily.presentationFamilyIndex.value())
        {
            uint32_t family_indices[] =
            {
                queueFamily.graphicsFamilyIndex.value(),
                queueFamily.presentationFamilyIndex.value()
            };
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;
            create_info.pQueueFamilyIndices = nullptr;
        }
        create_info.preTransform = m_swapChainDetails.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;
        if (vkCreateSwapchainKHR(m_logicalDevice->getLogicalDevice(), &create_info, nullptr, &m_VKswapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swapchain.");
        }
        std::cout <<  "Created swapchain." << std::endl;

        // Get the swapchain images.
        uint32_t num_images = 0;
        vkGetSwapchainImagesKHR(m_logicalDevice->getLogicalDevice(), m_VKswapChain, &num_images,  nullptr);
        if (num_images == 0)
        {
            throw std::runtime_error("Failed to retrieve swapchain images.");
        }        
        m_swapChainImages.resize(num_images);

        vkGetSwapchainImagesKHR(m_logicalDevice->getLogicalDevice(), m_VKswapChain, &num_images, m_swapChainImages.data());
        std::cout << "Created " << num_images << " swapchain images." << std::endl;

        m_swapChainImageViews.resize(num_images);
        for (int i = 0; i < num_images; i++)
        {
            m_swapChainImageViews[i] = createImageView(m_swapChainImages[i], m_swapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT, 1);
            std::cout << "Created swapchain imageview: " << i << std::endl;
        }        
    }

    /**
     * @brief Destory the SwapChain.
     * 
     */
    void SwapChain::destroySwapChain()
    {
        if (m_swapChain)
        {
            for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
            {
                vkDestroyImageView(m_logicalDevice->getLogicalDevice(), m_swapChainImageViews[i], nullptr);
            }
            vkDestroySwapchainKHR(m_logicalDevice->getLogicalDevice(), m_VKswapChain, nullptr);
        }
        std::cout << "- Cleaning up SwapChain." << std::endl;
    }
/******************************************************************************/
    SwapChain::~SwapChain()
    {
        destroySwapChain();
    }
/******************************************************************************/
    VkFormat SwapChain::getSwapChainImageFormat()
    {
        return m_swapChainImageFormat;
    }

/******************************************************************************/
    /**
        Query details of swapchain support.
    */
    SwapChainSupportDetails SwapChain::querySwapChainSupport()
    {
        SwapChainSupportDetails details;
        // Get surface capabilities.
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice->getPhysicalDevice(), m_surface->getSurface(),
                &details.capabilities) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to get surface capabilities.");
        }
        // Get supported surface formats.
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice->getPhysicalDevice(), m_surface->getSurface(), &format_count, nullptr);
        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice->getPhysicalDevice(), m_surface->getSurface(), &format_count,
                                                 details.formats.data());
        }
        // Get the supported presentation modes.
        uint32_t mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice->getPhysicalDevice(), m_surface->getSurface(), &mode_count, nullptr);
        if (mode_count != 0)
        {
            details.presentModes.resize(mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice->getPhysicalDevice(), m_surface->getSurface(), &mode_count,
                    details.presentModes.data());
        }
        return details;
    }
/******************************************************************************/
    /******************************************************************
        Choose the surface format.
    *******************************************************************/
    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        // Go through the available formats and choose the appropriate one.
        for (auto & format : available_formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                m_swapChainImageFormat = format.format;
                return format;
            }
        }
        // just return the first one if nothing else is available.
        return available_formats[0];
    }

/******************************************************************************/
    /******************************************************************
        Choose the presentation mode.
            - VK_PRESENT_MODE_MAILBOX_KHR if available.
            - VK_PRESENT_MODE_FIFO_KHR otherwise.
    *******************************************************************/
    VkPresentModeKHR SwapChain::chooseSwapPresentationMode(const std::vector<VkPresentModeKHR>& available_modes)
    {
        for (auto & pres_mode : available_modes)
        {
            if (pres_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return pres_mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    } /// choseSwapPresentationMode


/******************************************************************************/
    /******************************************************************
        Choose the swap extent.
    *******************************************************************/
    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            ExtentSize eSize = Window::getInstance()->getWindowSize();

            VkExtent2D actual_extent =
            {
                static_cast<uint32_t>(eSize.width),
                static_cast<uint32_t>(eSize.height)
            };

            actual_extent.width = std::max(capabilities.maxImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actual_extent.width));

            actual_extent.height = std::max(capabilities.maxImageExtent.height,
                std::min(capabilities.maxImageExtent.height, capabilities.maxImageExtent.height));

            return actual_extent;
        }
    } /// chooseSwapExtent
/******************************************************************************/
    std::vector<VkImage> SwapChain::getSwapChainImages()
    {
        return m_swapChainImages;
    }

/******************************************************************************/
    std::vector<VkImageView> SwapChain::getSwapChainImageViews()
    {
        return m_swapChainImageViews;
    }
/******************************************************************************/
    VkExtent2D SwapChain::getSwapChainExtent()
    {
        return m_swapChainExtent;
    }

}
