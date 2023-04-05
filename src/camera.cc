/**
 * @file camera.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "camera.h"

#include "ray.h"

namespace rt {

Camera::Camera() {
  const float aspect_ratio = 16.f / 9.f;
  const float focal_length = 1.f;
  const float viewport_height = 2.f;
  const float viewport_width = viewport_height * aspect_ratio;

  origin_ = glm::vec3(0.f, 0.f, 0.f);
  horizontal_ = glm::vec3(viewport_width, 0.f, 0.f);
  vertical_ = glm::vec3(0.f, viewport_height, 0.f);
  lower_left_ = origin_ - horizontal_ / 2.f - vertical_ / 2.f -
                glm::vec3(0.f, 0.f, focal_length);
}

Ray Camera::GetRay(float u, float v) const {
  return Ray(origin_, lower_left_ + u * horizontal_ + v * vertical_ - origin_);
}

}  // namespace rt
