/**
 * @file application.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_APPLICATION_H_
#define RAY_TRACING_INCLUDE_APPLICATION_H_

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "layer.h"
#include "utils.h"

namespace rt {

class Application {
 public:
  Application();
  ~Application();

  void Run();

  // create GLFW window
  void CreateWindow();
  // create Vulkan instance
  void CreateInstance();
  // setup Vulkan debug messenger
  void SetupDebugMessenger();
  // create window surface
  void CreateWindowSurface();
  // pick physical device
  void PickPhysicalDevice();
  // create logical device
  void CreateLogicalDevice();
  // create swap chain
  void CreateSwapChain();
  // create image view
  void CreateImageViews();
  // create render pass
  void CreateRenderPass();
  // create descritor pool
  void CreateDescriptorPool();
  // create frame buffers
  void CreateFramebuffers();
  // create command pool
  void CreateCommandPool();
  // create command buffers
  void CreateCommandBuffers();
  // create fences and semaphores
  void CreateSyncObjects();
  // setup ImGui
  void SetupImGui();

  // draw frame
  void DrawFrame();

  // recreate swapchain
  void RecreateSwapChain();
  // clean swap chain
  void CleanupSwapChain();
  // record command buffer
  void RecordCommandBuffer(VkCommandBuffer command_buffer,
                           uint32_t image_index);

  // set framebuffer resized
  void SetFramebufferResized(bool resized);

 private:
  GLFWwindow* window_ = nullptr;

  VkInstance instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;

  QueueFamilies queue_families_;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;

  VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
  std::vector<VkImage> swap_chain_images_;
  VkFormat swap_chain_image_format_;
  VkExtent2D swap_chain_extent_;

  std::vector<VkImageView> swap_chain_image_views_;

  VkRenderPass render_pass_ = VK_NULL_HANDLE;

  VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;

  std::vector<VkFramebuffer> swap_chain_framebuffers_;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> command_buffers_;

  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;

  int current_frame_ = 0;
  bool framebuffer_resized_ = false;

  Layer* layer_ = nullptr;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_APPLICATION_H_
