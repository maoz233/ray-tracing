/**
 * @file layer.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_LAYER_H_
#define RAY_TRACING_INCLUDE_LAYER_H_

namespace rt {
class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnUIRender() = 0;
};

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_LAYER_H_
