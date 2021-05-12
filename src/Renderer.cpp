#include "Renderer.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Window.h"
#include "Surface.h"
#include "Instance.h"
#include "Common.h"
#include "Scene.h"
#include "Model.h"
#include "Pipeline.h"
#include "Util.h"
#include "Allocator.h"
#include "DescriptorSet.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <array>
#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace KMDM
{
    Renderer* Renderer::m_renderer = nullptr;

    /**
     * @brief Construct a new Renderer:: Renderer object
     * 
     */
    Renderer::Renderer()
    {
        m_window = Window::getInstance();
        m_instance = Instance::getInstance();
        m_surface = Surface::getInstance();
        m_physicalDevice = PhysicalDevice::getInstance();
        m_logicalDevice = LogicalDevice::getInstance();
        m_swapChain = SwapChain::getInstance();
        m_commandPool = CommandPool::getInstance();

        // Get the number of swapchain images.
        m_numFramebuffers = m_swapChain->getSwapChainImages().size();

        m_renderPass = new Renderpass();
        m_descriptorSet = new DescriptorSet(m_renderPass);
        m_graphicsPipeline = new Pipeline(m_renderPass, m_descriptorSet);


        // Create the framebuffers.
        createFrameBuffers();

        // Initialize synchronization objects.
        createSyncObjects();

        // Create depth buffer.
        createDepthResources();

        // Create the camera buffer.
        createCameraBuffers();
    }

    void Renderer::recreateSwapChain()
    {

    }


    /**
     * @brief Destroy the Renderer object.
     * 
     */
    void Renderer::destroyRenderer()
    {
        std::cout << "- Cleaning up Renderer." << std::endl;
        delete(m_renderPass);
        m_commandPool->destroyCommandPool();
        m_swapChain->destroySwapChain();
        m_logicalDevice->destroyLogicalDevice();
        m_surface->destroySurface();
        m_instance->destoryInstance();
        m_window->destoryWindow();
    }

    /**
     * @brief Destroy the Renderer:: Renderer object
     * 
     */
    Renderer::~Renderer()
    {
        // Destory the camera buffers.
        for (size_t i = 0; i < m_cameraBuffers.size(); i++)
        {
            vmaDestroyBuffer(m_allocator->getAllocator(), m_cameraBuffers[i].buffer, m_cameraBuffers[i].allocation);
        }

        destroyRenderer();
    }

    /**
     * @brief Get an instance of the Renderer singleton.
     * 
     * @return Renderer* 
     */
    Renderer* Renderer::getInstance()
    {
        if (!m_renderer)
        {
            m_renderer = new Renderer();
        }
        return m_renderer;
    }

    /**
     * @brief Create the VkFramebuffer objects.
     * 
     */
    void Renderer::createFrameBuffers()
    {
        VkFramebufferCreateInfo frameInfo = {};
        frameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameInfo.renderPass = m_renderPass->getRenderPass();
        frameInfo.attachmentCount = 1;
        frameInfo.width = m_swapChain->getSwapChainExtent().width;
        frameInfo.height = m_swapChain->getSwapChainExtent().height;
        frameInfo.layers = 1;

        m_framebuffers.resize(m_numFramebuffers);
        for (size_t i = 0; i < m_framebuffers.size(); i++)
        {
            VkImageView attachments[2];
            attachments[0] = m_swapChain->getSwapChainImageViews()[i];
            attachments[1] = m_depthImageView;

            frameInfo.attachmentCount = 2;
            frameInfo.pAttachments = attachments;

            if (vkCreateFramebuffer(m_logicalDevice->getLogicalDevice(), &frameInfo,
                nullptr, &m_framebuffers[i]) != VK_SUCCESS)
            {
                std::runtime_error("Failed to create framebuffer.");
            }

            // Add camera to frameData
        }
    }


    /**
     * @brief Create the depth image and image view.
     * 
     */
    void Renderer::createDepthResources()
    {
        // Get the depth format.
        VkFormat depthFormat = m_renderPass->findDepthFormat();

        // Create the depth image.
        m_depthImage = m_allocator->getVMAImage(
            m_swapChain->getSwapChainExtent().width,
            m_swapChain->getSwapChainExtent().height,
            1,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.mipLevels = 1;

        m_depthImageView = createImageView(m_depthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    }

    /**
     * @brief Create the camera buffers.
     * 
     */
    void Renderer::createCameraBuffers()
    {
        size_t size = SwapChain::getInstance()->getSwapChainImages().size();
        m_cameraBuffers.resize(size);

        for (size_t i = 0; i < size; i++)
        {
            m_cameraBuffers[i] = m_allocator->getVMABuffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                VMA_MEMORY_USAGE_CPU_TO_GPU);
        }
    }


    /**
     * @brief Draw the frame.
     * 
     */
    void Renderer::drawFrame()
    {
        // Get the current frame.
        size_t currentFrame = m_frameSyncObjects.currentFrame;

        // Wait for the fence.
        vkWaitForFences(m_logicalDevice->getLogicalDevice(),
            1,
            &m_frameSyncObjects.inflightFences[currentFrame],
            VK_TRUE,
            UINT64_MAX);

        // Acquire an image from the swapchain.
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            m_logicalDevice->getLogicalDevice(),
            m_swapChain->getSwapChain(),
            UINT64_MAX,
            m_frameSyncObjects.imageAvailableSemaphores[currentFrame],
            VK_NULL_HANDLE,
            &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain(); // gotta bite the bullet on this one.
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swapchain image.");
        }

        // Wait for the image in flight fence.
        if (m_frameSyncObjects.imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(m_logicalDevice->getLogicalDevice(),
                1,
                &m_frameSyncObjects.imagesInFlight[imageIndex],
                VK_TRUE,
                UINT64_MAX);
        }
        m_frameSyncObjects.imagesInFlight[imageIndex] =  m_frameSyncObjects.inflightFences[currentFrame];

        // Update the uniform buffer associaged with the image.
        //updateUniformBuffer(imageIndex);

        // Submit the command buffer.
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_frameSyncObjects.imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitstages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitstages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_drawCommandBuffers[imageIndex]; // Need to incorporate command buffers.

        VkSemaphore signalSemaphores[] = {m_frameSyncObjects.renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_logicalDevice->getLogicalDevice(), 1, &m_frameSyncObjects.inflightFences[currentFrame]);

        // Submit to the graphics queue.
        if (vkQueueSubmit(m_logicalDevice->getGraphicsQueue(), 1, &submitInfo,
            m_frameSyncObjects.inflightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer.");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_swapChain->getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(m_logicalDevice->getPresentationQueue(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_frameBufferResized)
        {
            m_frameBufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swapchain image.");
        }

        vkQueueWaitIdle(m_logicalDevice->getPresentationQueue());
        m_descriptorSet->resetDescriptorPool();
        m_frameSyncObjects.incrementFrame();
    } /// drawFrame

  
    /**
     * @brief Create the frame synchronization objects.
     * 
     */
    void Renderer::createSyncObjects()
    {
        m_frameSyncObjects.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_frameSyncObjects.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_frameSyncObjects.inflightFences.resize(MAX_FRAMES_IN_FLIGHT);
        m_frameSyncObjects.imagesInFlight.resize(m_swapChain->getSwapChainImages().size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(m_logicalDevice->getLogicalDevice(), &semaphoreInfo, nullptr,
                &m_frameSyncObjects.renderFinishedSemaphores[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create render finished semaphore.");
            }
            if (vkCreateSemaphore(m_logicalDevice->getLogicalDevice(), &semaphoreInfo, nullptr,
                &m_frameSyncObjects.imageAvailableSemaphores[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image finished semaphore.");
            }
            if (vkCreateFence(m_logicalDevice->getLogicalDevice(), &fenceInfo, nullptr,
                &m_frameSyncObjects.inflightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create in flight fence.");
            }
        }
    }

    /**
     * @brief Create the command buffers for issuing draw calls.
     * 
     */
    void Renderer::createCommandBuffers()
    {
        m_drawCommandBuffers.resize(m_framebuffers.size());
        // Allocate the command buffers.
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPool->getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) m_drawCommandBuffers.size();

        if (vkAllocateCommandBuffers(m_logicalDevice->getLogicalDevice(), &allocInfo, m_drawCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers.");
        }

        // Do something with a descriptor set.
        std::vector<VkDescriptorSet> descriptorSets = m_descriptorSet->getDescriptorSets();

        // Begin recording command buffers.
        size_t bindingIndex = 0;
        for (size_t i = 0; i < m_drawCommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(m_drawCommandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to begin recording command buffer.");
            }

            // Start render passes.
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_renderPass->getRenderPass();
            renderPassInfo.framebuffer = m_framebuffers[i];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

            // Clear the color and depth stencil at the beginning of our renderpass.
            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_drawCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Bind the graphics pipeline.
            vkCmdBindPipeline(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, *m_graphicsPipeline->getPipeline());

// This should be where we deal with dynamic storage buffers for streaming our geometry into a single draw call
// rather than calling an individual draw call for each individual mesh.

            for (auto & mesh : Scene::getInstance()->getMeshes())
            {
                // Bind the vertex & index buffers.
                // vertexBuffers.push_back(mesh.getVertexBuffer());
                // indexBuffers.
                VkDeviceSize offsets[] = {0};
                VkBuffer buffers[] =  { mesh.getVertexBuffer().buffer };
                vkCmdBindVertexBuffers(m_drawCommandBuffers[i], 0, 1, buffers, offsets);
                vkCmdBindIndexBuffer(m_drawCommandBuffers[i], mesh.getIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);
                
                // Bind the UBO descriptor sets.
                vkCmdBindDescriptorSets(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, 
                    *m_graphicsPipeline->getPipelineLayout(), 0, 1, &descriptorSets[bindingIndex],
                    0, nullptr);
                bindingIndex++;

                // Draw.
                // vkCmdDraw(m_drawCommandBuffers[i], 3, 1, 0, 0);

                vkCmdDrawIndexed(m_drawCommandBuffers[i], static_cast<uint32_t>(mesh.getIndices().size()), 1, 0, 0, 0);

            }

            vkCmdEndRenderPass(m_drawCommandBuffers[i]);

            if (vkEndCommandBuffer(m_drawCommandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    } /// createCommandBuffers
}
