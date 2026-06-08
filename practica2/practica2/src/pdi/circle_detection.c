#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "circle_detection.h"

PixelSet* obtenerPixelesNegros(gcIMG *img) {
    unsigned int i,j,k;
    unsigned int NumeroPixelesNegros = 0;

    PixelSet *p = (PixelSet*)malloc(sizeof(PixelSet));

    if(p == NULL) {
        return NULL;
    }

    /* Contar pixeles negros */
    for(i=0; i<img->alto; i++) {
        for(j=0; j<img->ancho; j++) {
            if(img->imx[i*img->ancho+j] == 0)
                NumeroPixelesNegros++;
        }
    }

    p->n = NumeroPixelesNegros;

    p->x = (int*)malloc(NumeroPixelesNegros*sizeof(int));
    p->y = (int*)malloc(NumeroPixelesNegros*sizeof(int));

    if(p->x == NULL || p->y == NULL) {
        free(p->x);
        free(p->y);
        free(p);
        return NULL;
    }
    k = 0;

    for(i=0; i<img->alto; i++) {
        for(j=0; j<img->ancho; j++) {
            if(img->imx[i*img->ancho+j] == 0) {
                p->x[k] = j;
                p->y[k] = i;
                k++;
            }
        }
    }
    return p;
}

void liberarPixelSet(PixelSet *p) {
    if(p == NULL)
        return;

    free(p->x);
    free(p->y);
    free(p);
}

void dibujarCirculo(gcIMG *img, float cx, float cy, float r) {
    int steps = (int)(2.0f * 3.14159265f * r);
    int i, px, py;
    float angle;
    if (steps < 360) steps = 360;
    for (i = 0; i < steps; i++) {
        angle = 2.0f * 3.14159265f * i / steps;
        px = (int)(cx + r * cosf(angle) + 0.5f);
        py = (int)(cy + r * sinf(angle) + 0.5f);
        if (px >= 0 && px < (int)img->ancho && py >= 0 && py < (int)img->alto)
            img->imx[py * img->ancho + px] = 128.0f;
    }
}