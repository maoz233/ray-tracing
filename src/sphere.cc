/**
 * @file sphere.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "sphere.h"

#include <cmath>

#define RAY_TRACING_INCLUDE_GLM
#include <glm/glm.hpp>

#include "hittable.h"
#include "ray.h"

namespace rt {

Sphere::Sphere(glm::vec3 center, float radius)
    : radius_{radius}, center_{center} {}

bool Sphere::Hit(const Ray& ray, float t_min, float t_max,
                 HitRecord& record) const {
  glm::vec3 oc = ray.GetOrigin() - center_;
  float a = glm::dot(ray.GetDirection(), ray.GetDirection());
  float half_b = glm::dot(oc, ray.GetDirection());
  float c = glm::dot(oc, oc) - radius_ * radius_;
  // discriminant = b * b - 4*a*c
  float discriminant = half_b * half_b - a * c;

  // no root
  if (discriminant < 0) {
    return false;
  }

  // root existed
  float sqrtd = std::sqrt(discriminant);
  float root = (-half_b - sqrtd) / a;

  // find nearst root that fall between acceptable range
  if (root < t_min || root > t_max) {
    root = (-half_b + sqrtd) / a;

    if (root < t_min || root > t_max) {
      return false;
    }
  }

  // save hit result
  record.t = root;
  record.point = ray.At(record.t);

  glm::vec3 outward_normal = (record.point - center_) / radius_;
  record.SetFaceNormal(ray, outward_normal);

  return true;
}

}  // namespace rt
