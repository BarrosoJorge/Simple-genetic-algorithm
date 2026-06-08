#include "experiments/program.hpp"
#include "ga/sga.hpp"
extern "C" {
#include "pdi/pdi.h"
#include "pdi/circle_detection.h"
}

#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//Parametros de AG
const int    POBLACION    = 200;
const int    GENERACIONES = 500;
const double PC = 0.9;
const double PM = 0.003;
const int    BITS = 16;
const int    NUM_RUNS = 100;
const float  TOLERANCIA = 1.5f; // píxeles de margen sobre el perímetro
const int    N_SECTORES = 12;  // sectores angulares para medir uniformidad

int main()
{
    srand((unsigned int)time(nullptr));
    struct ImageInfo { const char *ruta; const char *nombre; };
    vector<ImageInfo> imagenes = {
        { "data/4_img.bmp", "C01.bmp"  },
        { "data/5_img.bmp", "C02.bmp"  },
        { "data/6_img.bmp", "C03.bmp"  },
        { "data/7_img.bmp", "C04.bmp"  },
        { "data/8_img.bmp", "C05.bmp"  },
        { "data/3_img.bmp", "auto.bmp" },
        { "data/9_img.bmp", "lamp.bmp" },
    };

    ofstream csv("resultados_circulos.csv");
    csv << "imagen,run,cx,cy,r,score\n";

    cout << "Pob=" << POBLACION << " Gen=" << GENERACIONES
         << " Pc=" << PC << " Pm=" << PM
         << " Bits=" << BITS << " Tol=" << TOLERANCIA << "\n\n";

    for (auto &info : imagenes)
    {
        gcIMG *img = gcGetImgBmp((char *)info.ruta);
        if (!img) { cerr << "Error al cargar: " << info.ruta << "\n"; continue; }

        PixelSet *pixels = obtenerPixelesNegros(img);
        cout <<info.nombre << " (" << img->ancho << "x" << img->alto << ")"<< "  px_negros=" << pixels->n << "\n";

        double sum_cx=0, sum_cy=0, sum_r=0, sum_s=0;
        double sum_cx2=0, sum_cy2=0, sum_r2=0;
        float best_score_run = -1.0f;
        float best_cx_run = 0.0f, best_cy_run = 0.0f, best_r_run = 0.0f;
        for (int run = 1; run <= NUM_RUNS; run++)
        {
            srand((unsigned)(time(nullptr) + run * 999983u));

            unsigned int NG = 3;
            vector<unsigned int> bits_gen(NG, (unsigned int)BITS);

            float r_max = (float)(min((int)img->ancho, (int)img->alto)) / 2.0f;
            vector<float> lsup = { (float)(img->ancho - 1), (float)(img->alto - 1), r_max };
            vector<float> linf = { 0.0f, 0.0f, 1.0f };

            algoritmogeneticsimple ga(
                (unsigned int)POBLACION, NG, bits_gen,
                lsup, linf, (float)PC, (float)PM,
                1, (unsigned int)GENERACIONES
            );

            // Función objetivo: cobertura normalizada × uniformidad angular
            // cobertura  = conteo / (2πr)           → penaliza círculos grandes vacíos
            // uniformidad = sectores con píxeles / 12 → penaliza arcos parciales
            ga.set_funcion([pixels](const vector<float> &genes) -> float {
                float cx = genes[0], cy = genes[1], r = genes[2];
                if (r < 1.0f) return 0.0f;

                const float PI2 = 2.0f * 3.14159265f;
                int sectores[12] = {};
                int conteo = 0;

                for (int k = 0; k < pixels->n; k++) {
                    float dx   = (float)pixels->x[k] - cx;
                    float dy   = (float)pixels->y[k] - cy;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (fabsf(dist - r) <= TOLERANCIA) {
                        conteo++;
                        float angulo = atan2f(dy, dx);
                        if (angulo < 0.0f) angulo += PI2;
                        int s = (int)(angulo * N_SECTORES / PI2);
                        if (s >= N_SECTORES) s = N_SECTORES - 1;
                        sectores[s]++;
                    }
                }

                // mayor hueco angular consecutivo (traversal circular)
                int max_gap = N_SECTORES;
                for (int s = 0; s < N_SECTORES; s++) {
                    if (sectores[s] > 0) {
                        max_gap = 0;
                        int gap = 0;
                        for (int i = 1; i <= N_SECTORES; i++) {
                            if (sectores[(s + i) % N_SECTORES] == 0) {
                                gap++;
                            } else {
                                if (gap > max_gap) max_gap = gap;
                                gap = 0;
                            }
                        }
                        if (gap > max_gap) max_gap = gap;
                        break;
                    }
                }

                float cobertura   = fminf((float)conteo / (PI2 * r), 1.0f);
                float uniformidad = 1.0f - (float)max_gap / N_SECTORES;

                return -((float)conteo * cobertura * uniformidad);
            });

            ga.fit(MINIMIZAR);

            vector<float> best = ga.ObtenerValoresRealesMejor();
            float score = -ga.ObtenerMejorObjetivo();  // positivo
            float cx = best[0], cy = best[1], r = best[2];

            csv << info.nombre << "," << run << "," << cx << "," << cy << "," << r << "," << score << "\n";

            sum_cx += cx; sum_cx2 += cx * cx;
            sum_cy += cy; sum_cy2 += cy * cy;
            sum_r  += r; sum_r2  += r  * r;
            sum_s  += score;

            if (score > best_score_run) {
                best_score_run = score;
                best_cx_run = cx; best_cy_run = cy; best_r_run = r;
            }

            cout << "Ejecución no: " << run << "/" << NUM_RUNS << "  cx=" << cx << " cy=" << cy << " r=" << r << " score=" << score << "\n";
        }

        double n   = NUM_RUNS;
        double mcx = sum_cx / n, mcy = sum_cy / n, mr = sum_r / n;
        double scx = sqrt(max(0.0, sum_cx2/n - mcx*mcx));
        double scy = sqrt(max(0.0, sum_cy2/n - mcy*mcy));
        double sr  = sqrt(max(0.0, sum_r2 /n - mr *mr));

        cout << "\nRESULTADO (" << info.nombre << "):\n"
             << "cx = " << mcx << " +/- " << scx << "\n"
             << "cy = " << mcy << " +/- " << scy << "\n"
             << "r  = " << mr  << " +/- " << sr  << "\n"
             << "score promedio = " << sum_s / n << "\n"
             << "mejor run: cx=" << best_cx_run << " cy=" << best_cy_run << " r=" << best_r_run << " score=" << best_score_run << "\n\n";

        dibujarCirculo(img, best_cx_run, best_cy_run, best_r_run);
        string out_ruta = string("resultado_") + info.nombre;
        gcPutImgBmp((char*)out_ruta.c_str(), img);
        cout << "Imagen guardada: " << out_ruta << "\n\n";

        liberarPixelSet(pixels);
        gcFreeImg(img);
    }
    csv.close();
    cout << "Resultados guardados\n";
    return 0;
}