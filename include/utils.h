/**
 * @file utils.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_UTILS_H_
#define RAY_TRACING_INCLUDE_UTILS_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

namespace rt {

struct QueueFamilies {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  inline bool IsCompleted();
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;

  inline bool IsAdequate();
};

struct ImGuiBackendData {};

class Utils {
 public:
  // GLFW error callback
  static void GLFWErrorCallback(const int code, const char* description);

  // Vulkan result validator
  static void CheckVulkanResult(const int result, const char* error_msg);

  // Vulkan instance required extensions
  static std::vector<const char*> QueryVulkanInstanceExts();

  // Vulkan instance required layers
  static std::vector<const char*> QueryVulkanLayers();

  // Vulakn debug messenger callback
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                VkDebugUtilsMessageTypeFlagsEXT message_type,
                const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                void* user_data);

  // Vulkan debug messenger create and destroy
  static VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* create_info,
      const VkAllocationCallbacks* allocator,
      VkDebugUtilsMessengerEXT* debug_messenger);
  static void DestroyDebugUtilsMessengerEXT(
      VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
      const VkAllocationCallbacks* allocator);

  // evaluate GPU
  static int EvaluatePhysicalDevice(const VkPhysicalDevice& physical_device,
                                    const VkSurfaceKHR& surface);

  // queue faimilies
  static QueueFamilies QueryQueueFamilies(
      const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);

  //  device required extensions
  static std::vector<const char*> QueryVulkanDeviceExts(
      const VkPhysicalDevice& physical_device);

  // swap chain support details
  static SwapChainSupportDetails QuerySwapChainSupport(
      const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);

  // swap surface format
  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats);

  // presentation mode
  static VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes);

  // swap extent
  static VkExtent2D ChooseSwapExtent(
      GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

  // create image view
  static VkImageView CreateImageView(const VkDevice& device,
                                     const VkImage& image,
                                     const VkFormat& format);

  // read file
  static std::vector<char> ReadFile(const std::string& filename);

  // create shader module
  static VkShaderModule CreateShaderMoudle(const VkDevice& device,
                                           const std::vector<char>& code);
  // begin single time command
  static VkCommandBuffer BeginSingleTimeCommand(
      const VkDevice& device, const VkCommandPool& command_pool);

  // end single time command
  static void EndSingleTimeCommand(const VkDevice& device,
                                   const VkQueue& graphics_queue,
                                   const VkCommandPool& command_pool,
                                   VkCommandBuffer& command_buffer);

  // frame buffer resize callback
  static void FramebufferResizeCallback(GLFWwindow* window, int width,
                                        int height);

  // find memory types
  static uint32_t FindMemoryType(const VkPhysicalDevice& physical_device,
                                 uint32_t type_filter,
                                 VkMemoryPropertyFlags properties);

  // create buffer
  static void CreateBuffer(const VkPhysicalDevice& physical_device,
                           const VkDevice& device, const VkDeviceSize size,
                           const VkBufferUsageFlags usage,
                           const VkMemoryPropertyFlags properties,
                           VkBuffer& buffer, VkDeviceMemory& buffer_memory);

  // copy buffer
  static void CopyBuffer(const VkDevice& device,
                         const VkCommandPool& command_pool,
                         const VkQueue& graphics_queue, VkBuffer src_buffer,
                         VkBuffer dst_buffer, VkDeviceSize size);

  // create image
  static void CreateImage(uint32_t width, uint32_t height, VkFormat format,
                          VkImageTiling tiling, VkImageUsageFlags usage,
                          const VkPhysicalDevice& physical_device,
                          const VkDevice& device,
                          VkMemoryPropertyFlags properties, VkImage& image,
                          VkDeviceMemory& image_memory);
  // image transition
  static void TransitionImageLayout(const VkDevice& device,
                                    const VkQueue& graphics_queue,
                                    const VkCommandPool& command_pool,
                                    VkImage image, VkImageLayout old_layout,
                                    VkImageLayout new_layout);

  // copy buffer to image
  static void CopyBufferToImage(const VkDevice& device,
                                const VkQueue& graphics_queue,
                                const VkCommandPool& command_pool,
                                VkBuffer buffer, VkImage image, uint32_t width,
                                uint32_t height);
  // RGB to RGBA in hexadecimal
  static uint32_t GetColor(const glm::vec3 color, int samples_per_pixel = 1,
                           float gamma = 1.f);

  // RGBA in hexadecimal
  static uint32_t GetColor(const glm::vec4 color, int samples_per_pixel = 1,
                           float gamma = 1.f);

  // random float number
  static float RandomFloat(float min = 0.f, float max = 1.f);

  // random 3-dimension vector
  static glm::vec3 RandomVec3(float min = 0.f, float max = 1.f);

  // check vector is near zero
  static bool NearZero(const glm::vec3& vec);

  static glm::vec3 RandomInUnitSphere();

  static glm::vec3 RandomInHemiSphere(const glm::vec3& normal);
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_UTILS_H_
