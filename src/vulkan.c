#include "vulkan.h"

VkCommandBuffer vulkan_allocate_command_buffer(const Vulkan* vulkan)
{
    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vulkan->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VK_ASSERT(
        vkAllocateCommandBuffers(vulkan->device, &allocate_info, &command_buffer));
    return command_buffer;
}

VkImageView vulkan_create_image_view(const Vulkan* vulkan, VkImage image, VkFormat image_format)
{
    VkComponentMapping component_mapping = {
        .r = VK_COMPONENT_SWIZZLE_R,
        .g = VK_COMPONENT_SWIZZLE_G,
        .b = VK_COMPONENT_SWIZZLE_B,
        .a = VK_COMPONENT_SWIZZLE_A,
    };

    VkImageSubresourceRange sub_range = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };

    VkImageViewCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image_format,
        .components = component_mapping,
        .subresourceRange = sub_range,
    };

    VkImageView image_view = VK_NULL_HANDLE;
    vkCreateImageView(vulkan->device, &create_info, NULL, &image_view);
    return image_view;
}
