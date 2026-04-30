#include "asignacion.hpp"
#include <string>
#include <cstdlib>
#include <iostream>

Programa Asignacion(int argc, char **argv) {
  Programa config;

  //  Valores por defecto 
  config.NUM_GENES        = 30;    // 30 dimensiones para la tarea
  config.TAMAÑO_POBLACION = 100;
  config.MAX_GENERACIONES = 500;
  config.FUNCION          = 1;     // f1 por defecto
  config.PROB_CRUZA       = 0.9;
  config.PROB_MUTACION    = 0.01;

  // Parsear argumentos de línea de comandos 
  for (int i = 1; i < argc; i++) {
    string arg = argv[i];

    if (arg == "-genes" && i + 1 < argc) {
      config.NUM_GENES = atoi(argv[++i]);
      config.BITS_POR_GEN.clear();
      config.LIMITE_SUPERIOR.clear();
      config.LIMITE_INFERIOR.clear();
    }
    else if (arg == "-bits" && i + 1 < argc) {
      config.BITS_POR_GEN.clear();
      i++;
      while (i < argc) {
        string next = argv[i];
        if (next[0] == '-' && !isdigit(next[1])) break;
        config.BITS_POR_GEN.push_back(atoi(argv[i++]));
      }
      i--;
    }
    else if (arg == "-poblacion" && i + 1 < argc) {
      config.TAMAÑO_POBLACION = atoi(argv[++i]);
    }
    else if (arg == "-generaciones" && i + 1 < argc) {
      config.MAX_GENERACIONES = atoi(argv[++i]);
    }
    else if (arg == "-funcion" && i + 1 < argc) {      
      config.FUNCION = atoi(argv[++i]);
    }
    else if (arg == "-cruza" && i + 1 < argc) {        
      config.PROB_CRUZA = atof(argv[++i]);
    }
    else if (arg == "-mutacion" && i + 1 < argc) {     
      config.PROB_MUTACION = atof(argv[++i]);
    }
    else if (arg == "-sup" && i + 1 < argc) {
      config.LIMITE_SUPERIOR.clear();
      i++;
      while (i < argc) {
        string next = argv[i];
        if (next[0] == '-' && !isdigit(next[1])) break;
        config.LIMITE_SUPERIOR.push_back(atof(argv[i++]));
      }
      i--;
    }
    else if (arg == "-inf" && i + 1 < argc) {
      config.LIMITE_INFERIOR.clear();
      i++;
      while (i < argc) {
        string next = argv[i];
        if (next[0] == '-' && !isdigit(next[1])) break;
        config.LIMITE_INFERIOR.push_back(atof(argv[i++]));
      }
      i--;
    }
  }

  // Completar vectores repitiendo el último valor ingresado 
  if (config.BITS_POR_GEN.size() < config.NUM_GENES) {
    unsigned int ultimo = config.BITS_POR_GEN.empty() ? 16u : config.BITS_POR_GEN.back();
    while (config.BITS_POR_GEN.size() < config.NUM_GENES)
      config.BITS_POR_GEN.push_back(ultimo);
  }

  
  if (config.LIMITE_SUPERIOR.empty() || config.LIMITE_INFERIOR.empty()) {
    float sup_default, inf_default;
    switch (config.FUNCION) {
      case 1:  sup_default =   5.12f; inf_default =  -5.12f; break;
      case 4:  sup_default = 100.0f;  inf_default = -100.0f; break;
      case 5:  sup_default =  30.0f;  inf_default =  -30.0f; break;
      case 9:  sup_default =   5.12f; inf_default =  -5.12f; break;
      case 10: sup_default =  32.0f;  inf_default =  -32.0f; break;
      case 12: sup_default =  50.0f;  inf_default =  -50.0f; break;
      default: sup_default =  10.0f;  inf_default =  -10.0f; break;
    }
    config.LIMITE_SUPERIOR.assign(config.NUM_GENES, sup_default);
    config.LIMITE_INFERIOR.assign(config.NUM_GENES, inf_default);
  } else {
    // Completar si el usuario dio menos valores que genes
    while (config.LIMITE_SUPERIOR.size() < config.NUM_GENES)
      config.LIMITE_SUPERIOR.push_back(config.LIMITE_SUPERIOR.back());
    while (config.LIMITE_INFERIOR.size() < config.NUM_GENES)
      config.LIMITE_INFERIOR.push_back(config.LIMITE_INFERIOR.back());
  }

  return config;
}