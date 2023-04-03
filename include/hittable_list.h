/**
 * @file hittable_list.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-04-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_HITTABLE_LIST_H_
#define RAY_TRACING_INCLUDE_HITTABLE_LIST_H_

#include <memory>
#include <vector>

#include "hittable.h"
#include "ray.h"

namespace rt {

class HittableList : public Hittable {
 public:
  HittableList() = default;
  HittableList(std::shared_ptr<Hittable> object);
  ~HittableList() = default;

  inline std::vector<std::shared_ptr<Hittable> > GetObjects();

  void Add(std::shared_ptr<Hittable> ojbect);
  void Clear();

  virtual bool Hit(const Ray& ray, float t_min, float t_max,
                   HitRecord& record) const override;

 private:
  std::vector<std::shared_ptr<Hittable> > objects_;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_HITTABLE_LIST_H_