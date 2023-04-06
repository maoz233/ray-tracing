/**
 * @file material.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_MATERIAL_H_
#define RAY_TRACING_INCLUDE_MATERIAL_H_

#include <glm/glm.hpp>

#include "hittable.h"
#include "ray.h"

namespace rt {

class Material {
 public:
  Material() = default;
  virtual ~Material() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record,
                       glm::vec3& attenuation, Ray& scattered) const = 0;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_MATERIAL_H_