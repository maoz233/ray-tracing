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

#include <vulkan/vulkan.h>

#include <cstdint>

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

  glm::vec3 RandomInUnitSphere();

  glm::vec3 RandomInHemiSphere(const glm::vec3& normal);

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
  int samples_per_pixel_ = 1;
  int bounce_limit_ = 5;
  float gamma_ = 1.05f;
  bool is_playing_ = false;
  const char* play_button_label_ = "Play";
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_SCENE_H_
