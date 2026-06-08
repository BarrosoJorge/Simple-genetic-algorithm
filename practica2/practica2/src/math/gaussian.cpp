#include "math/gaussian.hpp"
#include <cmath>
float gaussian(float x, float c, float sigma) {
  float s = std::max(sigma, 1e-6f);
  return std::exp(-((x - c) * (x - c)) / (2 * s * s));
}
