#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imglib.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

BITMAPFILE img, grayimg, limg;
RGBQUAD **rgb, **lrgb;
YUV **yuv;
float **gray;
char filename[200], grayname[200], lightname[200];

int main(){
    memset(filename, 0, sizeof(filename));
    printf("Please input filename: ");
    scanf("%s", filename);
    memcpy(grayname, filename, sizeof(filename));
    memcpy(lightname, filename, sizeof(filename));
    *strchr(grayname, '.') = '\0';
    *strchr(lightname, '.') = '\0';

    InputBmpImg(filename, &img);

    rgb = Raw2RGB(&img); // Get RGB info from raw unsigned char array
    yuv = RGB2YUV(rgb, img.bmih.biWidth, abs(img.bmih.biHeight)); // Trans RGB to YUV

    gray = YUV2Gray(yuv, img.bmih.biWidth, abs(img.bmih.biHeight)); // Trans YUV to Gray(Y)
    rescale(gray, img.bmih.biWidth, abs(img.bmih.biHeight)); // Rescale Gray intensity to lie between [0, 255]
    grayimg = Gray2GrayscaleImg(gray, img.bmih.biWidth, abs(img.bmih.biHeight)); // Generate Grayscale Image
    strcat(grayname, "_gray.bmp");
    OutputBmpImg(grayname, &grayimg);
    free(grayimg.aBitmapBits);

    for (int i = 0; i < abs(img.bmih.biHeight); i++)
        for (int j = 0; j < img.bmih.biWidth; j++){
            yuv[i][j][YUV_Y] = min(255, yuv[i][j][YUV_Y] + 35); // Change the luminance value Y
        }
    lrgb = YUV2RGB(yuv, img.bmih.biWidth, abs(img.bmih.biHeight)); // Trans YUV to RGB
    limg = RGB2BMP(lrgb, img.bmih.biWidth, abs(img.bmih.biHeight)); // Generate BMP file from RGB info
    strcat(lightname, "_light.bmp");
    OutputBmpImg(lightname, &limg);
    free(limg.aBitmapBits);

    free(img.aBitmapBits);
    return 0;
}