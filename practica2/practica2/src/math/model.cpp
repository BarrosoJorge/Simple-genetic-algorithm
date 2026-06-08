#include "math/model.hpp"
#include "math/gaussian.hpp"
#include <iostream>
Model create_gaussian_model(int m) {
  return [m](const std::vector<float> &params, float x) {
    float y = 0.0f;
    for (int i = 0; i < m; i++) {
      float lambda = params[3 * i];
      float c = params[3 * i + 1];
      float s = params[3 * i + 2];

      y += lambda * gaussian(x, c, s);
    }
    return y;
  };
}
