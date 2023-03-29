/**
 * @file image.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_IMAGE_H_
#define RAY_TRACING_INCLUDE_IMAGE_H_

#include <vector>

#define RAY_TRACING_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

namespace rt {

class Image {
 public:
  Image() = default;
  Image(uint32_t width, uint32_t height, VkPhysicalDevice& physical_device,
        VkDevice& device, VkQueue& graphics_queue, VkCommandPool& command_pool,
        VkDescriptorPool& descritor_pool, const void* data = nullptr);
  ~Image();

  void CreateTextureImage();
  void CreateTextureImageView();
  void CreateTextureSampler();
  void CreateDescriptorSet();

  void SetData(const void* data);
  void Resize(uint32_t width, uint32_t height);

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;
  VkDescriptorSet GetDescritorSet() const;

 private:
  uint32_t width_ = 0;
  uint32_t height_ = 0;

  VkPhysicalDevice& physical_device_;
  VkDevice& device_;
  VkQueue& graphics_queue_;
  VkCommandPool& command_pool_;
  VkDescriptorPool& descriptor_pool_;

  VkImage texture_image_ = VK_NULL_HANDLE;
  VkDeviceMemory texture_image_memory_ = VK_NULL_HANDLE;
  VkImageView texture_image_view_ = VK_NULL_HANDLE;
  VkSampler texture_sampler_ = VK_NULL_HANDLE;

  VkDescriptorSet descriptor_set_ = VK_NULL_HANDLE;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_IMAGE_H_
