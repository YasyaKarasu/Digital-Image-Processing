#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "imglib.h"

BITMAPFILE img, bimg_global, bimg_local, bimg_dil, bimg_ero, bimg_open, bimg_close;
RGBQUAD **rgb;
GRAYSCALE **gray, **gray_global, **gray_local, **gray_bin, **gray_dil, **gray_ero, **gray_open, **gray_close;
char FileName[200], BinGlobalName[200], BinLocalName[200], DilName[200], EroName[200], OpenName[200], CloseName[200];

int main(){
    memset(FileName, 0, sizeof(FileName));
    printf("Please input Filename: ");
    scanf("%s", FileName);
    memcpy(BinGlobalName, FileName, sizeof(FileName));
    memcpy(BinLocalName, FileName, sizeof(FileName));
    memcpy(DilName, FileName, sizeof(FileName));
    memcpy(EroName, FileName, sizeof(FileName));
    memcpy(OpenName, FileName, sizeof(FileName));
    memcpy(CloseName, FileName, sizeof(FileName));
    *strchr(BinGlobalName, '.') = '\0';
    *strchr(BinLocalName, '.') = '\0';
    *strchr(DilName, '.') = '\0';
    *strchr(EroName, '.') = '\0';
    *strchr(OpenName, '.') = '\0';
    *strchr(CloseName, '.') = '\0';

    InputBmpImg(FileName, &img);
    rgb = Raw2RGB(&img);
    gray = RGB2Gray(rgb, img.bmih.biWidth, abs(img.bmih.biHeight));
    free(rgb);

    gray_global = GlobalBinarization(gray, img.bmih.biWidth, abs(img.bmih.biHeight));
    gray_local = LocalAdaptiveBinarization(gray, img.bmih.biWidth, abs(img.bmih.biHeight));

    bimg_global = Gray2GrayscaleImg(gray_global, img.bmih.biWidth, abs(img.bmih.biHeight));
    bimg_local = Gray2GrayscaleImg(gray_local, img.bmih.biWidth, abs(img.bmih.biHeight));

    strcat(BinGlobalName, "_bin_global.bmp");
    OutputBmpImg(BinGlobalName, &bimg_global);
    strcat(BinLocalName, "_bin_local.bmp");
    OutputBmpImg(BinLocalName, &bimg_local);

    gray_bin = gray_local;
    gray_dil = Dilation(gray_bin, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));
    gray_ero = Erosion(gray_bin, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));
    gray_open = Opening(gray_bin, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));
    gray_close = Closing(gray_bin, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));

    bimg_dil = Gray2GrayscaleImg(gray_dil, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));
    bimg_ero = Gray2GrayscaleImg(gray_ero, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));
    bimg_open = Gray2GrayscaleImg(gray_open, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));
    bimg_close = Gray2GrayscaleImg(gray_close, bimg_local.bmih.biWidth, abs(bimg_local.bmih.biHeight));

    strcat(DilName, "_dil.bmp");
    OutputBmpImg(DilName, &bimg_dil);
    strcat(EroName, "_ero.bmp");
    OutputBmpImg(EroName, &bimg_ero);
    strcat(OpenName, "_open.bmp");
    OutputBmpImg(OpenName, &bimg_open);
    strcat(CloseName, "_close.bmp");
    OutputBmpImg(CloseName, &bimg_close);

    free(gray);
    free(gray_global);
    free(gray_local);
    free(gray_dil);
    free(gray_ero);
    free(gray_open);
    free(gray_close);
    return 0;
}