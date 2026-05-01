#include "experiments/program.hpp"
#include "ga/sga.hpp"
#include "math/dataset.h"
#include <string>
int main(int argc, char *argv[]) {
  DataSet d = load_xy("../data/1_xi.txt", "../data/1_yi.txt");
  Programa experimentos = Asignacion(argc, argv);
  std::cout << "si acabo";
  algoritmogeneticsimple ga = algoritmogeneticsimple(
      experimentos.TAMAÑO_POBLACION, experimentos.NUM_GENES,
      experimentos.BITS_POR_GEN, experimentos.LIMITE_SUPERIOR,
      experimentos.LIMITE_INFERIOR, experimentos.PROB_CRUZA,
      experimentos.PROB_MUTACION, experimentos.m,
      experimentos.MAX_GENERACIONES);
  std::cout << "si inicializo";

  ga.fit_gaussian(d);
}
