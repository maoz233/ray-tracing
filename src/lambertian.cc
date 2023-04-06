/**
 * @file lambertian.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "lambertian.h"

#include <glm/glm.hpp>

#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"

namespace rt {

Lambertian::Lambertian(const glm::vec3& color) : albedo_{color} {}

bool Lambertian::Scatter(const Ray& ray, const HitRecord& record,
                         glm::vec3& attenuation, Ray& scattered) const {
  glm::vec3 scatter_direction =
      glm::reflect(glm::normalize(ray.GetDirection()), record.normal) +
      glm::normalize(Utils::RandomVec3());

  if (Utils::NearZero(scatter_direction)) {
    scatter_direction = record.normal;
  }

  scattered = Ray(record.point, scatter_direction);
  attenuation = albedo_;

  return true;
}

}  // namespace rt
