/**
 * @file image.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "image.h"

#include <vulkan/vulkan.h>

#define RAY_TRACING_INCLUDE_IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "utils.h"

namespace rt {

Image::Image(uint32_t width, uint32_t height, VkPhysicalDevice& physical_device,
             VkDevice& device, VkQueue& graphics_queue,
             VkCommandPool& command_pool, const void* data)
    : width_{width},
      height_{height},
      physical_device_{physical_device},
      device_{device},
      graphics_queue_{graphics_queue},
      command_pool_{command_pool} {
  // create image
  CreateTextureImage();
  // create image view
  CreateTextureImageView();
  // create sampler
  CreateTextureSampler();
  // create descritor set
  CreateDescriptorSet();

  if (data) {
    SetData(data);
  }
}

Image::~Image() {
  vkDestroySampler(device_, texture_sampler_, nullptr);
  vkDestroyImageView(device_, texture_image_view_, nullptr);
  vkDestroyImage(device_, texture_image_, nullptr);
  vkFreeMemory(device_, texture_image_memory_, nullptr);
}

void Image::CreateTextureImage() {
  // create image
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.format = VK_FORMAT_R8G8B8A8_UNORM;

  // image extent
  image_info.extent.width = width_;
  image_info.extent.height = height_;
  image_info.extent.depth = 1;

  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_info.usage =
      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VkResult result =
      vkCreateImage(device_, &image_info, nullptr, &texture_image_);
  Utils::CheckVulkanResult(result, "Error::Vulkan: Failed to create image!");

  VkMemoryRequirements mem_requirements{};
  vkGetImageMemoryRequirements(device_, texture_image_, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex =
      Utils::FindMemoryType(physical_device_, mem_requirements.memoryTypeBits,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  result =
      vkAllocateMemory(device_, &alloc_info, nullptr, &texture_image_memory_);
  Utils::CheckVulkanResult(result,
                           "Error::Vulkan: Failed to allocate image memory!");

  result = vkBindImageMemory(device_, texture_image_, texture_image_memory_, 0);
  Utils::CheckVulkanResult(result,
                           "Error::Vulkan: Failed to bind image memory!");
}

void Image::CreateTextureImageView() {
  // create image view
  VkImageViewCreateInfo image_view_info{};
  image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_info.image = texture_image_;
  image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
  image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_info.subresourceRange.levelCount = 1;
  image_view_info.subresourceRange.layerCount = 1;

  VkResult result = vkCreateImageView(device_, &image_view_info, nullptr,
                                      &texture_image_view_);
  Utils::CheckVulkanResult(result,
                           "Error::Vulkan: Failed to create image view");
}

void Image::CreateTextureSampler() {
  // create sampler
  VkSamplerCreateInfo sampler_info{};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.minLod = -1000;
  sampler_info.maxLod = 1000;
  sampler_info.maxAnisotropy = 1.0f;

  VkResult result =
      vkCreateSampler(device_, &sampler_info, nullptr, &texture_sampler_);
  Utils::CheckVulkanResult(result, "Error::Vulkan: Failed to create sampler!");
}

void Image::CreateDescriptorSet() {
  descriptor_set_ =
      ImGui_ImplVulkan_AddTexture(texture_sampler_, texture_image_view_,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Image::SetData(const void* data) {
  size_t image_size = width_ * height_ * 4;

  // create staging buffer
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;

  Utils::CreateBuffer(physical_device_, device_, image_size,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      staging_buffer, staging_buffer_memory);

  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(device_, staging_buffer, &mem_requirements);

  // map data to memory
  void* map;
  vkMapMemory(device_, staging_buffer_memory, 0, image_size, 0, &map);
  memcpy(map, data, static_cast<size_t>(image_size));
  VkMappedMemoryRange range[1]{};
  range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range[0].memory = staging_buffer_memory;
  range[0].size = mem_requirements.size;
  vkFlushMappedMemoryRanges(device_, 1, range);
  vkUnmapMemory(device_, staging_buffer_memory);

  Utils::TransitionImageLayout(device_, graphics_queue_, command_pool_,
                               texture_image_, VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  Utils::CopyBufferToImage(
      device_, graphics_queue_, command_pool_, staging_buffer, texture_image_,
      static_cast<uint32_t>(width_), static_cast<uint32_t>(height_));
  Utils::TransitionImageLayout(device_, graphics_queue_, command_pool_,
                               texture_image_,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void Image::Resize(uint32_t width, uint32_t height) {
  if (texture_image_ && width_ == width && height_ == height) {
    return;
  }

  width_ = width;
  height_ = height;

  vkDestroySampler(device_, texture_sampler_, nullptr);
  vkDestroyImageView(device_, texture_image_view_, nullptr);
  vkDestroyImage(device_, texture_image_, nullptr);
  vkFreeMemory(device_, texture_image_memory_, nullptr);

  CreateTextureImage();
  CreateTextureImageView();
  CreateTextureSampler();
}

uint32_t Image::GetWidth() const { return width_; }

uint32_t Image::GetHeight() const { return height_; }

VkDescriptorSet Image::GetDescritorSet() const { return descriptor_set_; }

}  // namespace rt