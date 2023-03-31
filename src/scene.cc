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

#include <iomanip>
#include <iostream>
#include <memory>

#define RAY_TRACING_INCLUDE_IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define RAY_TRACING_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "ray.h"

namespace rt {

Scene::Scene(VkPhysicalDevice& physical_device, VkDevice& device,
             VkQueue& graphics_queue, VkCommandPool& command_pool)
    : physical_device_{physical_device},
      device_{device},
      graphics_queue_{graphics_queue},
      command_pool_{command_pool} {}

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
                       graphics_queue_, command_pool_);
    // clear previous image data
    delete[] image_data_;
    // allocate new image data
    image_data_ = new uint32_t[width_ * height_];
  }

  // ray tracing viewport
  const float aspect_ratio =
      static_cast<float>(static_cast<float>(width_) / height_);
  const float focal_length = 1.f;
  const float viewport_height = 2.f;
  const float viewport_width = viewport_height * aspect_ratio;
  glm::vec3 origin{0.f, 0.f, 0.f};
  glm::vec3 horizontal{viewport_width, 0.f, 0.f};
  glm::vec3 vertical{0.f, viewport_height, 0.f};
  glm::vec3 lower_left = origin - horizontal / 2.f - vertical / 2.f -
                         glm::vec3(0.f, 0.f, focal_length);

  // set image pixel data
  for (uint32_t j = 0; j < height_; ++j) {
    for (uint32_t i = 0; i < width_; ++i) {
      auto u = static_cast<float>(i) / static_cast<float>(width_ - 1);
      auto v =
          static_cast<float>(height_ - j) / static_cast<float>(height_ - 1);
      Ray ray{origin, lower_left + u * horizontal + v * vertical - origin};

      image_data_[j * width_ + i] = RayColor(ray);
    }
  }

  // set image data
  image_->SetData(image_data_);
}

uint32_t Scene::RayColor(const Ray& ray) {
  glm::vec3 unit_direction = glm::normalize(ray.GetDirection());

  float t = 0.5f * (unit_direction.y + 1.f);
  auto color =
      (1.f - t) * glm::vec3(255, 255, 255) + t * glm::vec3(128, 179, 255);
  uint32_t R = static_cast<uint32_t>(color.r);
  uint32_t G = static_cast<uint32_t>(color.g);
  uint32_t B = static_cast<uint32_t>(color.b);

  auto res = (255 << 24) | (B << 16) | (G << 8) | R;

  return res;
}

}  // namespace rt
