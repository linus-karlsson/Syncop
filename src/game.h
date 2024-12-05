#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include "utils.h"
#include "vulkan.h"
#include <stdbool.h>
#endif

#include "event.h"

typedef struct KeyEvent
{
    uint16_t key;
    uint16_t action;
    bool ctrl_pressed;
    bool alt_pressed;
    bool shift_pressed;
    bool activated;
} KeyEvent;

typedef struct GameState
{
    bool initialized;
   
    KeyEvent key_event;

    bool keys[KEY_BUFFER_CAPACITY];

    VertexBuffer vertex_buffer;
    VertexArray* vertices;
    IndexBuffer index_buffer;
    IndexArray* indices;
    Buffer uniform_buffer;

    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_set;

    VkPipelineLayout pipeline_layout;
    VkPipeline graphic_pipeline;

    V2 position;
    V2 velocity;
    V2 acceleration; 

    V2 cam_position;

} GameState;

#define GAME_UPDATE_AND_RENDER(name) void name(GameState* game_state, const Vulkan* vulkan, double delta_time, uint32_t width, uint32_t height)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);
GAME_UPDATE_AND_RENDER(game_update_and_render_stub)
{
}
