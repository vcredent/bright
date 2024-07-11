/* ======================================================================== */
/* rendering_device_driver_vulkan.cpp                                       */
/* ======================================================================== */
/*                        This file is part of:                             */
/*                           PORTABLE ENGINE                                */
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

RenderingDeviceDriverVulkan::RenderingDeviceDriverVulkan(VkContext *context)
    : vk_context(context)
{
    // initialize
    device = vk_context->get_device();
    allocator = vk_context->get_allocator();
}

RenderingDeviceDriverVulkan::~RenderingDeviceDriverVulkan()
{
    // ...
}

void RenderingDeviceDriverVulkan::create_render_pipeline()
{
    VkResult U_ASSERT_ONLY err;

    // vertex and fragment shader stage create
    // the index 0 is vertex shader, that index 1
    // is fragment shader stage
    VkPipelineShaderStageCreateInfo shader_stages[] = {
        /* vertex shader stage */
        {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* stage */ VK_SHADER_STAGE_VERTEX_BIT,
            /* module */ nullptr,
            /* pName */ "main",
            /* pSpecializationInfo */ nullptr,
        },
        /* fragment shader stage */
        {
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /* pNext */ nextptr,
            /* flags */ no_flag_bits,
            /* stage */ VK_SHADER_STAGE_FRAGMENT_BIT,
            /* module */ nullptr,
            /* pName */ "main",
            /* pSpecializationInfo */ nullptr,
        },
    };

    /* ************************************* */
    /*        vertex input initialize        */
    /* ************************************* */
    VkVertexInputBindingDescription vertex_input_bind_description = {
        /* binding */ 0,
        /* stride */ sizeof(float) * 3,
        /* inputRate */ VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription vertex_input_attribute_descriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 0,
        }
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_stage_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ no_flag_bits,
        /* vertexBindingDescriptionCount */ 1,
        /* pVertexBindingDescriptions */ &vertex_input_bind_description,
        /* vertexAttributeDescriptionCount */ ARRAY_SIZE(vertex_input_attribute_descriptions),
        /* pVertexAttributeDescriptions */ vertex_input_attribute_descriptions
    };

    /* ************************************ */
    /*        pipeline input assembly       */
    /* ************************************ */
    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ no_flag_bits,
        /* topology */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        /* primitiveRestartEnable */ VK_FALSE,
    };

    /* ********************** */
    /*        viewport        */
    /* ********************** */
    VkViewport viewport = {
        /* x */ 0,
        /* y */ 0,
        /* width */ 800.0f,
        /* height */ 600.0f,
        /* minDepth */ 0.0f,
        /* maxDepth */ 1.0f
    };

    VkRect2D scissor = {
        {
            .x = 0,
            .y = 0
        },
        {
            .width = 800,
            .height = 600
        }
    };

    VkPipelineViewportStateCreateInfo viewport_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ no_flag_bits,
        /* viewportCount */ 1,
        /* pViewports */ &viewport,
        /* scissorCount */ 1,
        /* pScissors */ &scissor,
    };

    /* *************************** */
    /*        rasterization        */
    /* *************************** */
    VkPipelineRasterizationStateCreateInfo rasterization_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ no_flag_bits,
        /* depthClampEnable */ VK_FALSE,
        /* rasterizerDiscardEnable */ VK_FALSE,
        /* polygonMode */ VK_POLYGON_MODE_FILL,
        /* cullMode */ VK_CULL_MODE_BACK_BIT,
        /* frontFace */ VK_FRONT_FACE_COUNTER_CLOCKWISE,
        /* depthBiasEnable */ VK_FALSE,
        /* depthBiasConstantFactor */ 0.0f,
        /* depthBiasClamp */ 0.0f,
        /* depthBiasSlopeFactor */ 0.0f,
        /* lineWidth */ 1.0f
    };

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        /* sType */ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        /* pNext */ nextptr,
        /* flags */ no_flag_bits,
        /* stageCount */ ARRAY_SIZE(shader_stages),
        /* pStages */ shader_stages,
        /* pVertexInputState */ &vertex_input_stage_create_info,
        /* pInputAssemblyState */ &input_assembly_create_info,
        /* pTessellationState */ nullptr,
        /* pViewportState */ &viewport_create_info,
        /* pRasterizationState */ &rasterization_create_info,
        /* pMultisampleState */
        /* pDepthStencilState */
        /* pColorBlendState */
        /* pDynamicState */
        /* layout */
        /* renderPass */
        /* subpass */
        /* basePipelineHandle */
        /* basePipelineIndex */
    };

    VkPipeline pipeline;
    err = vkCreateGraphicsPipelines(device, nullptr, 1, &pipeline_create_info, allocation_callbacks, &pipeline);
    assert(!err);
}