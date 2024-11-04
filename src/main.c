#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define global static
#define presist static
#define internal static

#define ASSERT(expression)                                                                         \
    if (!(expression)) (*(uint32_t*)0 = 0)
#define VK_ASSERT(function)                                                                        \
    {                                                                                              \
        ASSERT((function) == VK_SUCCESS);                                                          \
    }
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

global bool running = false;

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_CLOSE:
        {
            running = false;
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
    char* formatted_error_message = calloc(formatted_error_message_length, sizeof(char));
    sprintf_s(formatted_error_message, formatted_error_message_length, "%s\n", message);

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

internal VKAPI_ATTR VkBool32 VKAPI_CALL
debug_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                       VkDebugUtilsMessageTypeFlagsEXT message_type,
                       const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
    if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        char* formatted_error_message = debug_format_error_message(callback_data->pMessage);
        OutputDebugString("ERROR: ");
        OutputDebugString(formatted_error_message);
        free(formatted_error_message);
        ASSERT(false);
    }
    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        char* formatted_warning_message = debug_format_error_message(callback_data->pMessage);
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
            output_debug_string("ERROR: Could not initialize debug messages callback");
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
    VK_ASSERT(vkEnumeratePhysicalDevices(vulkan->instance, &physical_device_count, NULL));
    VkPhysicalDevice* physical_devices = calloc(physical_device_count, sizeof(VkPhysicalDevice));
    VK_ASSERT(
        vkEnumeratePhysicalDevices(vulkan->instance, &physical_device_count, physical_devices));

    VkPhysicalDevice physical_device = physical_devices[0];
    uint32_t graphic_index = 0;

    output_debug_string("Physical devices: \n");
    for (uint32_t index = 0; index < physical_device_count; ++index)
    {
        VkPhysicalDeviceProperties physical_device_properties = { 0 };
        vkGetPhysicalDeviceProperties(physical_devices[index], &physical_device_properties);
        output_debug_string("\t%s\n", physical_device_properties.deviceName);

        uint32_t queue_family_properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[index],
                                                 &queue_family_properties_count, NULL);
        VkQueueFamilyProperties* family_properties =
            calloc(queue_family_properties_count, sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[index],
                                                 &queue_family_properties_count, family_properties);

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
    output_debug_string("\tPicked device: %s\n", physical_device_properties.deviceName);

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
    VK_ASSERT(vkCreateDevice(vulkan->physical_device, &device_create_info, NULL, &device));
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
    VK_ASSERT(vkCreateCommandPool(vulkan->device, &create_info, NULL, &command_pool));
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
    VK_ASSERT(vkAllocateCommandBuffers(vulkan->device, &allocate_info, &command_buffer));
    return command_buffer;
}

VkFence vulkan_create_fence(const Vulkan* vulkan)
{
    VkFenceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
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


VkSurfaceKHR vulkan_create_surface(const Vulkan* vulkan, HINSTANCE instance, HWND window)
{
    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = instance,
        .hwnd = window,
    };
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateWin32SurfaceKHR(vulkan->instance, &create_info, NULL, &surface));
    return surface;
}

VkSurfaceFormatKHR vulkan_get_surface_format(const Vulkan* vulkan)
{

    uint32_t surface_formats_count = 0;
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan->physical_device, vulkan->surface,
                                                   &surface_formats_count, NULL));
    VkSurfaceFormatKHR* surface_formats = calloc(surface_formats_count, sizeof(VkSurfaceFormatKHR));
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan->physical_device, vulkan->surface,
                                                   &surface_formats_count, surface_formats));

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

VkSwapchainKHR vulkan_create_swapchain(const Vulkan* vulkan, VkColorSpaceKHR color_space)
{
    VkSurfaceCapabilitiesKHR surface_capabilities = { 0 };
    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan->physical_device, vulkan->surface,
                                                        &surface_capabilities));

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

VkRenderPass vulkan_create_render_pass(const Vulkan* vulkan)
{
    VkAttachmentDescription color_attachment = {
        .format = vulkan->image_format,
        .samples = vulkan->sample_count, 
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

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
        .attachmentCount = 1,
        .pAttachments = &color_attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
    };
    VkRenderPass render_pass = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateRenderPass(vulkan->device, &create_info, NULL, &render_pass));
    return render_pass;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    WNDCLASS window_class = {
        .lpfnWndProc = window_procedure,
        .hInstance = instance,
        .lpszClassName = "Mutha class",
    };
    RegisterClass(&window_class);

    HWND window = CreateWindowEx(0, window_class.lpszClassName, "Mutha Window", WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
                                 NULL, instance, NULL);

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
        vulkan.swapchain = vulkan_create_swapchain(&vulkan, surface_format.colorSpace);
        vulkan.sample_count = VK_SAMPLE_COUNT_1_BIT;
        vulkan.render_pass = vulkan_create_render_pass(&vulkan);

        const double target_time = 0.016;
        double delta_time = target_time;
        double last_time = get_time();

        running = true;
        while (running)
        {
            MSG message;
            while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }

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
