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
#define RAY_TRACING_INCLUDE_GLM
#include <glm/glm.hpp>

#include "camera.h"
#include "config.h"
#include "dielectric.h"
#include "hittable_list.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
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
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
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
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
  ImGui::Begin("Settings");

  // imgui child window: statistics
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
  window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
  window_flags |= ImGuiWindowFlags_MenuBar;
  ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
  ImGui::BeginChild("Statistics", ImVec2(0.f, 100.f), true, window_flags);

  if (ImGui::BeginMenuBar()) {
    ImGui::BeginMenu("Statistics", false);
    ImGui::EndMenuBar();
  }

  // imgui text: time
  ImGui::Text("Time: %.2fms", delta_time_);
  // imgui text: fps
  ImGui::Text("FPS: %.2f", delta_time_ ? 1000.f / delta_time_ : 0.f);
  // imgui text: scene extent detail
  ImGui::Text("Scene: %d * %d", width_, height_);

  ImGui::EndChild();

  //  imgui child window: ray
  ImGui::BeginChild("Ray", ImVec2(0.f, 150.f), true, window_flags);

  if (ImGui::BeginMenuBar()) {
    ImGui::BeginMenu("Ray", false);
    ImGui::EndMenuBar();
  }

  // imgui input: samples per pixel
  ImGui::Text("Samples");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(50.f);
  ImGui::DragInt("##SamplesPerPixel", &samples_per_pixel_, 1.f, 1, 1000, "%d",
                 ImGuiSliderFlags_AlwaysClamp);

  // imgui input: bounce limit
  ImGui::Text("Bounce");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(50.f);
  ImGui::DragInt("##BounceLimit", &bounce_limit_, 1.f, 1, 1000, "%d",
                 ImGuiSliderFlags_AlwaysClamp);

  // imgui input: gamma
  ImGui::Text("Gamma");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(50.f);
  ImGui::DragFloat("##Gamma", &gamma_, 0.01f, 0.f, 10.f, "%.2f",
                   ImGuiSliderFlags_AlwaysClamp);

  ImGui::EndChild();

  // imgui child window: camera
  ImGui::BeginChild("Camera", ImVec2(0.f, 150.f), true, window_flags);

  if (ImGui::BeginMenuBar()) {
    ImGui::BeginMenu("Camera", false);
    ImGui::EndMenuBar();
  }

  ImGui::Text("Origin ");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(150.f);
  ImGui::DragFloat3("##CameraOrigin", origin_, 0.01f, 0.f, 0.f, "%.2f");

  ImGui::Text("FOV ");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(50.f);
  ImGui::DragFloat("##FieldOfViewVertically", &fov_, 0.01f, 0.f, 1.f, "%.2f",
                   ImGuiSliderFlags_AlwaysClamp);

  ImGui::Text("Aperture ");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(50.f);
  ImGui::DragFloat("##FieldOfDepth", &aperture_, 0.01f, 0.f, 1.f, "%.2f");

  ImGui::EndChild();

  //  imgui child window: render
  ImGui::BeginChild("Render", ImVec2(0.f, 100.f), true, window_flags);

  if (ImGui::BeginMenuBar()) {
    ImGui::BeginMenu("Render", false);
    ImGui::EndMenuBar();
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

  ImGui::EndChild();
  ImGui::PopStyleVar();
  ImGui::PopStyleVar();

  ImGui::End();
  ImGui::PopStyleVar();

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
  glm::vec3 origin(origin_[0], origin_[1], origin_[2]);
  glm::vec3 lookat(0.f);
  glm::vec3 world_up(0.f, 1.f, 0.f);

  Camera camera(origin, lookat, world_up, fov_,
                static_cast<float>(width_ / height_), aperture_, focus_dist_);

  // world
  HittableList world = RandomScene();

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
    Ray scattered{};
    glm::vec3 attenuation{};

    if (record.material->Scatter(ray, record, attenuation, scattered)) {
      return attenuation * RayColor(scattered, world, bounce - 1);
    }

    return glm::vec3(0.f, 0.f, 0.f);
  }

  // background color
  glm::vec3 unit_direction = glm::normalize(ray.GetDirection());
  float t = 0.5f * (unit_direction.y + 1.f);
  glm::vec3 color =
      (1.f - t) * glm::vec3(1.f, 1.f, 1.f) + t * glm::vec3(0.5f, 0.7f, 1.f);

  return color;
}

HittableList Scene::RandomScene() {
  HittableList world{};

  std::shared_ptr<Material> ground_material =
      std::make_shared<Lambertian>(glm::vec3(0.5f));
  std::shared_ptr<Hittable> ground = std::make_shared<Sphere>(
      glm::vec3(0.f, -1000.f, 0.f), 1000.f, ground_material);
  world.Add(ground);

  // for (int i = -11; i < 11; ++i) {
  //   for (int j = -11; j < 11; ++j) {
  //     float choose_mat = Utils::RandomFloat();
  //     glm::vec3 center(i + 0.9f * Utils::RandomFloat(), 0.2f,
  //                      j + 0.9f * Utils::RandomFloat());

  //     if (glm::length(center - glm::vec3(4.f, 0.2f, 0.f)) > 0.9f) {
  //       std::shared_ptr<Material> material{};

  //       if (choose_mat < 0.8f) {
  //         // diffuse
  //         glm::vec3 albedo = Utils::RandomVec3();
  //         material = std::make_shared<Lambertian>(albedo);
  //       } else if (choose_mat < 0.95f) {
  //         // metal
  //         float fuzz = Utils::RandomFloat(0.f, 0.05f);
  //         glm::vec3 albedo(Utils::RandomFloat(0.5f, 1.f));
  //         material = std::make_shared<Metal>(fuzz, albedo);
  //       } else {
  //         // glass
  //         material = std::make_shared<Dielectric>(1.5f);
  //       }

  //       std::shared_ptr<Sphere> sphere =
  //           std::make_shared<Sphere>(center, 0.2f, material);
  //       world.Add(sphere);
  //     }
  //   }
  // }

  std::shared_ptr<Material> diffuse_material =
      std::make_shared<Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f));
  std::shared_ptr<Sphere> diffuse_sphere = std::make_shared<Sphere>(
      glm::vec3(-4.f, 1.f, 0.f), 1.f, diffuse_material);
  world.Add(diffuse_sphere);

  std::shared_ptr<Material> metal_material =
      std::make_shared<Metal>(0.f, glm::vec3(0.7f, 0.6f, 0.5f));
  std::shared_ptr<Sphere> metal_sphere =
      std::make_shared<Sphere>(glm::vec3(4.f, 1.f, 0.f), 1.f, metal_material);
  world.Add(metal_sphere);

  std::shared_ptr<Material> dielectric_material =
      std::make_shared<Dielectric>(1.5f);
  std::shared_ptr<Sphere> dielectric_sphere = std::make_shared<Sphere>(
      glm::vec3(0.f, 1.f, 0.f), 1.f, dielectric_material);
  world.Add(dielectric_sphere);

  return world;
}

}  // namespace rt
