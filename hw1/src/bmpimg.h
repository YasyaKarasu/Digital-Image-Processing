#ifndef _BITIMG_H
#define _BITIMG_H

typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffbits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    unsigned int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;

typedef unsigned char RGBQUAD[4];

enum {
    RGBA_B = 0,
    RGBA_G = 1,
    RGBA_R = 2,
    RGBA_A = 3
};

typedef struct {
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    RGBQUAD aColors[256];
    unsigned char *aBitmapBits;
} BITMAPFILE;

#endif