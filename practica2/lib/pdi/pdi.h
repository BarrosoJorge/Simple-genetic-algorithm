#ifndef PDI_H
#define PDI_H

typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned long int dword;

typedef struct {
    byte id[2];
    word offset;
    word ancho;
    word alto;
    byte bpp;
    int size;
    byte* head;
    float* imx;
} gcIMG;

gcIMG* gcGetImgBmp(char *ruta);
void gcPutImgBmp(char *ruta, gcIMG* img);
gcIMG* gcNewImg(int ancho, int alto);
void gcFreeImg(gcIMG* img);

#endif 