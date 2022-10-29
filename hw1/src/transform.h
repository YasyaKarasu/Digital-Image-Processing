#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "bmpimg.h"

typedef float YUV[3];

enum {
    YUV_Y = 0,
    YUV_U = 1,
    YUV_V = 2
};

RGBQUAD **Raw2RGB(BITMAPFILE *imgfp);

YUV **RGB2YUV(RGBQUAD **rgb, int w, int h);
RGBQUAD **YUV2RGB(YUV **yuv, int w, int h);

float **YUV2Gray(YUV **yuv, int w, int h);
void rescale(float **gray, int w, int h);
BITMAPFILE Gray2GrayscaleImg(float **gray, int w, int h);

BITMAPFILE RGB2BMP(RGBQUAD **rgb, int w, int h);

#endif