/**
 * @file hittable.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_HITTABLE_H_
#define RAY_TRACING_INCLUDE_HITTABLE_H_

#include <glm/glm.hpp>

#include "ray.h"

namespace rt {

struct HitRecord {
  bool front_face;
  float t;
  glm::vec3 point;
  glm::vec3 normal;

  inline void SetFaceNormal(const Ray& ray, const glm::vec3& outward_normal) {
    front_face = glm::dot(ray.GetDirection(), outward_normal) < 0.f;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
 public:
  virtual ~Hittable() = default;

  virtual bool Hit(const Ray& ray, float t_min, float t_max,
                   HitRecord& record) const = 0;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_HITTABLE_H_