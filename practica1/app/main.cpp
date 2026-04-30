#include "math/dataset.h"
#include <string>
int main() {
  DataSet d = load_xy("../data/1_xi.txt", "../data/1_yi.txt");
  for (const auto &x : d.x) {
    std::cout << x << std::endl;
  }
}
