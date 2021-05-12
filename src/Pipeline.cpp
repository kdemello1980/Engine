#include "Pipeline.h" 
#include "Common.h"
#include "LogicalDevice.h"
#include "types.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Renderpass.h"
#include "DescriptorSet.h"


#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>

namespace KMDM
{
    /**
     * @brief Construct a new Pipeline:: Pipeline object
     * 
     * @param render_pass 
     */
    Pipeline::Pipeline(Renderpass* render_pass, DescriptorSet* descriptor_set)
    {
        m_renderPass = render_pass;
        m_logicalDevice = LogicalDevice::getInstance();
        m_descriptorSet = descriptor_set;
        createGraphicsPipeline();
    } 

    /**
     * @brief Destroy the Pipeline:: Pipeline object
     * 
     */
    Pipeline::~Pipeline()
    {
        vkDestroyPipelineLayout(m_logicalDevice->getLogicalDevice(), m_graphicsPipelineLayout, nullptr);
        vkDestroyPipeline(m_logicalDevice->getLogicalDevice(), m_graphicsPipeline, nullptr);
    }

    /**
     * @brief 
     * 
     */
    void Pipeline::destoryPipeline()
    {
        std::cout << "Cleaning up Pipeline." << std::endl;
        vkDestroyPipelineLayout(m_logicalDevice->getLogicalDevice(), m_graphicsPipelineLayout, nullptr);
        vkDestroyPipeline(m_logicalDevice->getLogicalDevice(), m_graphicsPipeline, nullptr);
    }

    /**
     * @brief 
     * 
     * @return VkPipeline* 
     */
    VkPipeline* Pipeline::getPipeline()
    {
        return &m_graphicsPipeline;
    }

    /**
     * @brief 
     * 
     * @return VkPipelineLayout* 
     */
    VkPipelineLayout* Pipeline::getPipelineLayout()
    {
        return &m_graphicsPipelineLayout;
    }
/******************************************************************************/


    /******************************************************************
        Create graphics pipeline.  Pipeline stages:

        1) Input assembler.
        2) Vertex shader (programmable).
        3) Tessellation shader (programmable).
        4) Geometry shader (programmable).
        5) Rasterization.
        6) Fragment shader (programmable).
        7) Color blending.

    *******************************************************************/
    void Pipeline::createGraphicsPipeline()
    {
        // Read in the bytecode.
        auto vertShaderCode = readFile("shaders/vert.spv");
        auto fragShaderCode = readFile("shaders/frag.spv");
        // Create the shader modules.
        VkShaderModule vertShaderModule = m_logicalDevice->createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = m_logicalDevice->createShaderModule(fragShaderCode);

        // Vertex shader stage.
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        // Tesselation shader stage goes here.
        // Geometry shader stage goes here.
        // Fragment shader stage.
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        // Vertex input.
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Input assembly.
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport.
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) SwapChain::getInstance()->getSwapChainExtent().width;
        viewport.height = (float) SwapChain::getInstance()->getSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor.
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = SwapChain::getInstance()->getSwapChainExtent();

        // Combine viewport and scissor into viewport state.
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterizer.
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; // using this requires enabling a gpu feature.
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
    //        rasterizer.depthBiasConstantFactor = 0.0f;
    //        rasterizer.depthBiasClamp = 0.0f;
    //        rasterizer.depthBiasSlopeFactor = 0.0f;

        // Multisampling.
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        // Color blending.
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                              | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        // Dynamic state.
        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        // Pipeline layout.
        VkDescriptorSetLayout layouts[] = {
            m_descriptorSet->getLayout()
        };

        // Push constants.
        VkPushConstantRange pushConstant = {};
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(CameraData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = layouts;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

        // Depth stencil.
        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        if (vkCreatePipelineLayout(m_logicalDevice->getLogicalDevice(), &pipelineLayoutInfo,
            nullptr, &m_graphicsPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout.");
        }
        std::cout << "Created pipeline layout." << std::endl;

        // Throw it all together.
        VkGraphicsPipelineCreateInfo pipeLineInfo = {};
        pipeLineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeLineInfo.stageCount = 2;
        pipeLineInfo.pStages = shaderStages;
        pipeLineInfo.pVertexInputState = &vertexInputInfo;
        pipeLineInfo.pInputAssemblyState = &inputAssembly;
        pipeLineInfo.pViewportState = &viewportState;
        pipeLineInfo.pRasterizationState = &rasterizer;
        pipeLineInfo.pMultisampleState = &multisampling;
        pipeLineInfo.pDepthStencilState = &depthStencil;
        pipeLineInfo.pColorBlendState = &colorBlending;
        // pipeLineInfo.pDynamicState = &dynamicState;
        pipeLineInfo.pDynamicState = nullptr;
        pipeLineInfo.layout = m_graphicsPipelineLayout;
        pipeLineInfo.renderPass = m_renderPass->getRenderPass();
        pipeLineInfo.subpass = 0;
        pipeLineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipeLineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(m_logicalDevice->getLogicalDevice(), VK_NULL_HANDLE, 1,
            &pipeLineInfo, nullptr, &m_graphicsPipeline)
                != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline.");
        }
        std::cout << "Created graphics pipeline." << std::endl;

        ////////////////////////////////////////
        vkDestroyShaderModule(m_logicalDevice->getLogicalDevice(), fragShaderModule, nullptr);
        vkDestroyShaderModule(m_logicalDevice->getLogicalDevice(), vertShaderModule, nullptr);
    } /// createGraphicsPipeline
}
