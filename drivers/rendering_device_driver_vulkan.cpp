/* ======================================================================== */
/* rendering_device_driver_vulkan.cpp                                       */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           COPILOT ENGINE                                 */
/* ======================================================================== */
/*                                                                          */
/* Copyright (C) 2022 Vcredent All rights reserved.                         */
/*                                                                          */
/* Licensed under the Apache License, Version 2.0 (the "License");          */
/* you may not use this file except in compliance with the License.         */
/*                                                                          */
/* You may obtain a copy of the License at                                  */
/*     http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                          */
/* Unless required by applicable law or agreed to in writing, software      */
/* distributed under the License is distributed on an "AS IS" BASIS,        */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* ======================================================================== */
#include "rendering_device_driver_vulkan.h"
// copilot
#include "vulkan_utils.h"

RenderingDeviceDriverVulkan::RenderingDeviceDriverVulkan(RenderingContextDriverVulkan *p_ctx)
    : render_driver_context(p_ctx)
{
    vk_device = render_driver_context->get_device();
    allocator = render_driver_context->get_allocator();

    _initialize_descriptor_pool();
}

RenderingDeviceDriverVulkan::~RenderingDeviceDriverVulkan()
{
    vkDestroyDescriptorPool(vk_device, descriptor_pool, allocation_callbacks);
}

void RenderingDeviceDriverVulkan::create_graph_pipeline(VkPipeline *p_pipeline)
{
    VkResult U_ASSERT_ONLY err;

    VkPipelineLayoutCreateInfo pipeline_layout_creat_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* setLayoutCount */ 0,
            /* pSetLayouts */ nullptr,
            /* pushConstantRangeCount */ 0,
            /* pPushConstantRanges */ nullptr,
    };

    VkPipelineLayout pipeline_layout;
    err = vkCreatePipelineLayout(vk_device, &pipeline_layout_creat_info, allocation_callbacks, &pipeline_layout);
    assert(!err);

    VkShaderModule vertex_shader_module, fragment_shader_module;

    vertex_shader_module = load_shader_module(vk_device, "../shader/vertex.glsl.spv");
    fragment_shader_module = load_shader_module(vk_device, "../shader/fragment.glsl.spv");

    VkPipelineShaderStageCreateInfo vertex_shader_create_info = {};
    vertex_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_create_info.module = vertex_shader_module;
    vertex_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_create_info = {};
    fragment_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_create_info.module = fragment_shader_module;
    fragment_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages_info[] = {
            vertex_shader_create_info,
            fragment_shader_create_info
    };

    VkVertexInputBindingDescription input_binds[] = {
            { 0, 0, VK_VERTEX_INPUT_RATE_VERTEX }
    };

    VkVertexInputAttributeDescription input_attributes[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 }
    };

    VkPipelineVertexInputStateCreateInfo input_state_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* vertexBindingDescriptionCount */ ARRAY_SIZE(input_binds),
            /* pVertexBindingDescriptions */ input_binds,
            /* vertexAttributeDescriptionCount */ ARRAY_SIZE(input_attributes),
            /* pVertexAttributeDescriptions */ input_attributes,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* topology */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            /* primitiveRestartEnable */ VK_FALSE,
    };

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = render_driver_context->get_width();
    viewport.height = render_driver_context->get_height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = render_driver_context->get_width();
    scissor.extent.width = render_driver_context->get_height();

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* viewportCount */ 1,
            /* pViewports */ &viewport,
            /* scissorCount */ 1,
            /* pScissors */ &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
    rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_create_info.depthClampEnable = VK_FALSE;
    rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_create_info.lineWidth = 1.0f;
    rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state_create_info.minSampleShading = 1.0f;
    multisample_state_create_info.pSampleMask = nullptr;
    multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.blendEnable = VK_TRUE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
    color_blend_state_create_info.blendConstants[0] = 0.0f;
    color_blend_state_create_info.blendConstants[1] = 0.0f;
    color_blend_state_create_info.blendConstants[2] = 0.0f;
    color_blend_state_create_info.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* stageCount */ ARRAY_SIZE(shader_stages_info),
            /* pStages */ shader_stages_info,
            /* pVertexInputState */ &input_state_create_info,
            /* pInputAssemblyState */ &input_assembly,
            /* pTessellationState */ nullptr,
            /* pViewportState */ &viewport_state_create_info,
            /* pRasterizationState */ &rasterization_state_create_info,
            /* pMultisampleState */ &multisample_state_create_info,
            /* pDepthStencilState */ nullptr,
            /* pColorBlendState */ &color_blend_state_create_info,
            /* pDynamicState */ nullptr,
            /* layout */ pipeline_layout,
            /* renderPass */ render_driver_context->get_render_pass(),
            /* subpass */ 0,
            /* basePipelineHandle */ VK_NULL_HANDLE,
            /* basePipelineIndex */ -1,
    };

    err = vkCreateGraphicsPipelines(vk_device, nullptr, 1, &pipeline_create_info, allocation_callbacks, p_pipeline);
    assert(!err);

    vkDestroyShaderModule(vk_device, vertex_shader_module, allocation_callbacks);
    vkDestroyShaderModule(vk_device, fragment_shader_module, allocation_callbacks);
}

void RenderingDeviceDriverVulkan::_initialize_descriptor_pool()
{
    VkResult U_ASSERT_ONLY err;

    VkDescriptorPoolSize pool_size[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                64 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          64 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          64 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   64 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   64 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         64 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         64 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 64 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 64 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       64 }
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* maxSets */ 1024,
            /* poolSizeCount */ ARRAY_SIZE(pool_size),
            /* pPoolSizes */ pool_size,
    };

    err = vkCreateDescriptorPool(vk_device, &descriptor_pool_create_info, allocation_callbacks, &descriptor_pool);
    assert(!err);
}