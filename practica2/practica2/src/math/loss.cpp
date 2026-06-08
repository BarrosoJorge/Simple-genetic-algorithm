#include "math/loss.hpp"

std::function<float(const std::vector<float> &)>
create_mse(const DataSet &dataset, Model model) {
  return [dataset, model](const std::vector<float> &params) {
    float error = 0.0f;
    int n = dataset.x.size();
    for (int i = 0; i < n; i++) {
      float y_pred = model(params, dataset.x[i]);
      float diff = y_pred - dataset.y[i];
      error += diff * diff;
    }
    return error / n;
  };
}
