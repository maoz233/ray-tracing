/**
 * @file camera.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_CAMERA_H_
#define RAY_TRACING_INCLUDE_CAMERA_H_

#include <glm/glm.hpp>

#include "ray.h"

namespace rt {

class Camera {
 public:
  Camera() = default;
  Camera(glm::vec3 origin, glm::vec3 look_at, glm::vec3 world_up, float fov,
         float aspect_ratio);
  ~Camera() = default;

  Ray GetRay(float u, float v) const;

 private:
  glm::vec3 origin_;
  glm::vec3 lower_left_;
  glm::vec3 horizontal_;
  glm::vec3 vertical_;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_CAMERA_H_
