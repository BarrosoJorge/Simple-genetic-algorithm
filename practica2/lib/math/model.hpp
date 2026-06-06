
#include <functional>
using Model = std::function<float(const std::vector<float> &, float)>;
Model create_gaussian_model(int m);
