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
               float fov, float aspect_ratio, float aperture,
               float focus_dist) {
  const float theta = Utils::DegreesToRadians(fov);
  const float height = glm::tan(theta / 2.f);
  const float viewport_height = 2.f * height;
  const float viewport_width = viewport_height * aspect_ratio;

  forward_ = glm::normalize(look_at - origin);
  right_ = glm::normalize(glm::cross(forward_, world_up));
  up_ = glm::cross(right_, forward_);

  origin_ = origin;
  horizontal_ = focus_dist * viewport_width * right_;
  vertical_ = focus_dist * viewport_height * up_;
  lower_left_ =
      origin_ - horizontal_ / 2.f - vertical_ / 2.f + focus_dist * forward_;

  lens_radius_ = aperture / 2.f;
}

Ray Camera::GetRay(float u, float v) const {
  glm::vec3 ray_direction = lens_radius_ * Utils::RandomInUnitDisk();
  glm::vec3 offset = right_ * ray_direction.x + up_ * ray_direction.y;

  return Ray(origin_ + offset,
             lower_left_ + u * horizontal_ + v * vertical_ - origin_ - offset);
}

}  // namespace rt
