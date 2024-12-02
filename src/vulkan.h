#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include <stdint.h>
#include <vulkan/vulkan.h>
#include "sy_windows.h"
#include "math/syn_math.h"
#endif

typedef struct Vertex
{
    V4 color;
    V2 position;
} Vertex;

typedef struct ViewProjection
{
    M4 view;
    M4 projection;
} VP;

typedef struct VertexArray
{
    uint32_t size;
    uint32_t capacity;
    Vertex* data;
} VertexArray;

typedef struct U32Array
{
    uint32_t size;
    uint32_t capacity;
    uint32_t* data;
} U32Array, IndexArray;

typedef struct Buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize size_in_bytes;
    void* mapped_data;
} Buffer;

typedef struct VertexBuffer
{
    Buffer buffer;
    VertexArray array;
} VertexBuffer;

typedef struct IndexBuffer
{
    Buffer buffer;
    IndexArray array;
} IndexBuffer;

typedef struct Vulkan
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkDevice device;
    VkPhysicalDevice physical_device;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;

    VkFormat image_format;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkSampleCountFlagBits sample_count;

    uint32_t swapchain_image_count;
    VkImage* swapchain_images;
    VkImageView* swapchain_image_views;
    VkFramebuffer* frame_buffers;
    VkRenderPass render_pass;

    VkFence fence;
    VkSemaphore wait_semaphore;
    VkSemaphore signal_semaphore;

    uint32_t graphic_index;
} Vulkan;

typedef struct PhysicalDeviceReturnValues
{
    VkPhysicalDevice physical_device;
    uint32_t graphic_index;
} PhysicalDeviceReturnValues;

typedef struct SwapchainImageReturn
{
    uint32_t count;
    VkImage* images;
} SwapchainImageReturn;

typedef struct CreateDescriptorSetReturn
{
    VkDescriptorPool pool;
    VkDescriptorSet set;
} CreateDescriptorSetReturn;

VkInstance vulkan_create_instance(void);

VkDebugUtilsMessengerEXT vulkan_initialize_debug_messages(const Vulkan* vulkan);

PhysicalDeviceReturnValues vulkan_get_physical_device(const Vulkan* vulkan);
VkDevice vulkan_create_device(const Vulkan* vulkan);

VkCommandPool vulkan_creata_command_pool(const Vulkan* vulkan);
VkCommandBuffer vulkan_allocate_command_buffer(const Vulkan* vulkan);

VkFence vulkan_create_fence(const Vulkan* vulkan);
VkSemaphore vulkan_create_semaphore(const Vulkan* vulkan);

VkSurfaceKHR vulkan_create_surface(const Vulkan* vulkan, void* instance, void* window);
VkSurfaceFormatKHR vulkan_get_surface_format(const Vulkan* vulkan);
VkSwapchainKHR vulkan_create_swapchain(const Vulkan* vulkan, VkColorSpaceKHR color_space);

SwapchainImageReturn vulkan_get_swapchain_images(const Vulkan* vulkan);

VkImageView vulkan_create_image_view(const Vulkan* vulkan, VkImage image);

VkRenderPass vulkan_create_render_pass(const Vulkan* vulkan);
VkFramebuffer vulkan_create_frame_buffer(const Vulkan* vulkan, VkImageView image_view, uint32_t width, uint32_t height);

VkPipelineLayout vulkan_create_pipeline_layout(const Vulkan* vulkan, VkDescriptorSetLayout descriptor_set_layout);
VkPipeline vulkan_create_graphic_pipeline(const Vulkan* vulkan, VkPipelineLayout pipeline_layout, const char* vertex_shader_file_name, const char* fragment_shader_file_name, uint32_t width, uint32_t height);

Buffer vulkan_create_buffer(const Vulkan* vulkan, VkBufferUsageFlags usage, uint32_t size_bytes);
Buffer vulkan_create_vertex_buffer(const Vulkan* vulkan, const VertexArray* array);
Buffer vulkan_create_index_buffer(const Vulkan* vulkan, const IndexArray* array);
Buffer vulkan_create_uniform_buffer(const Vulkan* vulkan);
void vertex_buffer_copy_data(VertexBuffer* buffer);
void index_buffer_copy_data(IndexBuffer* buffer);

VkDescriptorSetLayout vulkan_create_descriptor_set_layout(const Vulkan* vulkan);
CreateDescriptorSetReturn vulkan_create_descriptor_set(const Vulkan* vulkan, VkDescriptorSetLayout set_layout, const Buffer* uniform_buffer);
