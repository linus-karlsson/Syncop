#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#if 0
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include "sy_windows.h"
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include "utils.h"

#ifdef SYNCOP_UNIT_BUILD
#define SYN_MATH_IMPLEMENTATION
#endif
#include "math/syn_math.h"

#define global static
#define presist static
#define internal static

#define ASSERT(expression)                                                          \
    if (!(expression)) (*(uint32_t*)0 = 0)
#define VK_ASSERT(function)                                                         \
    {                                                                               \
        ASSERT((function) == VK_SUCCESS);                                           \
    }
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

typedef struct Vertex
{
    V4 color;
    V2 position;
} Vertex;

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

global bool g_running;
global uint32_t g_width;
global uint32_t g_height;

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param,
                                  LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_CLOSE:
        {
            g_running = false;
            break;
        }
        case WM_SIZE:
        {
            g_width = LOWORD(l_param);
            g_height = HIWORD(l_param);
            break;
        }
        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        }
    }

    return result;
}

double get_time(void)
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return now.tv_sec + (now.tv_nsec * 0.000000001);
}

void output_debug_string(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char buffer[1024];

    vsnprintf(buffer, sizeof(buffer), format, args);

    OutputDebugString(buffer);

    va_end(args);
}

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

char* debug_format_error_message(const char* message)
{
    const size_t message_length = strlen(message);
    const size_t new_line_extra = 2;
    const size_t formatted_error_message_length = message_length + new_line_extra;
    char* formatted_error_message =
        calloc(formatted_error_message_length, sizeof(char));
    sprintf_s(formatted_error_message, formatted_error_message_length, "%s\n",
              message);

    for (size_t index = 80; index < message_length; index += 80)
    {
        for (size_t s = index; s < message_length; s++)
        {
            if (formatted_error_message[s] == ' ')
            {
                formatted_error_message[s] = '\n';
                break;
            }
        }
    }
    return formatted_error_message;
}

internal VKAPI_ATTR VkBool32 VKAPI_CALL debug_message_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
    if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        char* formatted_error_message =
            debug_format_error_message(callback_data->pMessage);
        OutputDebugString("ERROR: ");
        OutputDebugString(formatted_error_message);
        free(formatted_error_message);
        ASSERT(false);
    }
    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        char* formatted_warning_message =
            debug_format_error_message(callback_data->pMessage);
        OutputDebugString("WARNING: ");
        OutputDebugString(formatted_warning_message);
        free(formatted_warning_message);
    }

    return VK_TRUE;
}

VkDebugUtilsMessengerCreateInfoEXT debug_get_create_info(void)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_message_callback,
    };
    return create_info;
}

VkInstance vulkan_create_instance(void)
{
    uint32_t version_supported = 0;
    vkEnumerateInstanceVersion(&version_supported);

    const uint32_t major = VK_API_VERSION_MAJOR(version_supported);
    const uint32_t minor = VK_API_VERSION_MINOR(version_supported);
    const uint32_t patch = VK_API_VERSION_PATCH(version_supported);
    output_debug_string("Vulkan version supported: %u_%u_%u\n", major, minor, patch);

    const VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Syncop",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 1),
        .pEngineName = "Syncop",
        .engineVersion = VK_MAKE_VERSION(0, 1, 1),
        .apiVersion = version_supported,
    };
    const char* extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };

    const char* validations[] = { "VK_LAYER_KHRONOS_validation" };
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = debug_get_create_info();

    const VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debug_create_info,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validations,
        .enabledExtensionCount = ARRAY_SIZE(extensions),
        .ppEnabledExtensionNames = extensions,
    };

    VkInstance instance = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateInstance(&create_info, NULL, &instance));
    return instance;
}

VkDebugUtilsMessengerEXT vulkan_initialize_debug_messages(const Vulkan* vulkan)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info = debug_get_create_info();

    PFN_vkCreateDebugUtilsMessengerEXT callback =
        (PFN_vkCreateDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
            vulkan->instance, "vkCreateDebugUtilsMessengerEXT"));

    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    if (callback)
    {
        if (callback(vulkan->instance, &create_info, NULL, &debug_messenger))
        {
            output_debug_string(
                "ERROR: Could not initialize debug messages callback");
        }
    }
    else
    {
        output_debug_string("ERROR: Debug messages extension is not present");
    }
    return debug_messenger;
}

typedef struct PhysicalDeviceReturnValues
{
    VkPhysicalDevice physical_device;
    uint32_t graphic_index;
} PhysicalDeviceReturnValues;
PhysicalDeviceReturnValues vulkan_get_physical_device(const Vulkan* vulkan)
{
    uint32_t physical_device_count = 0;
    VK_ASSERT(
        vkEnumeratePhysicalDevices(vulkan->instance, &physical_device_count, NULL));
    VkPhysicalDevice* physical_devices =
        calloc(physical_device_count, sizeof(VkPhysicalDevice));
    VK_ASSERT(vkEnumeratePhysicalDevices(vulkan->instance, &physical_device_count,
                                         physical_devices));

    VkPhysicalDevice physical_device = physical_devices[0];
    uint32_t graphic_index = 0;

    output_debug_string("Physical devices: \n");
    for (uint32_t index = 0; index < physical_device_count; ++index)
    {
        VkPhysicalDeviceProperties physical_device_properties = { 0 };
        vkGetPhysicalDeviceProperties(physical_devices[index],
                                      &physical_device_properties);
        output_debug_string("\t%s\n", physical_device_properties.deviceName);

        uint32_t queue_family_properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_devices[index], &queue_family_properties_count, NULL);
        VkQueueFamilyProperties* family_properties =
            calloc(queue_family_properties_count, sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[index],
                                                 &queue_family_properties_count,
                                                 family_properties);

        bool valid_physical_devive = false;
        for (uint32_t family_index = 0; family_index < queue_family_properties_count;
             ++family_index)
        {
            if (family_properties[family_index].queueCount > 0 &&
                (family_properties[family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                graphic_index = family_index;
                valid_physical_devive = true;
                break;
            }
        }
        free(family_properties);
        if (valid_physical_devive)
        {
            physical_device = physical_devices[index];
        }
    }
    VkPhysicalDeviceProperties physical_device_properties = { 0 };
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    output_debug_string("\tPicked device: %s\n",
                        physical_device_properties.deviceName);

    free(physical_devices);
    return (PhysicalDeviceReturnValues){
        .physical_device = physical_device,
        .graphic_index = graphic_index,
    };
}

VkDevice vulkan_create_device(const Vulkan* vulkan)
{
    const float queue_priorities = 1.0f;
    const VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = vulkan->graphic_index,
        .queueCount = 1,
        .pQueuePriorities = &queue_priorities,
    };

    const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledExtensionCount = ARRAY_SIZE(extensions),
        .ppEnabledExtensionNames = extensions,
    };
    VkDevice device = VK_NULL_HANDLE;
    VK_ASSERT(
        vkCreateDevice(vulkan->physical_device, &device_create_info, NULL, &device));
    return device;
}

VkCommandPool vulkan_creata_command_pool(const Vulkan* vulkan)
{
    VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vulkan->graphic_index,
    };

    VkCommandPool command_pool = VK_NULL_HANDLE;
    VK_ASSERT(
        vkCreateCommandPool(vulkan->device, &create_info, NULL, &command_pool));
    return command_pool;
}

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

VkFence vulkan_create_fence(const Vulkan* vulkan)
{
    VkFenceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VkFence fence = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateFence(vulkan->device, &create_info, NULL, &fence));
    return fence;
}

VkSemaphore vulkan_create_semaphore(const Vulkan* vulkan)
{
    VkSemaphoreCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateSemaphore(vulkan->device, &create_info, NULL, &semaphore));
    return semaphore;
}

VkSurfaceKHR vulkan_create_surface(const Vulkan* vulkan, HINSTANCE instance,
                                   HWND window)
{
    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = instance,
        .hwnd = window,
    };
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VK_ASSERT(
        vkCreateWin32SurfaceKHR(vulkan->instance, &create_info, NULL, &surface));
    return surface;
}

VkSurfaceFormatKHR vulkan_get_surface_format(const Vulkan* vulkan)
{

    uint32_t surface_formats_count = 0;
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        vulkan->physical_device, vulkan->surface, &surface_formats_count, NULL));
    VkSurfaceFormatKHR* surface_formats =
        calloc(surface_formats_count, sizeof(VkSurfaceFormatKHR));
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        vulkan->physical_device, vulkan->surface, &surface_formats_count,
        surface_formats));

    VkSurfaceFormatKHR surface_format_to_use = surface_formats[0];
    for (uint32_t index = 0; index < surface_formats_count; ++index)
    {
        const VkSurfaceFormatKHR* surface_format = surface_formats + index;
        if (surface_format->format == VK_FORMAT_B8G8R8A8_SRGB &&
            surface_format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surface_format_to_use = *surface_format;
            break;
        }
    }
    free(surface_formats);
    return surface_format_to_use;
}

VkSwapchainKHR vulkan_create_swapchain(const Vulkan* vulkan,
                                       VkColorSpaceKHR color_space)
{
    VkSurfaceCapabilitiesKHR surface_capabilities = { 0 };
    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vulkan->physical_device, vulkan->surface, &surface_capabilities));

    ASSERT(surface_capabilities.maxImageCount > 0);

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vulkan->surface,
        .minImageCount = surface_capabilities.minImageCount + 1,
        .imageFormat = vulkan->image_format,
        .imageColorSpace = color_space,
        .imageExtent = surface_capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &vulkan->graphic_index,
        .preTransform = surface_capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .oldSwapchain = VK_NULL_HANDLE,
    };
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateSwapchainKHR(vulkan->device, &create_info, NULL, &swapchain));
    return swapchain;
}

typedef struct SwapchainImageReturn
{
    uint32_t count;
    VkImage* images;
} SwapchainImageReturn;

SwapchainImageReturn vulkan_get_swapchain_images(const Vulkan* vulkan)
{
    uint32_t image_count = 0;
    vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapchain, &image_count, NULL);
    VkImage* swapchain_images = (VkImage*)calloc(image_count, sizeof(VkImage));
    vkGetSwapchainImagesKHR(vulkan->device, vulkan->swapchain, &image_count,
                            swapchain_images);

    SwapchainImageReturn result = {
        .count = image_count,
        .images = swapchain_images,
    };
    return result;
}

VkImageView vulkan_create_image_view(const Vulkan* vulkan, VkImage image)
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
        .format = vulkan->image_format,
        .components = component_mapping,
        .subresourceRange = sub_range,
    };

    VkImageView image_view = VK_NULL_HANDLE;
    vkCreateImageView(vulkan->device, &create_info, NULL, &image_view);
    return image_view;
}

VkRenderPass vulkan_create_render_pass(const Vulkan* vulkan)
{
    VkAttachmentDescription attachment_descriptions[1] = { 0 };

    VkAttachmentDescription color_attachment = {
        .format = vulkan->image_format,
        .samples = vulkan->sample_count,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    attachment_descriptions[0] = color_attachment;

#if 0
    VkAttachmentDescription resolve_image_desc = {
        .format = vulkan->image_format,
        .samples = vulkan->sample_count,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    attachment_descriptions[1] = resolve_image_desc;
    VkAttachmentReference resolve_reference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
#endif

    VkAttachmentReference color_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_reference,
    };

    VkRenderPassCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = ARRAY_SIZE(attachment_descriptions),
        .pAttachments = attachment_descriptions,
        .subpassCount = 1,
        .pSubpasses = &subpass,
    };
    VkRenderPass render_pass = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateRenderPass(vulkan->device, &create_info, NULL, &render_pass));
    return render_pass;
}

VkFramebuffer vulkan_create_frame_buffer(const Vulkan* vulkan,
                                         VkImageView image_view, uint32_t width,
                                         uint32_t height)
{
    VkFramebufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vulkan->render_pass,
        .attachmentCount = 1,
        .pAttachments = &image_view,
        .width = width,
        .height = height,
        .layers = 1,
    };

    VkFramebuffer frame_buffer = VK_NULL_HANDLE;
    VK_ASSERT(
        vkCreateFramebuffer(vulkan->device, &create_info, NULL, &frame_buffer));
    return frame_buffer;
}

VkPipeline vulkan_create_graphic_pipeline(const Vulkan* vulkan,
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
        .cullMode = VK_CULL_MODE_NONE,
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

#if 0
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
    vkCreateDescriptorSetLayout(vulkan->device, &desc_set_layout_info, NULL,
                                &descriptor_set_layout);

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
    };
#endif

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    vkCreatePipelineLayout(vulkan->device, &pipeline_layout_create_info, NULL,
                           &pipeline_layout);

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

    vkDestroyPipelineLayout(vulkan->device, pipeline_layout, NULL);

#if 0
    vkDestroyDescriptorSetLayout(vulkan->device, descriptor_set_layout, NULL);
#endif
    return pipeline;
}

Buffer vulkan_create_buffer(const Vulkan* vulkan, VkBufferUsageFlags usage,
                            uint32_t size_bytes, void* data)
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

    void* mapped_data = NULL;
    vkMapMemory(vulkan->device, memory, 0, size_bytes, 0, &mapped_data);
    memcpy(mapped_data, data, size_bytes);
    vkUnmapMemory(vulkan->device, memory);

    Buffer result = {
        .buffer = buffer,
        .size_in_bytes = size_bytes,
        .memory = memory,
    };
    return result;
}

Buffer vulkan_create_vertex_buffer(const Vulkan* vulkan, const VertexArray* array)
{
    VkBufferUsageFlagBits usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    return vulkan_create_buffer(vulkan, usage, array->size * sizeof(Vertex),
                                array->data);
}

Buffer vulkan_create_index_buffer(const Vulkan* vulkan, const IndexArray* array)
{
    VkBufferUsageFlagBits usage =
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    return vulkan_create_buffer(vulkan, usage, array->size * sizeof(uint32_t),
                                array->data);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line,
                   int show_cmd)
{
    WNDCLASS window_class = {
        .lpfnWndProc = window_procedure,
        .hInstance = instance,
        .lpszClassName = "Mutha class",
    };
    RegisterClass(&window_class);

    HWND window =
        CreateWindowEx(0, window_class.lpszClassName, "Mutha Window",
                       WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, instance, NULL);

    if (window != NULL)
    {
        ShowWindow(window, show_cmd);

        Vulkan vulkan = { 0 };
        vulkan.instance = vulkan_create_instance();
        vulkan.debug_messenger = vulkan_initialize_debug_messages(&vulkan);
        PhysicalDeviceReturnValues pdrv = vulkan_get_physical_device(&vulkan);
        vulkan.physical_device = pdrv.physical_device;
        vulkan.graphic_index = pdrv.graphic_index;
        vulkan.device = vulkan_create_device(&vulkan);
        vulkan.command_pool = vulkan_creata_command_pool(&vulkan);
        vulkan.command_buffer = vulkan_allocate_command_buffer(&vulkan);
        vulkan.fence = vulkan_create_fence(&vulkan);
        vulkan.wait_semaphore = vulkan_create_semaphore(&vulkan);
        vulkan.signal_semaphore = vulkan_create_semaphore(&vulkan);

        vulkan.surface = vulkan_create_surface(&vulkan, instance, window);
        VkSurfaceFormatKHR surface_format = vulkan_get_surface_format(&vulkan);
        vulkan.image_format = surface_format.format;
        vulkan.swapchain =
            vulkan_create_swapchain(&vulkan, surface_format.colorSpace);
        SwapchainImageReturn swapchain_images = vulkan_get_swapchain_images(&vulkan);
        vulkan.swapchain_images = swapchain_images.images;
        vulkan.swapchain_image_count = swapchain_images.count;

        vulkan.sample_count = VK_SAMPLE_COUNT_1_BIT;
        vulkan.render_pass = vulkan_create_render_pass(&vulkan);

        vulkan.swapchain_image_views =
            (VkImageView*)calloc(vulkan.swapchain_image_count, sizeof(VkImageView));
        vulkan.frame_buffers = (VkFramebuffer*)calloc(vulkan.swapchain_image_count,
                                                      sizeof(VkFramebuffer));
        for (uint32_t i = 0; i < vulkan.swapchain_image_count; ++i)
        {
            vulkan.swapchain_image_views[i] =
                vulkan_create_image_view(&vulkan, vulkan.swapchain_images[i]);
            vulkan.frame_buffers[i] = vulkan_create_frame_buffer(
                &vulkan, vulkan.swapchain_image_views[i], g_width, g_height);
        }

        VertexBuffer vertex_buffer = { 0 };
        array_create(&vertex_buffer.array, 10);

        Vertex vertex = {
            .color = v4i(1.0f),
            .position = v2f(-0.5f, 0.5f),
        };
        array_append(&vertex_buffer.array, vertex);
        vertex.position.x = 0.5f;
        array_append(&vertex_buffer.array, vertex);
        vertex.position.x = 0.0f;
        vertex.position.y = -0.5f;
        array_append(&vertex_buffer.array, vertex);

        vertex_buffer.buffer =
            vulkan_create_vertex_buffer(&vulkan, &vertex_buffer.array);

        IndexBuffer index_buffer = { 0 };
        array_create(&index_buffer.array, 10);
        array_append(&index_buffer.array, 0);
        array_append(&index_buffer.array, 1);
        array_append(&index_buffer.array, 2);

        index_buffer.buffer =
            vulkan_create_index_buffer(&vulkan, &index_buffer.array);

        VkPipeline graphic_pipeline = vulkan_create_graphic_pipeline(
            &vulkan, "res/shaders/spv/shader.vert.spv",
            "res/shaders/spv/shader.frag.spv", g_width, g_height);

        VkQueue graphic_queue = VK_NULL_HANDLE;
        vkGetDeviceQueue(vulkan.device, vulkan.graphic_index, 0, &graphic_queue);

        const double target_time = 0.016;
        double delta_time = target_time;
        double last_time = get_time();

        g_running = true;
        while (g_running)
        {
            MSG message;
            while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }

            vkWaitForFences(vulkan.device, 1, &vulkan.fence, VK_TRUE, UINT64_MAX);

            uint32_t image_index = 0;
            VkResult result = vkAcquireNextImageKHR(
                vulkan.device, vulkan.swapchain, UINT64_MAX, vulkan.wait_semaphore,
                VK_NULL_HANDLE, &image_index);

            ASSERT(result == VK_SUCCESS);

            vkResetFences(vulkan.device, 1, &vulkan.fence);
            vkResetCommandBuffer(vulkan.command_buffer, 0);

            VkCommandBufferBeginInfo command_begin_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            };

            vkBeginCommandBuffer(vulkan.command_buffer, &command_begin_info);

            VkRect2D render_area = {
                .extent = {
                    .width = g_width,
                    .height = g_height,
                },
            };
            VkClearColorValue color_value = {
                .float32[0] = 0.0f,
                .float32[1] = 0.0f,
                .float32[2] = 0.0f,
                .float32[3] = 1.0f,
            };
            VkClearValue clear_value = {
                .color = color_value,
            };
            VkRenderPassBeginInfo render_pass_begin_info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = vulkan.render_pass,
                .framebuffer = vulkan.frame_buffers[image_index],
                .renderArea = render_area,
                .clearValueCount = 1,
                .pClearValues = &clear_value,
            };
            vkCmdBeginRenderPass(vulkan.command_buffer, &render_pass_begin_info, 0);

            vkCmdBindPipeline(vulkan.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              graphic_pipeline);

            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(vulkan.command_buffer, 0, 1,
                                   &vertex_buffer.buffer.buffer, &offset);
            vkCmdBindIndexBuffer(vulkan.command_buffer, index_buffer.buffer.buffer,
                                 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(vulkan.command_buffer, index_buffer.array.size, 1, 0, 0,
                             0);

            vkCmdEndRenderPass(vulkan.command_buffer);

            vkEndCommandBuffer(vulkan.command_buffer);

            VkPipelineStageFlags wait_stage =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo submit_info = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &vulkan.wait_semaphore,
                .pWaitDstStageMask = &wait_stage,
                .commandBufferCount = 1,
                .pCommandBuffers = &vulkan.command_buffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &vulkan.signal_semaphore,
            };
            vkQueueSubmit(graphic_queue, 1, &submit_info, vulkan.fence);

            VkPresentInfoKHR present_info = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &vulkan.signal_semaphore,
                .swapchainCount = 1,
                .pSwapchains = &vulkan.swapchain,
                .pImageIndices = &image_index,
            };
            vkQueuePresentKHR(graphic_queue, &present_info);

            double current_time = get_time();
            delta_time = current_time - last_time;
            if (delta_time < target_time)
            {
                Sleep((DWORD)((target_time - delta_time) * 1000.0));
                current_time = get_time();
                delta_time = current_time - last_time;
            }

            // output_debug_string("Delta: %f\n", delta_time);
            // output_debug_string("Target: %f\n", target_time);

            last_time = current_time;
        }
    }
    else
    {
        // TODO: LOGG
    }
    return 0;
}

#ifdef SYNCOP_UNIT_BUILD
#include "utils.c"
#endif

