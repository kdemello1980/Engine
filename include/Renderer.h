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
#include "DescriptorPool.h"

namespace KMDM
{
    class Renderer
    {
        public:
            static Renderer* getInstance();
            void destroyRenderer();
            virtual ~Renderer();

            void drawFrame();

        protected:
            void createFrameBuffers();
            void createSyncObjects();
            void createCommandBuffers();
            void recreateSwapChain();
            void createDepthResources();
            void createCameraBuffers();

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

            // Renderpass
            Renderpass* m_renderPass;

            // Framebuffers
            std::vector<VkFramebuffer> m_framebuffers;
            uint32_t m_numFramebuffers;

            // FrameData
            std::vector<FrameData> m_frameData;

            // Synchronization objects.
            FrameSyncObjects m_frameSyncObjects;

            // Draw command buffers.
            std::vector<VkCommandBuffer> m_drawCommandBuffers;

            // Graphics pipeline.
            Pipeline* m_graphicsPipeline;

            // Depth resources.
            // VkBuffer m_depthBuffer;
            // VkDeviceMemory m_depthBufferMemory;
            // VkImage m_depthImage;
            VkImageView m_depthImageView;
            AllocatedImage m_depthImage;

            // Camera buffer.
            // CameraData m_cameraData;
            std::vector<AllocatedBuffer> m_cameraBuffers;
    };
}
#endif // RENDERER_H
