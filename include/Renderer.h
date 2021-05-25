#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <array>

#include "Window.h"
#include "Surface.h"
#include "Instance.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h" 
#include "CommandPool.h"
#include "SwapChain.h"
#include "Renderpass.h"
#include "types.h"
#include "Pipeline.h"
#include "Allocator.h"
#include "DescriptorSet.h"

namespace KMDM
{
    class Renderer
    {
        public:
            static Renderer* getInstance();
            void destroyRenderer();
            virtual ~Renderer();

            void drawFrame();
            void run();
            
        protected:
            void createFrameBuffers();
            void createSyncObjects();
            void createCommandBuffers();
            void recreateSwapChain();
            void createDepthResources();
            void createCameraBuffers();
            void createUniformBuffers();
            void createGPUSceneBuffers();

            void updateUniformBuffer(uint32_t currentImage);

            void cleanupDepthResources();
            void cleanupSyncObjects();
            void cleanupCameraBuffers();
            void cleanupGPUSceneBuffers();


        private:
            Renderer();

            static Renderer* m_renderer;
            bool m_frameBufferResized = false;

            // Singletons.
            Window* m_window;
            Instance* m_instance;
            Surface* m_surface;
            PhysicalDevice* m_physicalDevice;
            LogicalDevice* m_logicalDevice;
            SwapChain* m_swapChain;
            CommandPool* m_commandPool;
            Allocator* m_allocator;
            DescriptorSet* m_descriptorSet;

            // Renderpass
            Renderpass* m_renderPass;

            // Framebuffers
            std::vector<VkFramebuffer> m_framebuffers;
            size_t m_numFramebuffers;

            // FrameData
            std::vector<FrameData> m_frameData;

            // Synchronization objects.
            FrameSyncObjects m_frameSyncObjects;

            // Draw command buffers.
            std::vector<VkCommandBuffer> m_drawCommandBuffers;

            // Graphics pipeline.
            Pipeline* m_graphicsPipeline;

            // Depth resources.
            VkImageView m_depthImageView;
            VkImage m_depthImage;
            VkDeviceMemory m_depthMemory;

            // Camera buffer.
            // CameraData m_cameraData;
            std::vector<VkBuffer> m_cameraBuffers;
            std::vector<VkDeviceMemory> m_cameraMemory;

            // Descriptor set vector.
            std::vector<VkDescriptorSet> m_descriptorSets;

            // The current frame.
            size_t m_currentFrame;

            // Uniform Buffers.
            std::vector<VkBuffer> m_uniformBuffers;
            std::vector<VkDeviceMemory> m_uniformBufferMemory;

            // Scene data.
            std::vector<VkBuffer> m_gpuSceneData;
            std::vector<VkDeviceMemory> m_gpuSceneMemory;
    };
}
#endif // RENDERER_H
