/**
 * @file hittable_list.cc
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hittable_list.h"

#include <memory>
#include <vector>

#include "hittable.h"
#include "ray.h"

namespace rt {

HittableList::HittableList(std::shared_ptr<Hittable> object) { Add(object); }

std::vector<std::shared_ptr<Hittable> > HittableList::GetObjects() {
  return objects_;
}

void HittableList::Add(std::shared_ptr<Hittable> object) {
  objects_.push_back(object);
}

void HittableList::Clear() { objects_.clear(); }

bool HittableList::Hit(const Ray& ray, float t_min, float t_max,
                       HitRecord& record) const {
  bool hit_anything = false;
  float closest_so_far = t_max;
  HitRecord temp_record{};

  for (const auto& object : objects_) {
    if (object->Hit(ray, t_min, closest_so_far, temp_record)) {
      hit_anything = true;
      closest_so_far = temp_record.t;
      record = temp_record;
    }
  }

  return hit_anything;
}

}  // namespace rt
