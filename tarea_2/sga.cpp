/* CÓDIGO FUENTE
   ALGORITMO GENETICO SIMPLE
   sga.cpp
   Implementación del algoritmo genético simple
   Dr. Carlos García
   Cómputo Evolutivo
   LIDIA
 */

#include "sga.hpp"

algoritmogeneticsimple::algoritmogeneticsimple(unsigned int _tamaño_poblacion,
                                               unsigned int _num_genes,
                                               const vector<unsigned int>& _bits_por_gen,
                                               const vector<float>& _limites_superiores,
                                               const vector<float>& _limites_inferiores,
                                               int _funcion_seleccionada) {
  this->tamaño_poblacion      = _tamaño_poblacion;
  this->num_genes             = _num_genes;
  this->bits_por_gen          = _bits_por_gen;
  this->limites_superiores    = _limites_superiores;
  this->limites_inferiores    = _limites_inferiores;
  this->funcion_seleccionada  = _funcion_seleccionada;  
  this->tamaño_cromosoma      = 0;

  // Calcular el tamaño del cromosoma
  for (unsigned int k = 0; k < this->num_genes; k++)
    this->tamaño_cromosoma += this->bits_por_gen[k];

  // Inicializar la población de individuos
  this->poblacion       = new Individuo[this->tamaño_poblacion];
  this->poblacion_nueva = new Individuo[this->tamaño_poblacion];
  this->seleccion       = new unsigned int[this->tamaño_poblacion];

  for (unsigned int k = 0; k < this->tamaño_poblacion; k++) {
    this->poblacion[k].cromosoma        = new BYTE[this->tamaño_cromosoma];
    this->poblacion[k].valores_enteros  = new unsigned int[this->num_genes];
    this->poblacion[k].valores_reales   = new float[this->num_genes];
    this->poblacion_nueva[k].cromosoma       = new BYTE[this->tamaño_cromosoma];
    this->poblacion_nueva[k].valores_enteros = new unsigned int[this->num_genes];
    this->poblacion_nueva[k].valores_reales  = new float[this->num_genes];

    // Inicializar el cromosoma aleatoriamente
    for (unsigned int i = 0; i < this->tamaño_cromosoma; i++)
      this->poblacion[k].cromosoma[i] = rand() % 2;

    for (unsigned int j = 0; j < this->num_genes; j++) {
      this->poblacion[k].valores_enteros[j] = 0;
      this->poblacion[k].valores_reales[j]  = 0;
    }
  }
}

algoritmogeneticsimple::~algoritmogeneticsimple() {
  for (unsigned int k = 0; k < this->tamaño_poblacion; k++) {
    delete[] this->poblacion[k].cromosoma;
    delete[] this->poblacion[k].valores_enteros;
    delete[] this->poblacion[k].valores_reales;
    delete[] this->poblacion_nueva[k].cromosoma;
    delete[] this->poblacion_nueva[k].valores_enteros;
    delete[] this->poblacion_nueva[k].valores_reales;
  }
  delete[] this->poblacion;
  delete[] this->poblacion_nueva;
  delete[] this->seleccion;
}


void algoritmogeneticsimple::DecodificarEnteros(void) {
  for (unsigned int k = 0; k < this->tamaño_poblacion; k++) {
    unsigned int indice_gen = 0;
    unsigned int acumulado  = this->bits_por_gen[0];
    unsigned int auxiliar   = 0;

    for (unsigned int i = 0, j = 0; i < this->tamaño_cromosoma; i++, j++) {
      auxiliar += this->poblacion[k].cromosoma[i] * (unsigned int)pow(2, j);

      if (i == (acumulado - 1)) {
        this->poblacion[k].valores_enteros[indice_gen] = auxiliar;
        auxiliar = 0;
        indice_gen++;
        if (indice_gen < this->num_genes)
          acumulado += this->bits_por_gen[indice_gen];
        j = -1;  // se incrementa a 0 en el for
      }
    }
  }
}

void algoritmogeneticsimple::DecodificarReales(void) {
  for (unsigned int k = 0; k < this->tamaño_poblacion; k++) {
    for (unsigned int g = 0; g < this->num_genes; g++) {
      float rango       = this->limites_superiores[g] - this->limites_inferiores[g];
      float denominador = (float)pow(2, this->bits_por_gen[g]) - 1.0f;
      this->poblacion[k].valores_reales[g] =
          ((this->poblacion[k].valores_enteros[g] / denominador) * rango)
          + this->limites_inferiores[g];
    }
  }
}

void algoritmogeneticsimple::ImprimirIndividuo(unsigned int _id_individuo) {
  int acumulado  = this->tamaño_cromosoma - 1;
  int indice_gen = this->num_genes - 1;
  cout << "[" << _id_individuo << "]";

  for (int i = this->tamaño_cromosoma - 1; i >= 0; i--) {
    if (i == acumulado) {
      cout << ":";
      acumulado -= this->bits_por_gen[indice_gen];
      indice_gen--;
    }
    cout << (int)this->poblacion[_id_individuo].cromosoma[i];
  }

  cout << " :";
  for (int j = this->num_genes - 1; j >= 0; j--)
    cout << this->poblacion[_id_individuo].valores_enteros[j] << ",";

  cout << " :";
  for (int j = this->num_genes - 1; j >= 0; j--)
    cout << this->poblacion[_id_individuo].valores_reales[j] << ",";

  cout << "Objetivo: " << this->poblacion[_id_individuo].valor_objetivo;
  cout << " Aptitud: " << this->poblacion[_id_individuo].valor_aptitud;
  cout << endl;
}

void algoritmogeneticsimple::ImprimirPoblacion(void) {
  for (unsigned int k = 0; k < this->tamaño_poblacion; k++)
    this->ImprimirIndividuo(k);

  cout << "ID Mejor Objetivo: "   << this->id_mejor_objetivo  << endl;
  cout << "ID Peor Objetivo: "    << this->id_peor_objetivo   << endl;
  cout << "Suma Objetivos: "      << this->suma_objetivos     << endl;
  cout << "Promedio Objetivos: "  << this->promedio_objetivos << endl;
  cout << "Suma Aptitud: "        << this->suma_aptitud       << endl;
  cout << "Promedio Aptitud: "    << this->promedio_aptitud   << endl;
}

void algoritmogeneticsimple::EvaluarPoblacion(void) {
  this->id_mejor_objetivo = 0;
  this->id_peor_objetivo  = 0;
  this->suma_objetivos    = 0;

  for (unsigned int k = 0; k < this->tamaño_poblacion; k++) {
    this->poblacion[k].valor_objetivo = this->FuncionObjetivo(k);

    // mejor = mayor valor_objetivo (antes de conversión de aptitud)
    if (this->poblacion[k].valor_objetivo >
        this->poblacion[this->id_mejor_objetivo].valor_objetivo)
      this->id_mejor_objetivo = k;

    if (this->poblacion[k].valor_objetivo <
        this->poblacion[this->id_peor_objetivo].valor_objetivo)
      this->id_peor_objetivo = k;

    this->suma_objetivos += this->poblacion[k].valor_objetivo;
  }

  this->promedio_objetivos = this->suma_objetivos / this->tamaño_poblacion;
}


void algoritmogeneticsimple::ConvertirObjetivoAptitud(TipoOptimizacion _tipo_optimizacion) {
  unsigned int k;
  float rango;
  this->suma_aptitud = 0;

  if (_tipo_optimizacion == MAXIMIZAR) {

    rango = this->poblacion[this->id_mejor_objetivo].valor_objetivo
          - this->poblacion[this->id_peor_objetivo].valor_objetivo;

    // guard contra división por cero (todos iguales)
    if (rango == 0.0f) {
      for (k = 0; k < this->tamaño_poblacion; k++) {
        this->poblacion[k].valor_aptitud = 100.0f;
        this->suma_aptitud += 100.0f;
      }
      this->promedio_aptitud = 100.0f;
      return;
    }

    for (k = 0; k < this->tamaño_poblacion; k++) {
      this->poblacion[k].valor_aptitud =
          100.0f * ((this->poblacion[k].valor_objetivo
                     - this->poblacion[this->id_peor_objetivo].valor_objetivo) / rango);
      this->suma_aptitud += this->poblacion[k].valor_aptitud;
    }
    this->promedio_aptitud = this->suma_aptitud / this->tamaño_poblacion;

  } else {  // MINIMIZAR

    for (k = 0; k < this->tamaño_poblacion; k++)
      this->poblacion[k].valor_aptitud =
          this->poblacion[this->id_mejor_objetivo].valor_objetivo
          - this->poblacion[k].valor_objetivo;

    
    unsigned int auxiliar       = this->id_mejor_objetivo;
    this->id_mejor_objetivo     = this->id_peor_objetivo;
    this->id_peor_objetivo      = auxiliar;

    // Normalizar aptitud a [0, 100]
    float minimo = this->poblacion[this->id_peor_objetivo].valor_aptitud;
    rango        = this->poblacion[this->id_mejor_objetivo].valor_aptitud - minimo;

    //guard contra división por cero
    if (rango == 0.0f) {
      for (k = 0; k < this->tamaño_poblacion; k++) {
        this->poblacion[k].valor_aptitud = 100.0f;
        this->suma_aptitud += 100.0f;
      }
      this->promedio_aptitud = 100.0f;
      return;
    }

    for (k = 0; k < this->tamaño_poblacion; k++) {
      this->poblacion[k].valor_aptitud =
          100.0f * ((this->poblacion[k].valor_aptitud - minimo) / rango);
      this->suma_aptitud += this->poblacion[k].valor_aptitud;
    }
    this->promedio_aptitud = this->suma_aptitud / this->tamaño_poblacion;
  }
}

void algoritmogeneticsimple::SeleccionRuleta(void) {
  unsigned int k, seleccionado;
  float suma_acumulada = 0, bolita_aleatoria;
  vector<float> probabilidad_acumulada(this->tamaño_poblacion);

  for (k = 0; k < this->tamaño_poblacion; k++) {
    suma_acumulada += this->poblacion[k].valor_aptitud / this->suma_aptitud;
    probabilidad_acumulada[k] = suma_acumulada;
  }

  for (k = 0; k < this->tamaño_poblacion; k++) {
    bolita_aleatoria = (float)rand() / RAND_MAX;
    seleccionado = 0;

    while (seleccionado < this->tamaño_poblacion - 1 &&
           probabilidad_acumulada[seleccionado] < bolita_aleatoria)
      seleccionado++;

    this->seleccion[k] = seleccionado;
  }
}


void algoritmogeneticsimple::CruzamientoPuntoUnico(double _probabilidad_cruza) {
  unsigned int i, k, punto_cruza, padre_1, padre_2;
  unsigned int limite = this->tamaño_cromosoma - 1;
  double valor_aleatorio;

  for (k = 0; k < this->tamaño_poblacion; k += 2) {
    valor_aleatorio = (double)rand() / RAND_MAX;

    if (valor_aleatorio < _probabilidad_cruza) {
      padre_1 = this->seleccion[k];
      padre_2 = this->seleccion[k + 1];
      this->poblacion_nueva[k].padre_1     = padre_1;
      this->poblacion_nueva[k].padre_2     = padre_2;
      this->poblacion_nueva[k+1].padre_1   = padre_2;
      this->poblacion_nueva[k+1].padre_2   = padre_1;
      punto_cruza = rand() % limite;

      for (i = 0; i <= punto_cruza; i++) {
        this->poblacion_nueva[k].cromosoma[i]   = this->poblacion[padre_1].cromosoma[i];
        this->poblacion_nueva[k+1].cromosoma[i] = this->poblacion[padre_2].cromosoma[i];
      }
      for (i = punto_cruza + 1; i < this->tamaño_cromosoma; i++) {
        this->poblacion_nueva[k].cromosoma[i]   = this->poblacion[padre_2].cromosoma[i];
        this->poblacion_nueva[k+1].cromosoma[i] = this->poblacion[padre_1].cromosoma[i];
      }
    } else {
      // Sin cruzamiento — copiar padres tal cual
      padre_1 = this->seleccion[k];
      padre_2 = this->seleccion[k + 1];
      this->poblacion_nueva[k].padre_1     = padre_1;
      this->poblacion_nueva[k].padre_2     = padre_1;
      this->poblacion_nueva[k+1].padre_1   = padre_2;
      this->poblacion_nueva[k+1].padre_2   = padre_2;

      for (i = 0; i < this->tamaño_cromosoma; i++) {
        this->poblacion_nueva[k].cromosoma[i]   = this->poblacion[padre_1].cromosoma[i];
        this->poblacion_nueva[k+1].cromosoma[i] = this->poblacion[padre_2].cromosoma[i];
      }
    }
  }
}

void algoritmogeneticsimple::Mutar(double _probabilidad_mutacion) {
  double valor_aleatorio;

  for (unsigned int k = 0; k < this->tamaño_poblacion; k++) {
    for (unsigned int i = 0; i < this->tamaño_cromosoma; i++) {
      valor_aleatorio = (double)rand() / RAND_MAX;
      if (valor_aleatorio < _probabilidad_mutacion)
        this->poblacion_nueva[k].cromosoma[i] =
            1 - this->poblacion_nueva[k].cromosoma[i];
    }
  }
}

void algoritmogeneticsimple::GenerarProximaGeneracion(void) {
  Individuo *auxiliar   = this->poblacion;
  this->poblacion       = this->poblacion_nueva;
  this->poblacion_nueva = auxiliar;
}

void algoritmogeneticsimple::Elitismo(void) {
  // el elitismo debe aplicarse ANTES del cruzamiento/mutación,
  // es decir aquí preservamos el mejor al slot 0 de poblacion_nueva.
  for (unsigned int i = 0; i < this->tamaño_cromosoma; i++)
    this->poblacion_nueva[0].cromosoma[i] =
        this->poblacion[this->id_mejor_objetivo].cromosoma[i];
}

unsigned int algoritmogeneticsimple::ObtenerMejor(void) {
  return this->id_mejor_objetivo;
}


float algoritmogeneticsimple::ObtenerMejorObjetivo(void) {
  // después de ConvertirObjetivoAptitud(MINIMIZAR),
  // id_mejor_objetivo ya apunta al individuo con MENOR valor_objetivo.
  return this->poblacion[this->id_mejor_objetivo].valor_objetivo;
}

float algoritmogeneticsimple::FuncionObjetivo(unsigned int _id_individuo) {
  float valor_objetivo = 0.0f;
  int   n              = this->num_genes;
  const float PI       = 3.14159265358979f;

  switch (this->funcion_seleccionada) {  // usa miembro de clase

    // ── f1: Sphere  
    // f1(x) = sum(xi^2),  rango [-5.12, 5.12],  minimo = 0
    case 1: {
      for (int i = 0; i < n; i++) {
        float xi = this->poblacion[_id_individuo].valores_reales[i];
        valor_objetivo += xi * xi;
      }
      break;
    }

    // ── f4: Quartic with noise  
    // f4(x) = max |xi|,  rango [-100, 100],  minimo = 0
    case 4: {
      float max_val = 0.0f;
      for (int i = 0; i < n; i++) {
        float abs_xi = std::fabs(this->poblacion[_id_individuo].valores_reales[i]);
        if (abs_xi > max_val) max_val = abs_xi;
      }
      valor_objetivo = max_val;
      break;
    }

    // ── f5: Rosenbrock  
    // f5(x) = sum(100*(xi+1 - xi^2)^2 + (xi-1)^2),  rango [-30, 30],  minimo = 0
    case 5: {
      for (int i = 0; i < n - 1; i++) {
        float xi      = this->poblacion[_id_individuo].valores_reales[i];
        float xi_next = this->poblacion[_id_individuo].valores_reales[i + 1];
        valor_objetivo += 100.0f * std::pow(xi_next - xi * xi, 2)
                        + std::pow(xi - 1.0f, 2);
      }
      break;
    }

    // ── f9: Rastrigin  
    // f9(x) = sum(xi^2 - 10*cos(2*pi*xi) + 10),  rango [-5.12, 5.12],  minimo = 0
    case 9: {
      for (int i = 0; i < n; i++) {
        float xi = this->poblacion[_id_individuo].valores_reales[i];
        valor_objetivo += xi * xi - 10.0f * std::cos(2.0f * PI * xi) + 10.0f;
      }
      break;
    }

    // ── f10: Ackley  
    // rango [-32, 32],  minimo = 0
    case 10: {
      float suma_cuad = 0.0f, suma_cos = 0.0f;
      for (int i = 0; i < n; i++) {
        float xi = this->poblacion[_id_individuo].valores_reales[i];
        suma_cuad += xi * xi;
        suma_cos  += std::cos(2.0f * PI * xi);
      }
      valor_objetivo = -20.0f * std::exp(-0.2f * std::sqrt(suma_cuad / n))
                     - std::exp(suma_cos / n)
                     + 20.0f + (float)M_E;
      break;
    }

    // ── f12: 
    // rango [-50, 50],  minimo = 0
    case 12: {
      auto calc_y = [](float x) { return 1.0f + 0.25f * (x + 1.0f); };
      auto calc_u = [](float x, float a, float b, float c) -> float {
        if (x >  a) return b * std::pow( x - a, c);
        if (x < -a) return b * std::pow(-x - a, c);
        return 0.0f;
      };

      float y_primero = calc_y(this->poblacion[_id_individuo].valores_reales[0]);
      float y_ultimo  = calc_y(this->poblacion[_id_individuo].valores_reales[n - 1]);

      float suma_y = 0.0f;
      for (int i = 0; i < n - 1; i++) {
        float yi      = calc_y(this->poblacion[_id_individuo].valores_reales[i]);
        float yi_next = calc_y(this->poblacion[_id_individuo].valores_reales[i + 1]);
        suma_y += std::pow(yi - 1.0f, 2)
                * (1.0f + 10.0f * std::pow(std::sin(PI * yi_next), 2));
      }

      float suma_u = 0.0f;
      for (int i = 0; i < n; i++)
        suma_u += calc_u(this->poblacion[_id_individuo].valores_reales[i],
                         10.0f, 100.0f, 4.0f);

      valor_objetivo = (PI / n) * (10.0f * std::pow(std::sin(PI * y_primero), 2)
                                   + suma_y
                                   + std::pow(y_ultimo - 1.0f, 2))
                     + suma_u;
      break;
    }

    default:
      cerr << "[ERROR] Función " << this->funcion_seleccionada << " no implementada." << endl;
      valor_objetivo = 0.0f;
      break;
  }

  return valor_objetivo;
}