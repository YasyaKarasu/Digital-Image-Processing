#include <stdlib.h>
#include <string.h>
#include "bmpimg.h"
#include "transform.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define REDMASK 0xF800
#define GREENMASK 0x07E0
#define BLUEMASK 0x001F

RGBQUAD **Raw2RGB(BITMAPFILE *imgfp){
    int w = imgfp->bmih.biWidth, h = abs(imgfp->bmih.biHeight);
    int BytesPerPixel = imgfp->bmih.biBitCount >> 3;
    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * w);
    switch (imgfp->bmih.biBitCount){
        case 24:
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++){
                    unsigned char *p = (imgfp->aBitmapBits) + BytesPerPixel * (i * w + j);
                    res[i][j][RGBA_B] = *p;
                    res[i][j][RGBA_G] = *(p + 1);
                    res[i][j][RGBA_R] = *(p + 2);
                    res[i][j][RGBA_A] = 0;
                }
            break;
        case 32:
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++){
                    unsigned char *p = (imgfp->aBitmapBits) + BytesPerPixel * (i * w + j);
                    res[i][j][RGBA_B] = *p;
                    res[i][j][RGBA_G] = *(p + 1);
                    res[i][j][RGBA_R] = *(p + 2);
                    res[i][j][RGBA_A] = *(p + 3);
                }
            break;
        case 8:
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++){
                    unsigned char *p = (imgfp->aBitmapBits) + (i * w + j);
                    memcpy(res[i][j], imgfp->aColors[*p], 4);
                }
            break;
        case 16:
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++){
                    unsigned char *p = (imgfp->aBitmapBits) + BytesPerPixel * (i * w + j);
                    unsigned short cur = 256 * (unsigned short)(*(p + 1)) + (unsigned short)(*p);
                    res[i][j][RGBA_R] = (cur & REDMASK) >> 8;
                    res[i][j][RGBA_G] = (cur & GREENMASK) >> 3;
                    res[i][j][RGBA_B] = (cur & BLUEMASK) << 3;
                    res[i][j][RGBA_A] = 0;
                }
            break;
    }
    return res;
}

YUV **RGB2YUV(RGBQUAD **rgb, int w, int h){
    YUV **res = (YUV**)malloc(sizeof(YUV*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (YUV*)malloc(sizeof(YUV) * w);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            res[i][j][YUV_Y] = 0.299 * (float)rgb[i][j][RGBA_R] + 0.587 * (float)rgb[i][j][RGBA_G] + 0.114 * (float)rgb[i][j][RGBA_B];
            res[i][j][YUV_V] = 0.5 * (float)rgb[i][j][RGBA_R] - 0.419 * (float)rgb[i][j][RGBA_G] - 0.081 * rgb[i][j][RGBA_B] + 128;
            res[i][j][YUV_U] = -0.169 * (float)rgb[i][j][RGBA_R] - 0.331 * (float)rgb[i][j][RGBA_G] + 0.5 * (float)rgb[i][j][RGBA_B] + 128;
        }
    return res;
}

float **YUV2Gray(YUV **yuv, int w, int h){
    float **res = (float**)malloc(sizeof(float*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (float*)malloc(sizeof(float) * w);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            res[i][j] = yuv[i][j][YUV_Y];
    return res;
}

static unsigned char rgbRound(float x){
    if (x <= 0)
        return 0;
    if (x >= 255)
        return 255;
    return (unsigned char)x;
}

RGBQUAD **YUV2RGB(YUV **yuv, int w, int h){
    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * w);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            res[i][j][RGBA_R] = rgbRound(yuv[i][j][YUV_Y] + 1.403 * (yuv[i][j][YUV_V] - 128));
            res[i][j][RGBA_G] = rgbRound(yuv[i][j][YUV_Y] - 0.344 * (yuv[i][j][YUV_U] - 128) - 0.714 * (yuv[i][j][YUV_V] - 128));
            res[i][j][RGBA_B] = rgbRound(yuv[i][j][YUV_Y] + 1.770 * (yuv[i][j][YUV_U] - 128));
            res[i][j][RGBA_A] = 0;
        }
    return res;
}

void rescale(float **gray, int w, int h){
    float gmin = 255, gmax = 0;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            gmin = min(gmin, gray[i][j]);
            gmax = max(gmax, gray[i][j]);
        }
    float delta = gmax - gmin;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            gray[i][j] = (gray[i][j] - gmin) / delta * 255;
    return;
}

BITMAPFILE Gray2GrayscaleImg(float **gray, int w, int h){
    BITMAPFILE grayimg;

    grayimg.bmfh.bfType = 19778;
    grayimg.bmfh.bfReserved1 = 0;
    grayimg.bmfh.bfReserved2 = 0;
    grayimg.bmfh.bfOffbits = 1078;

    grayimg.bmih.biSize = 40;
    grayimg.bmih.biWidth = w;
    grayimg.bmih.biHeight = -h;
    grayimg.bmih.biPlanes = 1;
    grayimg.bmih.biBitCount = 8;
    grayimg.bmih.biCompression = 0;
    grayimg.bmih.biSizeImage = 0;
    grayimg.bmih.biXPelsPerMeter = 0;
    grayimg.bmih.biXPelsPerMeter = 0;
    grayimg.bmih.biClrUsed = 256;
    grayimg.bmih.biClrImportant = 256;

    int RowSize = ((8 * grayimg.bmih.biWidth + 31) >> 5) << 2;
    int skip = 4 - (((8 * grayimg.bmih.biWidth) >> 3) & 3);
    if (skip == 4)
        skip = 0;
    grayimg.bmfh.bfSize = 54 + RowSize * abs(grayimg.bmih.biHeight);

    RGBQUAD palette[256];
    for (int i = 0; i < 256; i++){
        palette[i][RGBA_R] = i;
        palette[i][RGBA_G] = i;
        palette[i][RGBA_B] = i;
        palette[i][RGBA_A] = 0;
    }
    memcpy(grayimg.aColors, palette, sizeof(palette));

    grayimg.aBitmapBits = (unsigned char*)malloc(grayimg.bmih.biWidth * abs(grayimg.bmih.biHeight));
    unsigned char *p = grayimg.aBitmapBits;
    for (int i = 0; i < abs(grayimg.bmih.biHeight); i++)
        for (int j = 0; j < grayimg.bmih.biWidth; j++){
            *p = (unsigned char)gray[i][j];
            p++;
        }
    return grayimg;
}

BITMAPFILE RGB2BMP(RGBQUAD **rgb, int w, int h){
    BITMAPFILE img;
    img.bmfh.bfType = 19778;
    img.bmfh.bfReserved1 = 0;
    img.bmfh.bfReserved2 = 0;
    img.bmfh.bfOffbits = 54;

    img.bmih.biSize = 40;
    img.bmih.biWidth = w;
    img.bmih.biHeight = -h;
    img.bmih.biPlanes = 1;
    img.bmih.biBitCount = 24;
    img.bmih.biCompression = 0;
    img.bmih.biSizeImage = 0;
    img.bmih.biXPelsPerMeter = 0;
    img.bmih.biXPelsPerMeter = 0;
    img.bmih.biClrUsed = 0;
    img.bmih.biClrImportant = 0;

    memset(img.aColors, 0, sizeof(img.aColors));

    int RowSize = ((24 * img.bmih.biWidth + 31) >> 5) << 2;
    int skip = 4 - (3 * img.bmih.biWidth) & 3;
    if (skip == 4)
        skip = 0;
    img.bmfh.bfSize = 54 + 3 * RowSize * abs(img.bmih.biHeight);

    img.aBitmapBits = (unsigned char*)malloc(img.bmfh.bfSize);
    unsigned char *p = img.aBitmapBits;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            *(p++) = rgb[i][j][RGBA_B];
            *(p++) = rgb[i][j][RGBA_G];
            *(p++) = rgb[i][j][RGBA_R];
        }
    return img;
}