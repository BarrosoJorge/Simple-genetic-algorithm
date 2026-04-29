#pragma once
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

struct Programa {
  unsigned int NUM_GENES;
  vector<unsigned int> BITS_POR_GEN;
  unsigned int TAMAÑO_POBLACION;
  unsigned int MAX_GENERACIONES;
  vector<float> LIMITE_SUPERIOR;
  vector<float> LIMITE_INFERIOR;
  int FUNCION;               
  double PROB_CRUZA;         
  double PROB_MUTACION;
};

Programa Asignacion(int argc, char *argv[]);