#include "math/dataset.h"
#include <cmath>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned char BYTE;
typedef enum { MAXIMIZAR, MINIMIZAR } TipoOptimizacion;

typedef struct {
  BYTE *cromosoma;
  vector<unsigned int> valores_enteros;
  vector<float> valores_reales;
  float valor_objetivo;
  float valor_aptitud;
  unsigned int padre_1;
  unsigned int padre_2;
} Individuo;

class algoritmogeneticsimple {
private:
  Individuo *poblacion;
  Individuo *poblacion_nueva;
  unsigned int *seleccion;
  unsigned int tamaño_cromosoma;
  unsigned int num_genes;
  vector<unsigned int> bits_por_gen;
  unsigned int tamaño_poblacion;
  vector<float> limites_superiores;
  vector<float> limites_inferiores;
  unsigned int id_mejor_objetivo;
  unsigned int id_peor_objetivo;
  float suma_objetivos;
  float promedio_objetivos;
  float suma_aptitud;
  float promedio_aptitud;
  float probabilidad_mutacion;
  float probabilidad_cruza;
  unsigned int max_gen;
  vector<float> X;
  vector<float> Y;
  unsigned int m;
  std::function<float(const std::vector<float> &)> funcion_objetivo;

public:
  algoritmogeneticsimple(unsigned int _tamaño_poblacion,
                         unsigned int _num_genes,
                         const vector<unsigned int> &_bits_por_gen,
                         const vector<float> &_limites_superiores,
                         const vector<float> &_limites_inferiores,
                         const float &probabilidad_cruza,
                         const float &probabilidad_mutacion,
                         const unsigned int &m, const unsigned int &max_gen);

  ~algoritmogeneticsimple();

  void DecodificarEnteros(void);
  void DecodificarReales(void);
  void ImprimirIndividuo(unsigned int _id_individuo);
  void ImprimirPoblacion(void);
  void EvaluarPoblacion(void);
  float FuncionObjetivo(unsigned int _id_individuo);
  void ConvertirObjetivoAptitud(TipoOptimizacion _tipo_optimizacion);
  void SeleccionRuleta(void);
  void CruzamientoPuntoUnico(double _probabilidad_cruza);
  void CruzamientoDoblePunto(double _probabilidad_cruza);
  void CruzamientoUniforme(double _probabilidad_cruza);
  void Mutar(double _probabilidad_mutacion);
  void GenerarProximaGeneracion(void);
  void Elitismo(void);
  unsigned int ObtenerMejor(void);
  float ObtenerMejorObjetivo(void);
  void fit(TipoOptimizacion _tipo_optimizacion = MINIMIZAR);
  void fit_gaussian(const DataSet &d);
  void torneo(const unsigned int &N);
  void mutaReal(double _probabilidad_mutacion, double delta);
  void CruzaReal(double _probabilidad_cruza);
  void set_funcion(std::function<float(const std::vector<float> &)> fn) {
    this->funcion_objetivo = fn;
  }
  //Devuelve los valores reales decodificados del mejor individuo
  vector<float> ObtenerValoresRealesMejor(void) {
    return this->poblacion[this->id_mejor_objetivo].valores_reales;
  }
}; // FIN DE LA CLASE