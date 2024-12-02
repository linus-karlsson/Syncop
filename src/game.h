#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include "utils.h"
#include "vulkan.h"
#include <stdbool.h>
#endif

typedef struct GameState
{
    bool initialized;

    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;
    Buffer uniform_buffer;

    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_set;

    VkPipelineLayout pipeline_layout;
    VkPipeline graphic_pipeline;

    VP vp;
    V3 position;
    M4 model;

} GameState;

#define GAME_UPDATE_AND_RENDER(name) void name(GameState* game_state, const Vulkan* vulkan, double delta_time, uint32_t width, uint32_t height)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);
GAME_UPDATE_AND_RENDER(game_update_and_render_stub)
{
}
