/**
 * @file utils.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "application.h"

namespace rt {

inline bool QueueFamilies::IsCompleted() {
  return graphics_family.has_value() && present_family.has_value();
}

inline bool SwapChainSupportDetails::IsAdequate() {
  return capabilities.minImageCount > 0 && !formats.empty() &&
         !present_modes.empty();
}

void Utils::GLFWErrorCallback(const int code, const char* description) {
  std::cerr << "Error::GLFW:\n\tCode: " << code
            << "\n\tDescription: " << description << "\n";
}

void Utils::CheckVulkanResult(const int result, const char* error_msg) {
  if (VK_SUCCESS != result) {
    throw std::runtime_error(error_msg);
  }
}

std::vector<const char*> Utils::QueryVulkanInstanceExts() {
  std::vector<const char*> required_exts{};

  // GLFW required extensions
  uint32_t glfw_ext_cnt = 0;
  const char** glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_ext_cnt);
  for (uint32_t i = 0; i < glfw_ext_cnt; ++i) {
    required_exts.push_back(glfw_exts[i]);
  }

#ifdef __APPLE__
  required_exts.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  required_exts.push_back("VK_KHR_get_physical_device_properties2");
#endif

  required_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  // available instance extensions
  uint32_t available_extension_cnt = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_cnt,
                                         nullptr);
  std::vector<VkExtensionProperties> available_extensions(
      available_extension_cnt);
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_cnt,
                                         available_extensions.data());

  // compare
  std::set<std::string> extensions(required_exts.begin(), required_exts.end());
  for (const auto& extension : available_extensions) {
    extensions.erase(extension.extensionName);
  }

  if (!extensions.empty()) {
    throw std::runtime_error(
        "Error::Vulkan: Error::Vulkan: Find not supported instance "
        "extension(s)!");
  }

  return required_exts;
}

std::vector<const char*> Utils::QueryVulkanLayers() {
  std::vector<const char*> required_layers{};

  // add validation layer
  required_layers.push_back("VK_LAYER_KHRONOS_validation");

  // available instance layers
  uint32_t available_layer_cnt = 0;
  vkEnumerateInstanceLayerProperties(&available_layer_cnt, nullptr);
  std::vector<VkLayerProperties> available_layers(available_layer_cnt);
  vkEnumerateInstanceLayerProperties(&available_layer_cnt,
                                     available_layers.data());

  // compare
  std::set<std::string> layers(required_layers.begin(), required_layers.end());
  for (const auto& layer : available_layers) {
    layers.erase(layer.layerName);
  }

  if (!layers.empty()) {
    throw std::runtime_error("Error::Vulkan: Find not supported layer(s)!");
  }

  return required_layers;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
Utils::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                     VkDebugUtilsMessageTypeFlagsEXT message_type,
                     const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                     void* user_data) {
  if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    std::clog << "----- Validation Layer: "
              << "\n\tSeverity: " << message_severity
              << "\n\tType: " << message_type
              << "\n\tMessage: " << callback_data->pMessage
              << "\n\tUser Data Address: " << user_data << "\n";
  }

  return VK_FALSE;
}

VkResult Utils::CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (func) {
    return func(instance, create_info, allocator, debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void Utils::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* allocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func) {
    func(instance, debug_messenger, allocator);
  } else {
    throw std::runtime_error(
        "Error::Vualkan: Failed to destroy debug utils messenger!");
  }
}

int Utils::EvaluatePhysicalDevice(const VkPhysicalDevice& physical_device,
                                  const VkSurfaceKHR& surface) {
  int score = 0;

  // physical device properties
  VkPhysicalDeviceProperties device_properties{};
  vkGetPhysicalDeviceProperties(physical_device, &device_properties);
  if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == device_properties.deviceType) {
    score += 1000;
  }
  score += device_properties.limits.maxImageDimension2D;

  // physical device features
  VkPhysicalDeviceFeatures device_features{};
  vkGetPhysicalDeviceFeatures(physical_device, &device_features);
  if (!device_features.geometryShader) {
    score -= 1000;
  }

  // physical device queue family support
  QueueFamilies queue_faimlies = QueryQueueFamilies(physical_device, surface);
  if (!queue_faimlies.IsCompleted()) {
    score = -1000;
  }

  // physical device extension support
  std::vector<const char*> required_exts{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef __APPLE__
  required_exts.push_back("VK_KHR_portability_subset");
#endif

  uint32_t available_ext_cnt = 0;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &available_ext_cnt, nullptr);
  std::vector<VkExtensionProperties> available_exts(available_ext_cnt);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &available_ext_cnt, available_exts.data());

  // compare
  std::set<std::string> extensions(required_exts.begin(), required_exts.end());
  for (const auto& extension : available_exts) {
    extensions.erase(extension.extensionName);
  }

  if (!extensions.empty()) {
    score -= 1000;
  }

  // physical device swap chain support
  SwapChainSupportDetails swap_chain_support =
      QuerySwapChainSupport(physical_device, surface);

  if (!swap_chain_support.IsAdequate()) {
    score -= 1000;
  }

  std::clog << "Physical Device: " << device_properties.deviceName << "\n";

  return score;
}

QueueFamilies Utils::QueryQueueFamilies(const VkPhysicalDevice& physical_device,
                                        const VkSurfaceKHR& surface) {
  QueueFamilies indices{};

  // queue families
  uint32_t queue_family_cnt = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_cnt,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_cnt);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_cnt,
                                           queue_families.data());

  // get index of required queue family
  for (uint32_t i = 0; i < queue_family_cnt; ++i) {
    if (queue_families[i].queueCount > 0 &&
        (VK_QUEUE_GRAPHICS_BIT & queue_families[i].queueFlags)) {
      indices.graphics_family = i;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface,
                                         &present_support);
    if (queue_families[i].queueCount > 0 && present_support) {
      indices.present_family = i;
    }

    if (indices.IsCompleted()) {
      break;
    }
  }

  return indices;
}

std::vector<const char*> Utils::QueryVulkanDeviceExts(
    const VkPhysicalDevice& physical_device) {
  std::vector<const char*> required_exts{};

  // swap chain extension
  required_exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef __APPLE__
  required_exts.push_back("VK_KHR_portability_subset");
#endif

  // available device extensions
  uint32_t available_ext_cnt = 0;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &available_ext_cnt, nullptr);
  std::vector<VkExtensionProperties> available_exts(available_ext_cnt);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &available_ext_cnt, available_exts.data());

  // compare
  std::set<std::string> extensions(required_exts.begin(), required_exts.end());
  for (const auto& extension : available_exts) {
    extensions.erase(extension.extensionName);
  }

  if (!extensions.empty()) {
    throw std::runtime_error(
        "Error::Vulkan: Error::Vulkan: Find not supported device "
        "extension(s)!");
  }

  return required_exts;
}

SwapChainSupportDetails Utils::QuerySwapChainSupport(
    const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) {
  SwapChainSupportDetails details{};

  // capabilities: min/max number of images in swap chain, min/max width and
  // height of images
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
                                            &details.capabilities);

  // formats: pixel format, color space
  uint32_t format_cnt = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_cnt,
                                       nullptr);
  if (0 != format_cnt) {
    details.formats.resize(format_cnt);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_cnt,
                                         details.formats.data());
  }

  // present modes
  uint32_t present_mode_cnt = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                            &present_mode_cnt, nullptr);
  if (0 != present_mode_cnt) {
    details.present_modes.resize(present_mode_cnt);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                              &present_mode_cnt,
                                              details.present_modes.data());
  }

  return details;
}

VkSurfaceFormatKHR Utils::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
  for (const auto& available : available_formats) {
    if (available.format == VK_FORMAT_R8G8B8A8_UNORM &&
        available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available;
    }
  }

  return available_formats[0];
}

VkPresentModeKHR Utils::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& available_present_modes) {
  for (const auto& available : available_present_modes) {
    if (available == VK_PRESENT_MODE_MAILBOX_KHR) {
      return available;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Utils::ChooseSwapExtent(
    GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actual_extent = {static_cast<uint32_t>(width),
                                static_cast<uint32_t>(height)};

    actual_extent.width =
        std::clamp(actual_extent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actual_extent.height =
        std::clamp(actual_extent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actual_extent;
  }
}

VkImageView Utils::CreateImageView(const VkDevice& device, const VkImage& image,
                                   const VkFormat& format) {
  VkImageViewCreateInfo view_info{};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  VkImageView image_view;

  VkResult result = vkCreateImageView(device, &view_info, nullptr, &image_view);
  CheckVulkanResult(result, "Error::Vulkan: Failed to create image view!");

  return image_view;
}

std::vector<char> Utils::ReadFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Error::File: Failed to open file!");
  }

  size_t file_size = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);

  file.close();

  return buffer;
}

VkShaderModule Utils::CreateShaderMoudle(const VkDevice& device,
                                         const std::vector<char>& code) {
  VkShaderModuleCreateInfo shader_module_info{};
  shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_info.codeSize = code.size();
  shader_module_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  VkResult result = vkCreateShaderModule(device, &shader_module_info, nullptr,
                                         &shader_module);
  CheckVulkanResult(result, "Error::Vulkan: Failed to create shader module!");

  return shader_module;
}

VkCommandBuffer Utils::BeginSingleTimeCommand(
    const VkDevice& device, const VkCommandPool& command_pool) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer, &begin_info);

  return command_buffer;
}

void Utils::EndSingleTimeCommand(const VkDevice& device,
                                 const VkQueue& graphics_queue,
                                 const VkCommandPool& command_pool,
                                 VkCommandBuffer& command_buffer) {
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue);

  vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void Utils::FramebufferResizeCallback(GLFWwindow* window, int width,
                                      int height) {
  if (width && height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->SetFramebufferResized(true);
  }
}

uint32_t Utils::FindMemoryType(const VkPhysicalDevice& physical_device,
                               uint32_t type_filter,
                               VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) {
    if (type_filter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags &
                                   properties) == properties) {
      return i;
    }
  }

  CheckVulkanResult(0, "Error::Vulkan: Failed to find suitable memory type!");

  return 0;
}

void Utils::CreateBuffer(const VkPhysicalDevice& physical_device,
                         const VkDevice& device, const VkDeviceSize size,
                         const VkBufferUsageFlags usage,
                         const VkMemoryPropertyFlags properties,
                         VkBuffer& buffer, VkDeviceMemory& buffer_memory) {
  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult result = vkCreateBuffer(device, &buffer_info, nullptr, &buffer);
  CheckVulkanResult(result, "Error::Vulkan: Failed to create buffer!");

  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(
      physical_device, mem_requirements.memoryTypeBits, properties);

  result = vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory);
  CheckVulkanResult(result, "Error::Vulkan: Failed to allocate buffer memory!");

  vkBindBufferMemory(device, buffer, buffer_memory, 0);
}

void Utils::CopyBuffer(const VkDevice& device,
                       const VkCommandPool& command_pool,
                       const VkQueue& graphics_queue, VkBuffer src_buffer,
                       VkBuffer dst_buffer, VkDeviceSize size) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommand(device, command_pool);

  VkBufferCopy copy_region{};
  copy_region.srcOffset = 0;  // Optional
  copy_region.dstOffset = 0;  // Optional
  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

  EndSingleTimeCommand(device, graphics_queue, command_pool, command_buffer);
}

void Utils::CreateImage(uint32_t width, uint32_t height, VkFormat format,
                        VkImageTiling tiling, VkImageUsageFlags usage,
                        const VkPhysicalDevice& physical_device,
                        const VkDevice& device,
                        VkMemoryPropertyFlags properties, VkImage& image,
                        VkDeviceMemory& image_memory) {
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.flags = 0;

  VkResult result = vkCreateImage(device, &image_info, nullptr, &image);
  Utils::CheckVulkanResult(result, "Error::Vulkan: Failed to create image!");

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(device, image, &mem_requirements);

  VkMemoryAllocateInfo alloc_nfo{};
  alloc_nfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_nfo.allocationSize = mem_requirements.size;
  alloc_nfo.memoryTypeIndex = Utils::FindMemoryType(
      physical_device, mem_requirements.memoryTypeBits, properties);

  result = vkAllocateMemory(device, &alloc_nfo, nullptr, &image_memory);
  Utils::CheckVulkanResult(result,
                           "Error::Vulkan: Failed to allocate image memory!");

  vkBindImageMemory(device, image, image_memory, 0);
}

void Utils::TransitionImageLayout(const VkDevice& device,
                                  const VkQueue& graphics_queue,
                                  const VkCommandPool& command_pool,
                                  VkImage image, VkImageLayout old_layout,
                                  VkImageLayout new_layout) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommand(device, command_pool);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::invalid_argument(
        "Error::Vulkan: Unsupported layout transition!");
  }

  vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  EndSingleTimeCommand(device, graphics_queue, command_pool, command_buffer);
}

void Utils::CopyBufferToImage(const VkDevice& device,
                              const VkQueue& graphics_queue,
                              const VkCommandPool& command_pool,
                              VkBuffer buffer, VkImage image, uint32_t width,
                              uint32_t height) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommand(device, command_pool);

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(command_buffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  EndSingleTimeCommand(device, graphics_queue, command_pool, command_buffer);
}

uint32_t Utils::GetColor(const glm::vec3 color, int samples_per_pixel,
                         float gamma) {
  float scale = static_cast<float>(1.f / samples_per_pixel);

  uint32_t R = static_cast<uint32_t>(
      std::pow(std::clamp(scale * color.r, 0.f, 0.999f) * 256.f, 1.f / gamma));
  uint32_t G = static_cast<uint32_t>(
      std::pow(std::clamp(scale * color.g, 0.f, 0.999f) * 256.f, 1.f / gamma));
  uint32_t B = static_cast<uint32_t>(
      std::pow(std::clamp(scale * color.b, 0.f, 0.999f) * 256.f, 1.f / gamma));

  return (255 << 24) | (B << 16) | (G << 8) | R;
}

uint32_t Utils::GetColor(const glm::vec4 color, int samples_per_pixel,
                         float gamma) {
  float scale = static_cast<float>(1.f / samples_per_pixel);

  uint32_t R = static_cast<uint32_t>(
      std::pow(std::clamp(scale * color.r, 0.f, 0.999f) * 256.f, 1.f / gamma));
  uint32_t G = static_cast<uint32_t>(
      std::pow(std::clamp(scale * color.g, 0.f, 0.999f) * 256.f, 1.f / gamma));
  uint32_t B = static_cast<uint32_t>(
      std::pow(std::clamp(scale * color.b, 0.f, 0.999f) * 256.f, 1.f / gamma));
  uint32_t A = static_cast<uint32_t>(std::clamp(color.a, 0.f, 0.999f) * 256.f);

  return (A << 24) | (B << 16) | (G << 8) | R;
}

float Utils::RandomFloat(float min, float max) {
  static std::uniform_real_distribution<float> distribution(min, max);
  static std::mt19937 generator{};

  return distribution(generator);
}

glm::vec3 Utils::RandomVec3(float min, float max) {
  return glm::vec3(RandomFloat(min, max), RandomFloat(min, max),
                   RandomFloat(min, max));
}

}  // namespace rt
