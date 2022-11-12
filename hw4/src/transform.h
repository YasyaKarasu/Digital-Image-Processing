#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "bmpimg.h"

typedef float YUV[3];

enum {
    YUV_Y = 0,
    YUV_U = 1,
    YUV_V = 2
};

typedef unsigned char GRAYSCALE;

RGBQUAD **Raw2RGB(BITMAPFILE *imgfp);

YUV **RGB2YUV(RGBQUAD **rgb, int w, int h);
RGBQUAD **YUV2RGB(YUV **yuv, int w, int h);

GRAYSCALE **YUV2Gray(YUV **yuv, int w, int h);
GRAYSCALE **RGB2Gray(RGBQUAD **rgb, int w, int h);
void rescale(GRAYSCALE **gray, int w, int h);
BITMAPFILE Gray2GrayscaleImg(GRAYSCALE **gray, int w, int h);

GRAYSCALE GrayThresh(GRAYSCALE **gray, int w, int h);
GRAYSCALE **GlobalBinarization(GRAYSCALE **gray, int w, int h);
GRAYSCALE **LocalAdaptiveBinarization(GRAYSCALE **gray, int w, int h);

GRAYSCALE **Dilation(GRAYSCALE **gray, int w, int h);
GRAYSCALE **Erosion(GRAYSCALE **gray, int w, int h);
GRAYSCALE **Opening(GRAYSCALE **gray, int w, int h);
GRAYSCALE **Closing(GRAYSCALE **gray, int w, int h);

RGBQUAD **ToneMapping(RGBQUAD **rgb, int w, int h);
RGBQUAD **HistogramEqualization(RGBQUAD **rgb, int w, int h);

RGBQUAD **Translation(RGBQUAD **rgb, int w, int h, int delta_w, int delta_h);
RGBQUAD **Rotation(RGBQUAD **rgb, int w, int h, double theta, int *new_w, int *new_h);
RGBQUAD **Scale(RGBQUAD **rgb, int w, int h, double coef);

typedef enum {
    DX = 0,
    DY = 1
}DIR;

RGBQUAD **Shear(RGBQUAD **rgb, int w, int h, DIR dir, double coef);
RGBQUAD **Mirror(RGBQUAD **rgb, int w, int h, DIR dir);

BITMAPFILE RGB2BMP(RGBQUAD **rgb, int w, int h);

#endif