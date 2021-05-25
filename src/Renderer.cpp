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
#include <chrono>

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>
// #include <GLFW/glfw3native.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_video.h>

namespace KMDM
{
    Renderer* Renderer::m_renderer = nullptr;

    /**
     * @brief Construct a new Renderer:: Renderer object
     * 
     */
    Renderer::Renderer()
    {
        m_currentFrame = 0;
        m_window = Window::getInstance();
        m_instance = Instance::getInstance();
        m_surface = Surface::getInstance();
        m_physicalDevice = PhysicalDevice::getInstance();
        m_logicalDevice = LogicalDevice::getInstance();
        m_swapChain = SwapChain::getInstance();
        m_commandPool = CommandPool::getInstance();

        // Get the number of swapchain images.
        m_numFramebuffers = static_cast<size_t>(m_swapChain->getSwapChainImages().size());

        // Initialize synchronization objects.
        createSyncObjects();

        m_renderPass = new Renderpass();
        m_descriptorSet = new DescriptorSet(m_renderPass);
        m_graphicsPipeline = new Pipeline(m_renderPass, m_descriptorSet);
        
        // Create depth buffer before the framebuffers.
        createDepthResources();

        // Create the framebuffers.
        createFrameBuffers();

        // Create the command buffers.
        createCommandBuffers();

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
        // vkm_logicalDevice->getLogicalDevice()WaitIdle(m_logicalDevice->getLogicalDevice());
        std::cout << "- Cleaning up Renderer." << std::endl;

        cleanupDepthResources();
        cleanupCameraBuffers();

        for (auto & framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_logicalDevice->getLogicalDevice(), framebuffer, nullptr);
        }

        m_commandPool->destroyCommandPool();

        delete(m_graphicsPipeline);
        delete(m_renderPass);
        delete(m_descriptorSet);

        

        m_swapChain->destroySwapChain();
        cleanupSyncObjects();

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
        std::cout << "-- Calling Renderer destructor." << std::endl;
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
        std::cout << "Created framebuffers." << std::endl;
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
        // m_depthImage = m_allocator->getVMAImage(
        //     m_swapChain->getSwapChainExtent().width,
        //     m_swapChain->getSwapChainExtent().height,
        //     1,
        //     depthFormat,
        //     VK_IMAGE_TILING_OPTIMAL,
        //     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        //     VMA_MEMORY_USAGE_GPU_ONLY,
        //     VK_MEMORY_PROPERTY_m_logicalDevice->getLogicalDevice()_LOCAL_BIT
        // );


        // VkImageCreateInfo imageInfo = {};
        // imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        // imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        // imageInfo.mipLevels = 1;

        
        createImage( m_swapChain->getSwapChainExtent().width, 
            m_swapChain->getSwapChainExtent().height,
            1,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_depthImage,
            m_depthMemory
        );

        m_depthImageView = createImageView(m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
        std::cout << "Created depth imageview." << std::endl;
    }

    /**
     * @brief Clean up the depth resources.
     * 
     */
    void Renderer::cleanupDepthResources()
    {
        vkDestroyImageView(m_logicalDevice->getLogicalDevice(), m_depthImageView, nullptr);
        vkDestroyImage(m_logicalDevice->getLogicalDevice(), m_depthImage, nullptr);
        vkFreeMemory(m_logicalDevice->getLogicalDevice(), m_depthMemory, nullptr);
    }

    /**
     * @brief Create the camera buffers.
     * 
     */
    void Renderer::createCameraBuffers()
    {
        size_t size = SwapChain::getInstance()->getSwapChainImages().size();
        m_cameraBuffers.resize(size);
        m_cameraMemory.resize(size);

        for (size_t i = 0; i < size; i++)
        {
            createBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                m_cameraBuffers[i], m_cameraMemory[i] );
        }
        std::cout << "Created camera buffers." << std::endl;
    }

    /**
     * @brief Clean up the camera buffers.
     * 
     */
    void Renderer::cleanupCameraBuffers()
    {
        // Destory the camera buffers.
        for (size_t i = 0; i < m_cameraBuffers.size(); i++)
        {
            vkDestroyBuffer(m_logicalDevice->getLogicalDevice(), m_cameraBuffers[i], nullptr);
            vkFreeMemory(m_logicalDevice->getLogicalDevice(), m_cameraMemory[i], nullptr);
            std::cout << "- Cleaning up camera buffers." << std::endl;
        }
    }


    /**
     * @brief Draw the frame.
     * 
     */
    void Renderer::drawFrame()
    {
        // Wait for the fence.
        vkWaitForFences(m_logicalDevice->getLogicalDevice(),
            1,
            &m_frameSyncObjects.inflightFences[m_currentFrame],
            VK_TRUE,
            UINT64_MAX);

        // Acquire an image from the swapchain.
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            m_logicalDevice->getLogicalDevice(),
            m_swapChain->getSwapChain(),
            UINT64_MAX,
            m_frameSyncObjects.imageAvailableSemaphores[m_currentFrame],
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
        m_frameSyncObjects.imagesInFlight[imageIndex] =  m_frameSyncObjects.inflightFences[m_currentFrame];

        // Update the uniform buffer associaged with the image.
        updateUniformBuffer(imageIndex);

        // Submit the command buffer.
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_frameSyncObjects.imageAvailableSemaphores[m_currentFrame]};
        VkPipelineStageFlags waitstages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitstages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_drawCommandBuffers[imageIndex]; // Need to incorporate command buffers.

        VkSemaphore signalSemaphores[] = {m_frameSyncObjects.renderFinishedSemaphores[m_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_logicalDevice->getLogicalDevice(), 1, &m_frameSyncObjects.inflightFences[m_currentFrame]);

        // Submit to the graphics queue.
        if (vkQueueSubmit(m_logicalDevice->getGraphicsQueue(), 1, &submitInfo,
            m_frameSyncObjects.inflightFences[m_currentFrame]) != VK_SUCCESS)
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
            m_descriptorSet->resetDescriptorPools();
            m_currentFrame = (m_currentFrame += 1) % MAX_FRAMES_IN_FLIGHT;
    } /// drawFrame

  
    /**
     * @brief Create the frame synchronization objects.
     * 
     */
    void Renderer::createSyncObjects()
    {
        m_frameSyncObjects = {};
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
     * @brief Clean up semaphores and fences.
     * 
     */
    void Renderer::cleanupSyncObjects()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(m_logicalDevice->getLogicalDevice(), 
                m_frameSyncObjects.imageAvailableSemaphores[i], nullptr);

            vkDestroySemaphore(m_logicalDevice->getLogicalDevice(), 
                m_frameSyncObjects.renderFinishedSemaphores[i], nullptr);

            vkDestroyFence(m_logicalDevice->getLogicalDevice(),
                m_frameSyncObjects.imagesInFlight[i], nullptr);

            vkDestroyFence(m_logicalDevice->getLogicalDevice(),
                m_frameSyncObjects.inflightFences[i], nullptr);
        }
        std::cout << "- Cleaning up semaphores and fences." << std::endl;
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

        // Update the descriptor sets for the scene.
        std::vector<VkDescriptorSet> sceneDescriptorSets = m_descriptorSet->getSceneDescriptorSets(
            static_cast<uint32_t>(m_swapChain->getSwapChainImages().size()), m_gpuSceneData, m_uniformBuffers);

        // Update tdhe descriptor sets for the models.
        std::vector<Model> models = Scene::getInstance()->getMeshes();
        std::vector<VkDescriptorSet> modelSets = m_descriptorSet->getModelDescriptorSets(models);

        // Begin recording command buffers.
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
            vkCmdBindVertexBuffers(m_drawCommandBuffers[i], 0, 1, &m_uniformBuffers[i], 0);

            // vkCmdPushConstants(m_drawCommandBuffers[i], *m_graphicsPipeline->getPipelineLayout(),
            //     VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraData), &m_cameraBuffers[i]);
            vkCmdBindDescriptorSets(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, 
                *m_graphicsPipeline->getPipelineLayout(), 0, 1, &sceneDescriptorSets[i], 0, nullptr);
            
            for (size_t j = 0; j < models.size(); j++)
            {
                // Bind the vertex & index buffers.
                // vertexBuffers.push_back(mesh.getVertexBuffer());
                // indexBuffers.
                VkBuffer buffers[] =  { *models[j].getVertexBuffer() };
                vkCmdBindVertexBuffers(m_drawCommandBuffers[j], 0, 1, buffers, 0);
                vkCmdBindIndexBuffer(m_drawCommandBuffers[j], *models[i].getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
                
                // Bind the UBO descriptor sets.
                vkCmdBindDescriptorSets(m_drawCommandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, 
                    *m_graphicsPipeline->getPipelineLayout(), 0, 1, &modelSets[j],
                    0, nullptr);

                // Draw.
                // vkCmdDraw(m_drawCommandBuffers[i], 3, 1, 0, 0);

                vkCmdDrawIndexed(m_drawCommandBuffers[j], models[j].getIndexCount(), 1, 0, 0, 0);
            }
            vkCmdEndRenderPass(m_drawCommandBuffers[i]);

            if (vkEndCommandBuffer(m_drawCommandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    } /// createCommandBuffers

    void Renderer::run()
    {
        
        SDL_Event event;
        while(true)
        {
            while(SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    destroyRenderer();
                    // m_window->destoryWindow();
                }
            }
            createCommandBuffers();
            drawFrame();
        }
    }

    /**
     * @brief Create the uniform buffers.
     * 
     */
    void Renderer::createUniformBuffers()
    {
        VkDeviceSize size = sizeof(UniformBufferObject);
        m_uniformBuffers.resize(m_swapChain->getSwapChainImages().size());
        m_uniformBufferMemory.resize(m_swapChain->getSwapChainImages().size());

        for (size_t i = 0; i < m_swapChain->getSwapChainImages().size(); i++)
        {
            createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_uniformBuffers[i], m_uniformBufferMemory[i]);
        }
    }

    /**
     * @brief Create the GPUSceneData buffers.
     * 
     */
    void Renderer::createGPUSceneBuffers()
    {
        VkDeviceSize size = sizeof(GPUSceneData);
        m_gpuSceneData.resize(m_swapChain->getSwapChainImages().size());
        m_gpuSceneMemory.resize(m_swapChain->getSwapChainImages().size());

        for (size_t i = 0; i < m_swapChain->getSwapChainImages().size(); i++)
        {
            createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_gpuSceneData[i], m_gpuSceneMemory[i]);
        }
    }

    void Renderer::cleanupGPUSceneBuffers()
    {
        for (size_t i = 0; i < m_gpuSceneData.size(); i++)
        {
            vkDestroyBuffer(m_logicalDevice->getLogicalDevice(), m_gpuSceneData[i], nullptr);
        }
        for (size_t i = 0; i < m_gpuSceneMemory.size(); i++)
        {
            vkFreeMemory(m_logicalDevice->getLogicalDevice(), m_gpuSceneMemory[i], nullptr);
        }
    }

    /**
     * @brief Update the uniform buffer.
     * 
     * @param currentImage 
     */
    void Renderer::updateUniformBuffer(uint32_t currentImage) 
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), m_swapChain->getSwapChainExtent().width / 
            (float) m_swapChain->getSwapChainExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        void* data;
        vkMapMemory(m_logicalDevice->getLogicalDevice(), m_uniformBufferMemory[currentImage], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(m_logicalDevice->getLogicalDevice(), m_uniformBufferMemory[currentImage]);
    }
}
