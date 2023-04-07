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
#include "utils.h"

namespace rt {

Camera::Camera(glm::vec3 origin, glm::vec3 look_at, glm::vec3 world_up,
               float fov, float aspect_ratio) {
  const float theta = Utils::DegreesToRadians(fov);
  const float height = glm::tan(theta / 2.f);
  const float viewport_height = 2.f * height;
  const float viewport_width = viewport_height * aspect_ratio;

  glm::vec3 forward = glm::normalize(look_at - origin);
  glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
  glm::vec3 up = glm::cross(right, forward);

  origin_ = origin;
  horizontal_ = viewport_width * right;
  vertical_ = viewport_height * up;
  lower_left_ = origin_ - horizontal_ / 2.f - vertical_ / 2.f + forward;
}

Ray Camera::GetRay(float u, float v) const {
  return Ray(origin_, lower_left_ + u * horizontal_ + v * vertical_ - origin_);
}

}  // namespace rt
