#ifndef PDI_H
#define PDI_H

typedef unsigned char gc_byte;
typedef unsigned short int word;
typedef unsigned long int dword;

typedef struct {
    gc_byte id[2];
    word offset;
    word ancho;
    word alto;
    gc_byte bpp;
    int size;
    gc_byte* head;
    float* imx;
} gcIMG;

gcIMG* gcGetImgBmp(char *ruta);
void gcPutImgBmp(char *ruta, gcIMG* img);
gcIMG* gcNewImg(int ancho, int alto);
void gcFreeImg(gcIMG* img);

#endif