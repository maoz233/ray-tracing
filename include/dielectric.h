/**
 * @file dieletric.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_DIELECTRIC_H_
#define RAY_TRACING_INCLUDE_DIELECTRIC_H_

#include <glm/glm.hpp>

#include "hittable.h"
#include "material.h"
#include "ray.h"

namespace rt {

class Dielectric : public Material {
 public:
  Dielectric(float refraction_index);
  ~Dielectric() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record,
                       glm::vec3& attenuation, Ray& scattered) const override;

 private:
  float refraction_index_;

 private:
  // User Schlick's approximation for reflectance
  static float Reflectance(float cosine, float refraction_ratio);
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_DIELECTRIC_H_
