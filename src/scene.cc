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

#include <algorithm>
#include <chrono>
#include <memory>

#define RAY_TRACING_INCLUDE_IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define RAY_TRACING_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "camera.h"
#include "config.h"
#include "hittable_list.h"
#include "ray.h"
#include "sphere.h"
#include "utils.h"

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

  ImGui::ShowDemoWindow();

  // imgui viewport: settings
  ImGui::Begin("Settings");

  // imgui text: fps
  ImGui::Text("Time: %.2fms", delta_time_);
  // imgui text: fps
  ImGui::Text("FPS: %.2f", 1000.f / delta_time_);
  // imgui text: scene extent detail
  ImGui::Text("Scene: %d * %d", width_, height_);

  // imgui input: samples per pixel
  ImGui::Text("Samples: ");
  ImGui::SameLine();
  ImGui::InputInt("##SamplesPerPixel", &samples_per_pixel_);

  if (samples_per_pixel_ < 1) {
    samples_per_pixel_ = 1;
  }

  // imgui input: bounce limit
  ImGui::Text("Bounce: ");
  ImGui::SameLine();
  ImGui::InputInt("##BounceLimit", &bounce_limit_);

  if (bounce_limit_ < 0) {
    bounce_limit_ = 0;
  }

  // imgui input: gamma
  ImGui::Text("Gamma: ");
  ImGui::SameLine();
  ImGui::InputFloat("##Gamma", &gamma_, 0.01f, 1.f, "%.2f");

  if (gamma_ < 0.f) {
    gamma_ = 0.f;
  }

  // imgui: test button
  if (ImGui::Button("Test")) {
    Render();
  }

  // imgui: play/pause button
  if (ImGui::Button(play_button_label_)) {
    is_playing_ = !is_playing_;
    play_button_label_ = is_playing_ ? "Pause" : "Play";
  }

  ImGui::End();

  if (is_playing_) {
    Render();
  }
}

void Scene::Render() {
  // begin time
  auto begin = std::chrono::high_resolution_clock::now();

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

  // camera
  Camera camera{};

  // world
  HittableList world{};
  world.Add(std::make_shared<Sphere>(glm::vec3(0.f, 0.f, -1.f), 0.5f));
  world.Add(std::make_shared<Sphere>(glm::vec3(0.f, -100.5f, -1.f), 100.f));

  // set image pixel data
  for (uint32_t j = 0; j < height_; ++j) {
    for (uint32_t i = 0; i < width_; ++i) {
      glm::vec3 pixel_color{};

      for (int s = 0; s < samples_per_pixel_; ++s) {
        float u = static_cast<float>(i + Utils::RandomFloat()) /
                  static_cast<float>(width_ - 1);
        float v = 1.f - static_cast<float>(j + Utils::RandomFloat()) /
                            static_cast<float>(height_ - 1);

        Ray ray = camera.GetRay(u, v);
        pixel_color += RayColor(ray, world, bounce_limit_);
      }

      image_data_[j * width_ + i] =
          Utils::GetColor(pixel_color, samples_per_pixel_, gamma_);
    }
  }

  // set image data
  image_->SetData(image_data_);

  // end time
  auto end = std::chrono::high_resolution_clock::now();

  // delta time
  delta_time_ =
      std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(
          end - begin)
          .count() /
      1000.f;
}

glm::vec3 Scene::RayColor(const Ray& ray, const Hittable& world, int bounce) {
  // hit record
  HitRecord record{};

  // check whether exceed the ray bounce limit
  if (bounce <= 0) {
    return glm::vec3(0.f, 0.f, 0.f);
  }

  // hittable objects color
  if (world.Hit(ray, 0.001f, INFINITY_F, record)) {
    glm::vec3 target = record.point + record.normal +
                       glm::normalize(RandomInHemiSphere(record.normal));
    glm::vec3 color = 0.5f * RayColor(Ray(record.point, target - record.point),
                                      world, bounce - 1);

    return color;
  }

  // background color
  glm::vec3 unit_direction = glm::normalize(ray.GetDirection());
  float t = 0.5f * (unit_direction.y + 1.f);
  glm::vec3 color =
      (1.f - t) * glm::vec3(1.f, 1.f, 1.f) + t * glm::vec3(0.5f, 0.7f, 1.f);

  return color;
}

glm::vec3 Scene::RandomInUnitSphere() {
  while (true) {
    glm::vec3 point = Utils::RandomVec3(-1.f, 1.f);

    if (glm::dot(point, point) >= 1) {
      continue;
    }

    return point;
  }
}

glm::vec3 Scene::RandomInHemiSphere(const glm::vec3& normal) {
  glm::vec3 in_unit_sphere = RandomInUnitSphere();

  // whether in current normal semi-sphere or not
  if (glm::dot(in_unit_sphere, normal) > 0.f) {
    return in_unit_sphere;
  } else {
    return -in_unit_sphere;
  }
}

}  // namespace rt
