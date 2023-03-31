/**
 * @file ray.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "ray.h"

namespace rt {

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
    : origin_{origin}, direction_{direction} {}

glm::vec3 Ray::GetOrigin() const { return origin_; }

glm::vec3 Ray::GetDirection() const { return direction_; }

glm::vec3 Ray::At(float t) const { return origin_ + t * direction_; }

}  // namespace rt
