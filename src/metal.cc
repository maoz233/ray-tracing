/**
 * @file metal.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "metal.h"

#include <glm/glm.hpp>

#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "utils.h"

namespace rt {

Metal::Metal(float fuzz, const glm::vec3& color)
    : fuzz_{fuzz}, albedo_{color} {}

bool Metal::Scatter(const Ray& ray, const HitRecord& record,
                    glm::vec3& attenuation, Ray& scattered) const {
  glm::vec3 reflection =
      glm::reflect(glm::normalize(ray.GetDirection()), record.normal);

  scattered =
      Ray(record.point, reflection + fuzz_ * Utils::RandomInUnitSphere());
  attenuation = albedo_;

  return (glm::dot(scattered.GetDirection(), record.normal) > 0.f);
}

}  // namespace rt
