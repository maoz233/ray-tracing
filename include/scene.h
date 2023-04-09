/**
 * @file scene.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_SCENE_H_
#define RAY_TRACING_INCLUDE_SCENE_H_

#include <cstdint>

#define RAY_TRACING_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#define RAY_TRACING_INCLUDE_GLM
#include <glm/glm.hpp>

#include "hittable.h"
#include "image.h"
#include "layer.h"
#include "ray.h"
#include "sphere.h"

namespace rt {

class Scene : public Layer {
 public:
  Scene() = delete;
  Scene(VkPhysicalDevice& physical_device, VkDevice& device,
        VkQueue& graphics_queue, VkCommandPool& command_pool);
  virtual ~Scene();

  virtual void OnUIRender() override;

  void Render();

  glm::vec3 RayColor(const Ray& ray, const Hittable& world, int bounce);

 private:
  uint32_t width_ = 0;
  uint32_t height_ = 0;

  Image* image_ = nullptr;
  uint32_t* image_data_ = nullptr;

  VkPhysicalDevice& physical_device_;
  VkDevice& device_;
  VkQueue& graphics_queue_;
  VkCommandPool& command_pool_;

  float delta_time_ = 0.f;
  int samples_per_pixel_ = 100;
  int bounce_limit_ = 50;
  float gamma_ = 1.05f;
  bool is_playing_ = false;
  const char* play_button_label_ = "Play";

  float origin_[3] = {-2.f, 2.f, 1.f};
  float fov_ = 90.f;
  float aperture_ = 2.f;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_SCENE_H_
