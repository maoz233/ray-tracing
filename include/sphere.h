/**
 * @file sphere.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_SPHERE_H_
#define RAY_TRACING_INCLUDE_SPHERE_H_

#include <glm/glm.hpp>

#include "hittable.h"
#include "ray.h"

namespace rt {

class Sphere : public Hittable {
 public:
  Sphere() = default;
  Sphere(glm::vec3 center, float radius);
  ~Sphere() = default;

  virtual bool Hit(const Ray& ray, float t_min, float t_max,
                   HitRecord& record) const override;

 private:
  float radius_;
  glm::vec3 center_;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_SPHERE_H_