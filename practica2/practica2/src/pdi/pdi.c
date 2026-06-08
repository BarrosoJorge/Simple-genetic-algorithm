#include<stdio.h>
#include<stdlib.h>

#include "pdi.h"

gcIMG* gcGetImgBmp(char *ruta) {
    gcIMG* img;
    FILE* file;
    int i, j, a, ar;

    // Abrir el archivo BMP
    if((file = fopen(ruta, "rb")) == NULL) {
        printf("Error al abrir el archivo.\n");
        exit(1);
    }

    // Reservar memoria para la estructura gcIMG
    if((img = (gcIMG*) calloc(1, sizeof(gcIMG))) == NULL) {
        printf("Error al reservar memoria para la imagen.\n");
        exit(1);
    }

    // Leer el encabezado del archivo BMP
    fread(img->id, 2, 1, file);      // Leer el identificador (2 bytes)
    fseek(file, 10, SEEK_SET);       // Mover el puntero al offset
    fread(&img->offset, 2, 1, file); // Leer el offset (2 bytes)
    fseek(file, 18, SEEK_SET);       // Mover el puntero al ancho
    fread(&img->ancho, 2, 1, file);  // Leer el ancho (2 bytes)
    fseek(file, 22, SEEK_SET);       // Mover el puntero al alto
    fread(&img->alto, 2, 1, file);   // Leer el alto (2 bytes)
    fseek(file, 28, SEEK_SET);       // Mover el puntero a los bits por píxel
    fread(&img->bpp, 1, 1, file);    // Leer los bits por píxel (1 byte)
    fseek(file, 34, SEEK_SET);       // Mover el puntero al tamaño de la imagen
    fread(&img->size, 4, 1, file);   // Leer el tamaño de la imagen (4 bytes)

    // Comprobar el formato de la imagen
    if((img->id[0] != 'B') || (img->id[1] != 'M')) {
        printf("El archivo no es un BMP válido.\n");
        free(img);
        exit(1);
    }

    // Asignar memoria para el encabezado
    if((img->head = (unsigned char*) malloc(img->offset)) == NULL) {
        printf("Error al reservar memoria para el encabezado de la imagen.\n");
        free(img);
        exit(1);
    }

    // Asignar memoria para los datos de la imagen
    if((img->imx = (float*) calloc(img->ancho * img->alto, sizeof(float))) == NULL) {
        printf("Error al reservar memoria para los datos de la imagen.\n");
        free(img->head);
        free(img);
        exit(1);
    }

    // Leer el encabezado completo
    rewind(file);
    fread(img->head, 1078, 1, file); 

    // Leer los datos de la imagen
    a = img->ancho;
    ar = img->size / img->alto; // Calcular el tamaño de cada fila
    fseek(file, img->offset, SEEK_SET); // Mover el puntero al inicio de la imagen 

    for(i = 0; i < img->alto; i++) {
        for(j = 0; j < img->ancho; j++) {
            img->imx[i * a + j] = (float) fgetc(file);
        }
        /* consumir bytes de relleno de fila (BMP padding a múltiplo de 4) */
        for(j = 0; j < (ar - a); j++) {
            fgetc(file);
        }
    }

    fclose(file); 
    img->size = img->ancho * img->alto; 
    return img;
}


void gcPutImgBmp(char *ruta, gcIMG* img) {
    FILE* file;
    int aux, zero = 0, i, j, offset, newAncho;

    // Crear el nuevo archivo BMP
    if((file = fopen(ruta, "w + b")) == NULL) {
        printf("Error al crear el archivo.\n");
        exit(1);
    }

    // Revisar el ancho de la imagen
    offset = img->ancho % 4;
    if(offset) {
        newAncho = img->ancho + (4 - offset);
    } else {
        newAncho = img->ancho;
    }

    // Revisar el encabezado del archivo BMP
    if (img->head) {
        // Actualizar tamaño del bitmap
        img->size = newAncho * img->alto;

        // Escribir encabezado existente (1078 bytes)
        fwrite(img->head, 1078, 1, file);
    } else {
        // Identificador BMP
        fputc('B', file);
        fputc('M', file);

        // Tamaño total del archivo
        aux = newAncho * img->alto + 1078;
        fwrite(&aux, 4, 1, file);

        // Campos reservados
        fwrite(&zero, 4, 1, file);

        // Offset de datos
        aux = 1078;
        fwrite(&aux, 4, 1, file);

        // Image Header
        aux = 40;  fwrite(&aux, 4, 1, file);   // Header Size
        aux = img->ancho; fwrite(&aux, 4, 1, file);   // Width
        aux = img->alto;  fwrite(&aux, 4, 1, file);   // Height

        aux = 1;   fwrite(&aux, 2, 1, file);   // Planes
        aux = 8;   fwrite(&aux, 2, 1, file);   // Bits por pixel
        aux = 0;   fwrite(&aux, 4, 1, file);   // Compression

        aux = newAncho * img->alto;
        fwrite(&aux, 4, 1, file);              // Bitmap Size

        aux = 0;
        fwrite(&aux, 4, 1, file);              // HResolution
        fwrite(&aux, 4, 1, file);              // VResolution

        aux = 256;
        fwrite(&aux, 4, 1, file);              // Colors used

        aux = 0;
        fwrite(&aux, 4, 1, file);              // Important Colors

        // Escribir la paleta de colores (256 colores)
        for(aux = 0; aux < 256; aux++) {
            for(i = 0; i < 3; i++) {
                fwrite(&aux, 1, 1, file); // R, G, B
                fwrite(&zero, 1, 1, file); // Reservado
            }
        }
    }

    // Escribir mapa de bits
    aux = img->ancho;
    for(i = 0; i < img->alto; i++) {
        for(j = 0; j < img->ancho; j++) {
            if(j > aux + 1) {
                fputc(0, file); // Rellenar con ceros si el ancho no es múltiplo de 4
            } else {
                fputc((unsigned char) img->imx[i * aux + j], file); // Escribir el valor del píxel
            }
        }
    }
    fclose(file);
}


gcIMG *gcNewImg(int ancho, int alto) {
    gcIMG *img;

    // Reservar memoria para la estructura gcIMG
    img = (gcIMG *) calloc(1, sizeof(gcIMG));
    if (img == NULL) {
        printf("Error al reservar memoria para gcIMG\n");
        exit(1);
    }

    // Inicializar atributos básicos
    img->ancho = ancho;
    img->alto  = alto;
    img->size  = ancho * alto;

    // Reservar memoria para la imagen (matriz de pixeles en float)
    img->imx = (float *) calloc(img->size, sizeof(float));
    if (img->imx == NULL) {
        printf("Error al reservar memoria para la Imagen\n");
        free(img);  // Liberar estructura antes de salir
        exit(1);
    }

    // Inicializar encabezado como NULL
    img->head = NULL;

    return img;
}


void gcFreeImg(gcIMG* img) {
    free(img->head); // Liberar memoria del encabezado
    free(img->imx);  // Liberar memoria de la imagen
    free(img);       // Liberar memoria de la estructura gcIMG
}