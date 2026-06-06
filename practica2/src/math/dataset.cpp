
#include "math/dataset.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

std::vector<float> read_vector_txt(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("No se pudo  abrir el archivo");
  }
  std::vector<float> data;
  float value;

  while (file >> value) {
    data.push_back(value);
  }
  return data;
}
DataSet load_xy(const std::string &path_x, const std::string &path_y) {
  DataSet d;
  d.x = read_vector_txt(path_x);
  d.y = read_vector_txt(path_y);
  if (d.x.size() != d.y.size())
    throw std::runtime_error("X y Y tienen diferentes sizes");
  return d;
}
