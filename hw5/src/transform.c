#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bmpimg.h"
#include "transform.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x) > 0 ? (x) : (-(x)))
#define REDMASK 0xF800
#define GREENMASK 0x07E0
#define BLUEMASK 0x001F
#define esp 0.00005

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

GRAYSCALE **YUV2Gray(YUV **yuv, int w, int h){
    GRAYSCALE **res = (GRAYSCALE**)malloc(sizeof(GRAYSCALE*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (GRAYSCALE*)malloc(sizeof(GRAYSCALE) * w);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            res[i][j] = (unsigned char)yuv[i][j][YUV_Y];
    return res;
}

GRAYSCALE **RGB2Gray(RGBQUAD **rgb, int w, int h){
    YUV **yuv = RGB2YUV(rgb, w, h);
    GRAYSCALE **res = YUV2Gray(yuv, w, h);
    free(yuv);
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

void Rescale(GRAYSCALE **gray, int w, int h){
    float gmin = 255, gmax = 0;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            gmin = min(gmin, gray[i][j]);
            gmax = max(gmax, gray[i][j]);
        }
    float delta = gmax - gmin;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            gray[i][j] = (unsigned char)(((float)gray[i][j] - gmin) / delta * 255);
    return;
}

BITMAPFILE Gray2GrayscaleImg(GRAYSCALE **gray, int w, int h){
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

GRAYSCALE GrayThresh(GRAYSCALE **gray, int w, int h){
    int PixelNum = w * h;
    GRAYSCALE gmin = 255, gmax = 0, res = 0;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            gmin = min(gmin, gray[i][j]);
            gmax = max(gmax, gray[i][j]);
        }
    double sigma_max = 0;
    for (int t = gmin + 1; t <= gmax; t++){
        int fnum = 0, bnum = 0;
        double fsum = 0, bsum = 0;
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                if (gray[i][j] >= t){
                    fnum++;
                    fsum += gray[i][j];
                }  
                else{
                    bnum++;
                    bsum += gray[i][j];
                }
        double omega_f = (double)fnum / (double)PixelNum;
        double omega_b = (double)bnum / (double)PixelNum;
        double mu_f = fsum / (double)fnum;
        double mu_b = bsum / (double)bnum;
        double mu = omega_f * mu_f + omega_b * mu_b;
        double sigma = omega_f * omega_b * pow(mu_f - mu_b, 2);
        if (sigma > sigma_max){
            sigma_max = sigma;
            res = t;
        }
    }
    return res;
}

GRAYSCALE **GlobalBinarization(GRAYSCALE **gray, int w, int h){
    GRAYSCALE **res = (GRAYSCALE**)malloc(sizeof(GRAYSCALE*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (GRAYSCALE*)malloc(sizeof(GRAYSCALE) * w);

    GRAYSCALE T = GrayThresh(gray, w, h);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            res[i][j] = gray[i][j] >= T ? 255 : 0;

    return res;
}

#define DIVNUM 8

GRAYSCALE **LocalAdaptiveBinarization(GRAYSCALE **gray, int w, int h){
    GRAYSCALE **res = (GRAYSCALE **)malloc(sizeof(GRAYSCALE *) * h);
    for (int i = 0; i < h; i++)
        res[i] = (GRAYSCALE *)malloc(sizeof(GRAYSCALE) * w);

    int blockw = w / DIVNUM, blockh = h / DIVNUM;
    for (int i = 0; i < DIVNUM; i++)
        for (int j = 0; j < DIVNUM; j++){
            int window_w, window_h;
            if (i == DIVNUM - 1)
                window_h = h - i * blockh;
            else
                window_h = blockh;
            if (j == DIVNUM - 1)
                window_w = w - j * blockw;
            else
                window_w = blockw;
            GRAYSCALE **window_p = (GRAYSCALE**)malloc(sizeof(GRAYSCALE*) * window_h);
            for (int p = 0; p < window_h; p++)
                window_p[p] = (GRAYSCALE*)malloc(sizeof(GRAYSCALE) * window_w);
            for (int p = 0; p < window_h; p++)
                for (int q = 0; q < window_w; q++)
                    window_p[p][q] = gray[i * blockh + p][j * blockw + q];
            GRAYSCALE T = GrayThresh(window_p, window_w, window_h);
            for (int p = 0; p < window_h; p++)
                for (int q = 0; q < window_w; q++){
                    window_p[p][q] = window_p[p][q] >= T ? 255 : 0;
                    res[i * blockh + p][j * blockw + q] = window_p[p][q];
                }
            free(window_p);
        }
        
    return res;
}

GRAYSCALE **Dilation(GRAYSCALE **gray, int w, int h){
    GRAYSCALE **dil = (GRAYSCALE**)malloc(sizeof(GRAYSCALE*) * h);
    for (int i = 0; i < h; i++)
        dil[i] = (GRAYSCALE*)malloc(sizeof(GRAYSCALE) * w);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            dil[i][j] = 255;
    GRAYSCALE mask[3][3];
    memset(mask, 0, sizeof(mask));
    mask[0][1] = mask[1][0] = mask[1][1] = mask[1][2] = mask[2][1] = 1;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            int res = 0;
            for (int p = -1;p <= 1; p++)
                for (int q = -1; q <= 1; q++)
                    if (i + p >= 0 && i + p < h && j + q >= 0 && j + q < w)
                        res |= (gray[i + p][j + q] == 0) & mask[p + 1][q + 1];
            if (res == 1)
                dil[i][j] = 0;
        }
    return dil;
}

GRAYSCALE **Erosion(GRAYSCALE **gray, int w, int h){
    GRAYSCALE **ero = (GRAYSCALE**)malloc(sizeof(GRAYSCALE*) * h);
    for (int i = 0; i < h; i++)
        ero[i] = (GRAYSCALE*)malloc(sizeof(GRAYSCALE) * w);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            ero[i][j] = 255;
    GRAYSCALE mask[3][3];
    memset(mask, 0, sizeof(mask));
    mask[0][1] = mask[1][0] = mask[1][1] = mask[1][2] = mask[2][1] = 1;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            int res = 1;
            for (int p = -1;p <= 1; p++)
                for (int q = -1; q <= 1; q++)
                    if (i + p >= 0 && i + p < h && j + q >= 0 && j + q < w)
                        if (mask[p + 1][q + 1] == 1)
                            res &= (gray[i + p][j + q] == 0) & mask[p + 1][q + 1];
            if (res == 1)
                ero[i][j] = 0;
        }
    return ero;
}

GRAYSCALE **Opening(GRAYSCALE **gray, int w, int h){
    GRAYSCALE **ero = Erosion(gray, w, h);
    GRAYSCALE **dil = Dilation(ero, w, h);
    free(ero);
    return dil;
}

GRAYSCALE **Closing(GRAYSCALE **gray, int w, int h){
    GRAYSCALE **dil = Dilation(gray, w, h);
    GRAYSCALE **ero = Erosion(dil, w, h);
    free(dil);
    return ero;
}

RGBQUAD **ToneMapping(RGBQUAD **rgb, int w, int h){
    YUV **yuv = RGB2YUV(rgb, w, h);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            yuv[i][j][YUV_Y] /= 255;

    float lmax = -1;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            lmax = max(lmax, yuv[i][j][YUV_Y]);
    if (lmax < esp && lmax > -esp){
        free(yuv);
        return NULL;
    }
    
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            yuv[i][j][YUV_Y] = log(yuv[i][j][YUV_Y] + 1) / log(lmax + 1);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            yuv[i][j][YUV_Y] *= 255;
    
    RGBQUAD **res = YUV2RGB(yuv, w, h);
    free(yuv);
    return res;
}

RGBQUAD **HistogramEqualization(RGBQUAD **rgb, int w, int h){
    YUV **yuv = RGB2YUV(rgb, w, h);

    int hist[260];
    double pr[260];
    memset(hist, 0, sizeof(hist));
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            hist[(int)(yuv[i][j][YUV_Y])]++;
    for (int i = 0; i < 256; i++)
        pr[i] = hist[i] / (double)(w * h);

    double s[260];
    s[0] = pr[0];
    for (int i = 1; i < 256; i++)
        s[i] = s[i - 1] + pr[i];

    for (int i = 0; i < 256; i++)
        s[i] *= 255;
    
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            yuv[i][j][YUV_Y] = s[(int)yuv[i][j][YUV_Y]];

    RGBQUAD **res = YUV2RGB(yuv, w, h);
    free(yuv);
    return res;
}

RGBQUAD **Translation(RGBQUAD **rgb, int w, int h, int delta_w, int delta_h){
    int new_w = w + abs(delta_w), new_h = h + abs(delta_h);
    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * new_h);
    for (int i = 0; i < new_h; i++){
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * new_w);
        memset(res[i], 255, sizeof(RGBQUAD) * new_w);
    }
    
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            int i1 = delta_h > 0 ? i + delta_h : i;
            int j1 = delta_w > 0 ? j + delta_w : j;
            res[i1][j1][RGBA_A] = rgb[i][j][RGBA_A];
            res[i1][j1][RGBA_R] = rgb[i][j][RGBA_R];
            res[i1][j1][RGBA_G] = rgb[i][j][RGBA_G];
            res[i1][j1][RGBA_B] = rgb[i][j][RGBA_B];
        }
    return res;
}

#define PI 3.1415926

RGBQUAD **Rotation(RGBQUAD **rgb, int w, int h, double theta, int *new_w, int *new_h){
    while (theta < 0)
        theta += 360;
    while (theta > 360)
        theta -= 360;
    
    int basex = (h + 1) / 2, basey = (w + 1) / 2;
    double cosx = cos(theta / 180 * PI);
    double sinx = sin(theta / 180 * PI);

    if (theta < 90 || (theta > 180 && theta < 270)){
        *new_h = abs((double)(basex) * cosx + (double)(basey) * sinx) * 2;
        *new_w = abs((double)(basey) * cosx + (double)(basex) * sinx) * 2;
    } else {
        double tmp_theta = theta - 90;
        *new_h = abs((double)(basex) * cos(tmp_theta / 180 * PI) + (double)(basey) * sin(tmp_theta / 180 * PI)) * 2;
        *new_w = abs((double)(basey) * cos(tmp_theta / 180 * PI) + (double)(basex) * sin(tmp_theta / 180 * PI)) * 2;
    }

    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * (*new_h));
    for (int i = 0; i < *new_h; i++){
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * (*new_w));
        memset(res[i], 255, sizeof(RGBQUAD) * (*new_w));
    }

    int new_base_x = ((*new_h) + 1) / 2, new_base_y = ((*new_w) + 1) / 2;
    for (int i = 0; i < *new_h; i++)
        for (int j = 0; j < *new_w; j++){
            double xx = (double)(i - new_base_x) * cosx - (double)(j - new_base_y) * sinx + basex;
            double yy = (double)(i - new_base_x) * sinx + (double)(j - new_base_y) * cosx + basey;

            if (xx >= 0 && xx < h && yy >= 0 && yy < w)
                for (int k = RGBA_B; k <= RGBA_R; k++){
                    double fxx = min(max(0, floor(xx)), h - 1), fyy = min(max(0, floor(yy)), w - 1);
                    double cxx = max(0, min(h - 1, ceil(xx))), cyy = max(0, min(w - 1, ceil(yy)));
                    double tmp;
                    if (fxx == cxx && fyy == cyy)
                        tmp = rgb[(int)fxx][(int)fyy][k];
                    else if (fxx == cxx){
                        tmp = (double)rgb[(int)fxx][(int)fyy][k] / (cyy - fyy) * (cyy - yy)
                            + (double)rgb[(int)fxx][(int)cyy][k] / (cyy - fyy) * (yy - fyy);
                    } else if (fyy == cyy){
                        tmp = (double)rgb[(int)fxx][(int)fyy][k] / (cxx - fxx) * (cxx - xx)
                            + (double)rgb[(int)cxx][(int)fyy][k] / (cxx - fxx) * (xx - fxx);
                    } else{
                        tmp = (double)rgb[(int)fxx][(int)fyy][k] / ((cxx - fxx) * (cyy - fyy)) * (cxx - xx) * (cyy - yy)
                        + (double)rgb[(int)cxx][(int)fyy][k] / ((cxx - fxx) * (cyy - fyy)) * (xx - fxx) * (cyy - yy)
                        + (double)rgb[(int)fxx][(int)cyy][k] / ((cxx - fxx) * (cyy - fyy)) * (cxx - xx) * (yy - fyy)
                        + (double)rgb[(int)cxx][(int)cyy][k] / ((cxx - fxx) * (cyy - fyy)) * (xx - fxx) * (yy - fyy);
                    }
                    res[i][j][k] = tmp;
                }
        }
    
    return res;
}

RGBQUAD **Scale(RGBQUAD **rgb, int w, int h, double coef){
    int new_w = (double)w * coef, new_h = (double)h * coef;

    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * new_h);
    for (int i = 0; i < new_h; i++)
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * new_w);
    
    for (int i = 0; i < new_h; i++)
        for (int j = 0; j < new_w; j++){
            double xx = (double)i / coef;
            double yy = (double)j / coef;
            for (int k = RGBA_B; k <= RGBA_R; k++){
                double fxx = min(max(0, floor(xx)), h - 1), fyy = min(max(0, floor(yy)), w - 1);
                double cxx = max(0, min(h - 1, ceil(xx))), cyy = max(0, min(w - 1, ceil(yy)));
                double tmp;
                    if (fxx == cxx && fyy == cyy)
                        tmp = rgb[(int)fxx][(int)fyy][k];
                    else if (fxx == cxx){
                        tmp = (double)rgb[(int)fxx][(int)fyy][k] / (cyy - fyy) * (cyy - yy)
                            + (double)rgb[(int)fxx][(int)cyy][k] / (cyy - fyy) * (yy - fyy);
                    } else if (fyy == cyy){
                        tmp = (double)rgb[(int)fxx][(int)fyy][k] / (cxx - fxx) * (cxx - xx)
                            + (double)rgb[(int)cxx][(int)fyy][k] / (cxx - fxx) * (xx - fxx);
                    } else{
                        tmp = (double)rgb[(int)fxx][(int)fyy][k] / ((cxx - fxx) * (cyy - fyy)) * (cxx - xx) * (cyy - yy)
                        + (double)rgb[(int)cxx][(int)fyy][k] / ((cxx - fxx) * (cyy - fyy)) * (xx - fxx) * (cyy - yy)
                        + (double)rgb[(int)fxx][(int)cyy][k] / ((cxx - fxx) * (cyy - fyy)) * (cxx - xx) * (yy - fyy)
                        + (double)rgb[(int)cxx][(int)cyy][k] / ((cxx - fxx) * (cyy - fyy)) * (xx - fxx) * (yy - fyy);
                    }
                    res[i][j][k] = tmp;
            }
        }
    
    return res;
}

RGBQUAD **Shear(RGBQUAD **rgb, int w, int h, DIR dir, double coef){
    int new_h = dir == DX ? (int)((double)h + abs(coef) * (double)w) : h;
    int new_w = dir == DY ? (int)((double)w + abs(coef) * (double)h) : w;
    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * new_h);
    for (int i = 0; i < new_h; i++){
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD*) * new_w);
        memset(res[i], 255, sizeof(RGBQUAD) * new_w);
    }

    for (int i = 0; i < new_h; i++)
        for (int j = 0; j < new_w; j++){
            double xx = dir == DX ? (double)i + coef * (double)j : i;
            double yy = dir == DY ? (double)j + coef * (double)i : j;

            if (xx >= 0 && xx < h && yy >= 0 && yy < w)
                for (int k = RGBA_B; k <= RGBA_R; k++){
                    double fxx = min(max(0, floor(xx)), h - 1), fyy = min(max(0, floor(yy)), w - 1);
                    double cxx = max(0, min(h - 1, ceil(xx))), cyy = max(0, min(w - 1, ceil(yy)));
                    double tmp;
                        if (fxx == cxx && fyy == cyy)
                            tmp = rgb[(int)fxx][(int)fyy][k];
                        else if (fxx == cxx){
                            tmp = (double)rgb[(int)fxx][(int)fyy][k] / (cyy - fyy) * (cyy - yy)
                                + (double)rgb[(int)fxx][(int)cyy][k] / (cyy - fyy) * (yy - fyy);
                        } else if (fyy == cyy){
                            tmp = (double)rgb[(int)fxx][(int)fyy][k] / (cxx - fxx) * (cxx - xx)
                                + (double)rgb[(int)cxx][(int)fyy][k] / (cxx - fxx) * (xx - fxx);
                        } else{
                            tmp = (double)rgb[(int)fxx][(int)fyy][k] / ((cxx - fxx) * (cyy - fyy)) * (cxx - xx) * (cyy - yy)
                            + (double)rgb[(int)cxx][(int)fyy][k] / ((cxx - fxx) * (cyy - fyy)) * (xx - fxx) * (cyy - yy)
                            + (double)rgb[(int)fxx][(int)cyy][k] / ((cxx - fxx) * (cyy - fyy)) * (cxx - xx) * (yy - fyy)
                            + (double)rgb[(int)cxx][(int)cyy][k] / ((cxx - fxx) * (cyy - fyy)) * (xx - fxx) * (yy - fyy);
                        }
                        res[i][j][k] = tmp;
                }
        }

    return res;
}

RGBQUAD **Mirror(RGBQUAD **rgb, int w, int h, DIR dir){
    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * w);
        
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            for (int k = RGBA_B; k <= RGBA_R; k++)
                res[i][j][k] = rgb[dir == DX ? h - 1 - i : i][dir == DY ? w - 1 - j : j][k];
    
    return res;
}

RGBQUAD **SaltandPepper(RGBQUAD **rgb, int w, int h, double coef){
    int thred = coef * 100;
    srand(time(NULL));

    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * h);
    for (int i = 0; i < h; i++)
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * w);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            int rndres = rand() % 100 + 1;
            if (rndres > thred){
                for (int k = RGBA_B; k <= RGBA_A; k++)
                    res[i][j][k] = rgb[i][j][k];
            } else{
                res[i][j][RGBA_A] = 0;
                rndres = rand() % 2;
                for (int k = RGBA_B; k <= RGBA_R; k++)
                    res[i][j][k] = rndres ? 0 : 255;
            }
        }

    return res;
}

static int positionValid(int x, int y, int w, int h){
    if (x < 0 || y < 0 || x >= h || y >= w)
        return 0;
    return 1;
}

RGBQUAD **MeanFiltering(RGBQUAD **rgb, int w, int h){
    RGBQUAD **res = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * h);
    for (int i = 0; i < h; i++){
        res[i] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * w);
        memcpy(res[i], rgb[i], sizeof(RGBQUAD) * w);
    }

    int g[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            for (int k = RGBA_B; k <= RGBA_R; k++){
                int sum = 0, cnt = 0;
                for (int p = -1; p <= 1; p++)
                    for (int q = -1; q <= 1; q++)
                        if (positionValid(i + p, j + q, w, h)){
                            cnt += g[p + 1][q + 1];
                            sum += rgb[i + p][j + q][k] * g[p + 1][q + 1];
                        }
                res[i][j][k] = sum / cnt;
        }

    return res;
}

RGBQUAD **LaplacianEnhancement(RGBQUAD **rgb, int w, int h){
    YUV **yuv = RGB2YUV(rgb, w, h);

    double g[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++){
            double sum = 0;
            for (int p = -1; p <= 1; p++)
                for (int q = -1; q <= 1; q++){
                    int xx = min(max(0, i + p), h - 1);
                    int yy = min(max(0, j + q), w - 1);
                    sum += g[p + 1][q + 1] * yuv[xx][yy][YUV_Y];
                }
            yuv[i][j][YUV_Y] += min(max(0, sum), 255);
        }

    RGBQUAD **res = YUV2RGB(yuv, w, h);
    free(yuv);

    return res;
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