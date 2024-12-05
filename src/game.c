#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vulkan/vulkan.h>

#define SYN_MATH_IMPLEMENTATION
#include "math/syn_math.h"
#include "vulkan.h"
#include "utils.h"

#include "game.h"

VkPipelineLayout
vulkan_create_pipeline_layout(const Vulkan* vulkan,
                              VkDescriptorSetLayout descriptor_set_layout)
{
#if 1
    VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .size = sizeof(M4),
    };

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &push_constant_range,
    };
#else
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptor_set_layout,
    };
#endif

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    vkCreatePipelineLayout(vulkan->device, &pipeline_layout_create_info, NULL,
                           &pipeline_layout);

    return pipeline_layout;
}

VkPipeline vulkan_create_graphic_pipeline(const Vulkan* vulkan,
                                          VkPipelineLayout pipeline_layout,
                                          const char* vertex_shader_file_name,
                                          const char* fragment_shader_file_name,
                                          uint32_t width, uint32_t height)
{
    FileData vertex_shader_file = utils_read_file(vertex_shader_file_name);
    FileData fragment_shader_file = utils_read_file(fragment_shader_file_name);

    VkShaderModuleCreateInfo vertex_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = (size_t)vertex_shader_file.size,
        .pCode = (uint32_t*)vertex_shader_file.data,
    };

    VkShaderModuleCreateInfo fragment_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = (size_t)fragment_shader_file.size,
        .pCode = (uint32_t*)fragment_shader_file.data,
    };

    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;
    VK_ASSERT(vkCreateShaderModule(vulkan->device, &vertex_shader_info, NULL,
                                   &vertex_shader_module));
    VK_ASSERT(vkCreateShaderModule(vulkan->device, &fragment_shader_info, NULL,
                                   &fragment_shader_module));

    free(vertex_shader_file.data);
    free(fragment_shader_file.data);

    VkPipelineShaderStageCreateInfo shader_stage_info[2] = { 0 };
    shader_stage_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stage_info[0].module = vertex_shader_module;
    shader_stage_info[0].pName = "main";

    shader_stage_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stage_info[1].module = fragment_shader_module;
    shader_stage_info[1].pName = "main";

    VkVertexInputBindingDescription input_binding_desc = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription input_attribut_descs[2] = { 0 };
    input_attribut_descs[0].location = 0;
    input_attribut_descs[0].binding = 0;
    input_attribut_descs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    input_attribut_descs[0].offset = offsetof(Vertex, color);

    input_attribut_descs[1].location = 1;
    input_attribut_descs[1].binding = 0;
    input_attribut_descs[1].format = VK_FORMAT_R32G32_SFLOAT;
    input_attribut_descs[1].offset = offsetof(Vertex, position);

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &input_binding_desc,
        .vertexAttributeDescriptionCount = ARRAY_SIZE(input_attribut_descs),
        .pVertexAttributeDescriptions = input_attribut_descs,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkViewport view_port = {
        .width = (float)width,
        .height = (float)height,
    };
    VkRect2D scissor = { 
        .extent = {
            .width = width,
            .height = height,
        }, 
    };
    VkPipelineViewportStateCreateInfo viewport_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &view_port,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_TRUE,
        .depthBiasConstantFactor = 1.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 1.0f,
        .lineWidth = 1.0f,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    };

    VkPipelineColorBlendAttachmentState color_blend_attach = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_TRUE,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attach,
    };

    VkPipelineMultisampleStateCreateInfo multi_sample_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stage_info,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_create_info,
        .pRasterizationState = &rasterization_create_info,
        .pMultisampleState = &multi_sample_state_create_info,
        .pDepthStencilState = &depth_stencil_create_info,
        .pColorBlendState = &color_blend_info,
        .layout = pipeline_layout,
        .renderPass = vulkan->render_pass,
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateGraphicsPipelines(vulkan->device, VK_NULL_HANDLE, 1,
                                        &pipeline_create_info, NULL, &pipeline));

    vkDestroyShaderModule(vulkan->device, vertex_shader_module, NULL);
    vkDestroyShaderModule(vulkan->device, fragment_shader_module, NULL);

    return pipeline;
}

Buffer vulkan_create_buffer(const Vulkan* vulkan, VkBufferUsageFlags usage,
                            uint32_t size_bytes)
{

    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size_bytes,
        .usage = usage,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &vulkan->graphic_index
    };
    VkBuffer buffer = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateBuffer(vulkan->device, &buffer_create_info, NULL, &buffer));

    VkPhysicalDeviceMemoryProperties memory_properties = { 0 };
    vkGetPhysicalDeviceMemoryProperties(vulkan->physical_device, &memory_properties);

    VkMemoryPropertyFlags property_flags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    uint32_t index = 0;
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if ((memory_properties.memoryTypes[i].propertyFlags & property_flags) ==
            property_flags)
        {
            index = i;
            break;
        }
    }

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(vulkan->device, buffer, &memory_requirements);

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = index,
    };
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VK_ASSERT(vkAllocateMemory(vulkan->device, &allocate_info, NULL, &memory));

    vkBindBufferMemory(vulkan->device, buffer, memory, 0);

    Buffer result = {
        .buffer = buffer,
        .size_in_bytes = size_bytes,
        .memory = memory,
    };
    return result;
}

Buffer vulkan_create_vertex_buffer(const Vulkan* vulkan, uint32_t size)
{
    VkBufferUsageFlagBits usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    uint32_t size_in_bytes = size * sizeof(Vertex);
    Buffer buffer = vulkan_create_buffer(vulkan, usage, size_in_bytes);

    vkMapMemory(vulkan->device, buffer.memory, 0, size_in_bytes, 0,
                &buffer.mapped_data);
    return buffer;
}

Buffer vulkan_create_index_buffer(const Vulkan* vulkan, uint32_t size)
{
    VkBufferUsageFlagBits usage =
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    uint32_t size_in_bytes = size * sizeof(uint32_t);
    Buffer buffer = vulkan_create_buffer(vulkan, usage, size_in_bytes);

    vkMapMemory(vulkan->device, buffer.memory, 0, size_in_bytes, 0,
                &buffer.mapped_data);
    return buffer;
}

Buffer vulkan_create_uniform_buffer(const Vulkan* vulkan)
{
    VkBufferUsageFlagBits usage =
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    uint32_t size_in_bytes = sizeof(VP);
    Buffer buffer = vulkan_create_buffer(vulkan, usage, size_in_bytes);

    vkMapMemory(vulkan->device, buffer.memory, 0, size_in_bytes, 0,
                &buffer.mapped_data);
    return buffer;
}

void vertex_buffer_copy_data(VertexBuffer* buffer)
{
    memcpy(buffer->buffer.mapped_data, buffer->array.data,
           buffer->array.size * sizeof(Vertex));
}

void index_buffer_copy_data(IndexBuffer* buffer)
{
    memcpy(buffer->buffer.mapped_data, buffer->array.data,
           buffer->array.size * sizeof(uint32_t));
}

VkDescriptorSetLayout vulkan_create_descriptor_set_layout(const Vulkan* vulkan)
{
    VkDescriptorSetLayoutBinding set_layout_binding = {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutCreateInfo desc_set_layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &set_layout_binding,
    };

    VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateDescriptorSetLayout(vulkan->device, &desc_set_layout_info,
                                          NULL, &descriptor_set_layout));
    return descriptor_set_layout;
}

CreateDescriptorSetReturn
vulkan_create_descriptor_set(const Vulkan* vulkan, VkDescriptorSetLayout set_layout,
                             const Buffer* uniform_buffer)
{
    VkDescriptorPoolSize pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };

    VkDescriptorPoolCreateInfo pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &pool_size
    };

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateDescriptorPool(vulkan->device, &pool_create_info, NULL,
                                     &descriptor_pool));

    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo set_allocation_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &set_layout,
    };
    vkAllocateDescriptorSets(vulkan->device, &set_allocation_info, &descriptor_set);

    VkDescriptorBufferInfo buffer_info = {
        .buffer = uniform_buffer->buffer,
        .offset = 0,
        .range = sizeof(VP),
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &buffer_info,
    };
    vkUpdateDescriptorSets(vulkan->device, 1, &write_descriptor_set, 0, NULL);

    CreateDescriptorSetReturn result = {
        .pool = descriptor_pool,
        .set = descriptor_set,
    };
    return result;
}

void render_quad(VertexArray* array, V2 position, V2 size, V4 color)
{
    Vertex vertex = {
        .color = color,
        .position = position,
    };
    array_append(array, vertex);
    vertex.position.x += size.x;
    array_append(array, vertex);
    vertex.position.y += size.y;
    array_append(array, vertex);
    vertex.position.x -= size.x;
    array_append(array, vertex);
}

const uint32_t INDICES_TABLE[] = { 0, 1, 2, 2, 3, 0 };
void render_quad_indices(IndexArray* array, uint32_t offset)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(INDICES_TABLE); ++i)
    {
        array_append(array, INDICES_TABLE[i] + offset);
    }
}

void render_quad_full(VertexArray* vertices, IndexArray* indices, V2 position,
                      V2 size, V4 color)
{
    render_quad_indices(indices, vertices->size);
    render_quad(vertices, position, size, color);
}

#include "tile_map.h"

V2 update_character_velocity(const bool* keys, V2 old_velocity, float delta_time)
{
    V2 acceleration = v2d();
    float speed = 2500.0f;
    if (keys[SYNC_KEY_A])
    {
        acceleration.x = -1.0f;
    }
    if (keys[SYNC_KEY_W])
    {
        acceleration.y = 1.0f;
    }
    if (keys[SYNC_KEY_D])
    {
        acceleration.x = 1.0f;
    }
    if (keys[SYNC_KEY_S])
    {
        acceleration.y = -1.0f;
    }
    float len = v2_len(acceleration);
    if(len > 0.1f)
    {
        acceleration = v2_normalize_len(acceleration, len);
    }
    return v2_add(v2_s_multi(v2_s_multi(acceleration, speed), delta_time),
                  old_velocity);
}

V2 update_character_position(V2 old_position, V2 velocity, float delta_time)
{
    return v2_add(v2_s_multi(velocity, delta_time), old_position);
}

GAME_UPDATE_AND_RENDER(game_update_and_render)
{
    if (!game_state->initialized)
    {
        game_state->uniform_buffer = vulkan_create_uniform_buffer(vulkan);
        game_state->descriptor_set_layout =
            vulkan_create_descriptor_set_layout(vulkan);
        CreateDescriptorSetReturn descriptor_set_return =
            vulkan_create_descriptor_set(vulkan, game_state->descriptor_set_layout,
                                         &game_state->uniform_buffer);
        game_state->descriptor_pool = descriptor_set_return.pool;
        game_state->descriptor_set = descriptor_set_return.set;

        game_state->vertices = &game_state->vertex_buffer.array;
        game_state->indices = &game_state->index_buffer.array;
        array_create(game_state->vertices, KILOBYTE(50) * 4);
        array_create(game_state->indices, KILOBYTE(50) * 6);

        V2 position = v2f((width - 30.0f) * 0.5f, (height - 30.0f) * 0.5f);
        render_quad_full(game_state->vertices, game_state->indices, position,
                         v2f(60.0f, 60.0f), v4ic(1.0f));

        const float tile_size = 60.0f;
        for (int32_t i = ARRAY_SIZE(tile_map) - 1; i >= 0; --i)
        {
            for (uint32_t j = 0; j < ARRAY_SIZE(tile_map[0]); ++j)
            {
                V4 color = v4f(0.0f, 0.5f, 0.5f, 1.0f);
                if (tile_map[i][j])
                {
                    color = v4f(0.5f, 0.5f, 0.0f, 1.0f);
                }
                render_quad_full(game_state->vertices, game_state->indices,
                                 v2f(j * tile_size, i * tile_size), v2i(tile_size),
                                 color);
            }
        }

        game_state->vertex_buffer.buffer = vulkan_create_vertex_buffer(
            vulkan, game_state->vertex_buffer.array.capacity);
        vertex_buffer_copy_data(&game_state->vertex_buffer);

        game_state->index_buffer.buffer = vulkan_create_index_buffer(
            vulkan, game_state->index_buffer.array.capacity);
        index_buffer_copy_data(&game_state->index_buffer);

        game_state->pipeline_layout =
            vulkan_create_pipeline_layout(vulkan, game_state->descriptor_set_layout);
        game_state->graphic_pipeline = vulkan_create_graphic_pipeline(
            vulkan, game_state->pipeline_layout, "res/shaders/spv/shader.vert.spv",
            "res/shaders/spv/shader.frag.spv", width, height);

        game_state->position = v2d();
        game_state->cam_position = v2d();

        game_state->initialized = true;
    }

    game_state->velocity = update_character_velocity(
        game_state->keys, game_state->velocity, (float)delta_time);

    game_state->velocity.x -= 8.0f * game_state->velocity.x * (float)delta_time;
    game_state->velocity.y -= 8.0f * game_state->velocity.y * (float)delta_time;

    game_state->position = update_character_position(
        game_state->position, game_state->velocity, (float)delta_time);

    V2 direction = v2d();
    V2 neg_cam_position = v2_neg(game_state->cam_position);
    float distance = v2_distance(neg_cam_position, game_state->position);
    if (distance > 5.0f)
    {
        direction = v2_normalize(v2_sub(game_state->position, neg_cam_position));
        float speed = distance * 3.7f;
        direction = v2_s_multi(direction, speed);
    }
    v2_sub_equal(&game_state->cam_position,
                 v2_s_multi(direction, (float)delta_time));

    M4 model = m4_translate(v3_v2(game_state->position));

    VP vp = { 0 };
    vp.projection = ortho(0, (float)width, (float)height, 0, -1.0f, 1.0f);
    vp.view = m4_translate(v3_v2(game_state->cam_position));

    memcpy(game_state->uniform_buffer.mapped_data, &vp, sizeof(vp));

    vkCmdBindPipeline(vulkan->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      game_state->graphic_pipeline);

    vkCmdBindDescriptorSets(vulkan->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            game_state->pipeline_layout, 0, 1,
                            &game_state->descriptor_set, 0, NULL);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(vulkan->command_buffer, 0, 1,
                           &game_state->vertex_buffer.buffer.buffer, &offset);
    vkCmdBindIndexBuffer(vulkan->command_buffer,
                         game_state->index_buffer.buffer.buffer, 0,
                         VK_INDEX_TYPE_UINT32);

    M4 static_model = m4d();
    vkCmdPushConstants(vulkan->command_buffer, game_state->pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(static_model),
                       &static_model);

    vkCmdDrawIndexed(vulkan->command_buffer, game_state->index_buffer.array.size - 6,
                     1, 6, 0, 0);

    vkCmdPushConstants(vulkan->command_buffer, game_state->pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), &model);

    vkCmdDrawIndexed(vulkan->command_buffer, 6, 1, 0, 0, 0);
}

#ifdef SYNCOP_UNIT_BUILD
#include "utils.c"
#endif
