#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
typedef struct {
  std::vector<float> x;
  std::vector<float> y;
} DataSet;
std::vector<float> read_vector_txt(const std::string &path);
DataSet load_xy(const std::string &path_x, const std::string &path_y);
