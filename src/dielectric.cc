/**
 * @file dielectric.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "dielectric.h"

#include <cmath>

#define RAY_TRACING_INCLUDE_GLM
#include <glm/glm.hpp>

#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"

namespace rt {

Dielectric::Dielectric(float refraction_index)
    : refraction_index_{refraction_index} {}

bool Dielectric::Scatter(const Ray& ray, const HitRecord& record,
                         glm::vec3& attenuation, Ray& scattered) const {
  attenuation = glm::vec3(1.f, 1.f, 1.f);

  float refraction_ratio =
      record.front_face ? (1.f / refraction_index_) : refraction_index_;
  glm::vec3 unit_direction = glm::normalize(ray.GetDirection());

  float cos_theta = std::fmin(glm::dot(-unit_direction, record.normal), 1.f);
  float sin_theta = std::sqrtf(1.f - cos_theta * cos_theta);

  bool cannot_refract = refraction_ratio * sin_theta > 1.f;
  glm::vec3 direction{};

  if (cannot_refract || (Reflectance(cos_theta, refraction_ratio) >
                         Utils::RandomFloat())) {  // relfect
    direction = glm::reflect(unit_direction, record.normal);
  } else {  // refract
    direction = glm::refract(unit_direction, record.normal, refraction_ratio);
  }

  scattered = Ray(record.point, direction);

  return true;
}

float Dielectric::Reflectance(float cosine, float refraction_ratio) {
  float r = (1.f - refraction_ratio) / (1.f + refraction_ratio);
  r = r * r;

  return r + (1.f - r) * std::pow((1.f - cosine), 5.f);
}

}  // namespace rt
