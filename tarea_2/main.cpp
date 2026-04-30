#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <numeric>
#include "sga.hpp"
#include "asignacion.hpp"

using namespace std;

int main(int argc, char *argv[]) {

  //  Configuración desde argumentos de línea de comandos 
  Programa config = Asignacion(argc, argv);

  const int total_ejecuciones = 100;
  vector<float> mejores_resultados;
  mejores_resultados.reserve(total_ejecuciones);

  // Imprimir cabecera informativa en stderr
  cerr << "=== Algoritmo Genético Simple ===" << endl;
  cerr << "Función benchmark : f" << config.FUNCION          << endl;
  cerr << "Dimensiones       : " << config.NUM_GENES          << endl;
  cerr << "Tamaño población  : " << config.TAMAÑO_POBLACION   << endl;
  cerr << "Generaciones      : " << config.MAX_GENERACIONES   << endl;
  cerr << "Prob. cruza       : " << config.PROB_CRUZA         << endl;
  cerr << "Prob. mutación    : " << config.PROB_MUTACION       << endl;
  cerr << "Iniciando " << total_ejecuciones << " ejecuciones..." << endl;

  
  cout << "GEN,CORRIDA,MEJOR" << endl;

  //  Bucle de 100 ejecuciones 
  for (int corrida = 0; corrida < total_ejecuciones; corrida++) {

    // Semilla distinta y reproducible por corrida
    srand((unsigned int)(time(NULL)) + corrida * 1000u);

    algoritmogeneticsimple ag(config.TAMAÑO_POBLACION,
                              config.NUM_GENES,
                              config.BITS_POR_GEN,
                              config.LIMITE_SUPERIOR,
                              config.LIMITE_INFERIOR,
                              config.FUNCION);          

    // Evaluar población inicial
    ag.DecodificarEnteros();
    ag.DecodificarReales();
    ag.EvaluarPoblacion();
    ag.ConvertirObjetivoAptitud(MINIMIZAR);

    // Imprimir generación 0 (población inicial)
    cout << 0 << "," << corrida << "," << ag.ObtenerMejorObjetivo() << "\n";

    //  Bucle de generaciones 
    for (unsigned int gen = 1; gen <= config.MAX_GENERACIONES; gen++) {
      ag.SeleccionRuleta();
      ag.CruzamientoPuntoUnico(config.PROB_CRUZA);
      ag.Mutar(config.PROB_MUTACION);
      ag.Elitismo();
      ag.GenerarProximaGeneracion();
      ag.DecodificarEnteros();
      ag.DecodificarReales();
      ag.EvaluarPoblacion();
      ag.ConvertirObjetivoAptitud(MINIMIZAR);

      
      cout << gen << "," << corrida << "," << ag.ObtenerMejorObjetivo() << "\n";
    }

    float mejor = ag.ObtenerMejorObjetivo();
    mejores_resultados.push_back(mejor);

    if ((corrida + 1) % 10 == 0)
      cerr << "  Completadas " << corrida + 1 << " ejecuciones..." << endl;
  }

  //  Estadísticas finales 
  float suma     = accumulate(mejores_resultados.begin(), mejores_resultados.end(), 0.0f);
  float promedio = suma / total_ejecuciones;

  float suma_dif = 0.0f;
  for (float r : mejores_resultados)
    suma_dif += pow(r - promedio, 2);

  float desviacion = sqrt(suma_dif / total_ejecuciones);

  cerr << "\n=== RESULTADOS FINALES (100 ejecuciones) ===" << endl;
  cerr << "Promedio (Mean):              " << promedio   << endl;
  cerr << "Desviacion Estandar (StdDev): " << desviacion << endl;

  
  cout << "# MEAN=" << promedio << " STDDEV=" << desviacion << endl;

  return 0;
}