#ifndef CIRCLE_DETECTION_H
#define CIRCLE_DETECTION_H

#include "pdi.h"

typedef struct {
    int *x;
    int *y;
    int n;
} PixelSet;

PixelSet* obtenerPixelesNegros(gcIMG *img);
void liberarPixelSet(PixelSet *p);

#endif