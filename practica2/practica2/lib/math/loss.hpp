#pragma once
#include "math/dataset.h"
#include "math/model.hpp"
#include <functional>
#include <vector>
std::function<float(const std::vector<float> &)>
create_mse(const DataSet &dataset, Model model);
