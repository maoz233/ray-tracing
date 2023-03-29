/**
 * @file scene.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "scene.h"

#include <memory>

#define RAY_TRACING_INCLUDE_IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define RAY_TRACING_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

namespace rt {

Scene::Scene(VkPhysicalDevice& physical_device, VkDevice& device,
             VkQueue& graphics_queue, VkCommandPool& command_pool,
             VkDescriptorPool& descriptor_pool)
    : physical_device_{physical_device},
      device_{device},
      graphics_queue_{graphics_queue},
      command_pool_{command_pool},
      descriptor_pool_{descriptor_pool} {}

Scene::~Scene() {
  delete[] image_;
  delete[] image_data_;
}

void Scene::OnUIRender() {
  // imgui: scene viewport
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Scene");

  width_ = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
  height_ = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

  // imgui: draw image
  if (image_ && width_ && height_) {
    ImGui::Image(image_->GetDescritorSet(),
                 {static_cast<float>(width_), static_cast<float>(height_)});
  }

  ImGui::End();
  ImGui::PopStyleVar();

  // imgui: settings viewport
  ImGui::Begin("Settings");

  // imgui: scene extent detail text
  ImGui::Text("Scene: %d * %d", width_, height_);

  ImGui::End();

  Render();
}

void Scene::Render() {
  if (!image_ || width_ != image_->GetWidth() ||
      height_ != image_->GetHeight()) {
    // create new image
    image_ = new Image(width_, height_, physical_device_, device_,
                       graphics_queue_, command_pool_, descriptor_pool_);
    // clear previous image data
    delete[] image_data_;
    // allocate new image data
    image_data_ = new uint32_t[width_ * height_];
  }

  // set image pixel data
  for (uint32_t i = 0; i < width_ * height_; ++i) {
    image_data_[i] = 0xffffffff;
  }

  // set image data
  image_->SetData(image_data_);
}

}  // namespace rt
