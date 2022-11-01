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

BITMAPFILE RGB2BMP(RGBQUAD **rgb, int w, int h);

#endif