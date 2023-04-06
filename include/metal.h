/**
 * @file metal.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_METAL_H_
#define RAY_TRACING_INCLUDE_METAL_H_

#include <glm/glm.hpp>

#include "hittable.h"
#include "material.h"
#include "ray.h"

namespace rt {

class Metal : public Material {
 public:
  Metal(float fuzz, const glm::vec3& color);
  ~Metal() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record,
                       glm::vec3& attenuation, Ray& scattered) const;

 private:
  float fuzz_;
  glm::vec3 albedo_;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_METAL_H_
