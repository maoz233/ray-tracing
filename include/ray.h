/**
 * @file ray.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_RAY_H_
#define RAY_TRACING_INCLUDE_RAY_H_

#include <cstdint>

#define RAY_TRACING_INCLUDE_GLM
#include <glm/glm.hpp>

namespace rt {

class Ray {
 public:
  Ray() = default;
  Ray(const glm::vec3& origin, const glm::vec3& direction);
  ~Ray() = default;

  glm::vec3 GetOrigin() const;
  glm::vec3 GetDirection() const;

  glm::vec3 At(float t) const;

 private:
  glm::vec3 origin_;
  glm::vec3 direction_;
};

}  // namespace rt

#endif
