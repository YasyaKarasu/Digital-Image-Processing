#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imglib.h"

#define abs(x) ((x) > 0 ? (x) : (-(x)))

BITMAPFILE InImg, TransOut, RotOut, ScaleOut, ShearOut, MirOut;
RGBQUAD **rgb, **res;
char FileName[200], TranslatePositiveName[200], TranslateNegativeName[200], RotationName[200];
char ScaleName[200], ShearName[200], MirrorName[200];

int main()
{
    memset(FileName, 0, sizeof(FileName));
    printf("Please input Filename: ");
    scanf("%s", FileName);
    memcpy(TranslatePositiveName, FileName, sizeof(FileName));
    memcpy(TranslateNegativeName, FileName, sizeof(FileName));
    memcpy(RotationName, FileName, sizeof(FileName));
    memcpy(ScaleName, FileName, sizeof(FileName));
    memcpy(ShearName, FileName, sizeof(FileName));
    memcpy(MirrorName, FileName, sizeof(FileName));
    *strchr(TranslatePositiveName, '.') = '\0';
    *strchr(TranslateNegativeName, '.') = '\0';
    *strchr(RotationName, '.') = '\0';
    *strchr(ScaleName, '.') = '\0';
    *strchr(ShearName, '.') = '\0';
    *strchr(MirrorName, '.') = '\0';

    InputBmpImg(FileName, &InImg);

    rgb = Raw2RGB(&InImg);
    res = Translation(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), 100, 100);

    TransOut = RGB2BMP(res, InImg.bmih.biWidth + 100, abs(InImg.bmih.biHeight) + 100);

    strcat(TranslatePositiveName, "_trans_pos.bmp");
    OutputBmpImg(TranslatePositiveName, &TransOut);
    free(res);

    res = Translation(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), -100, -100);

    TransOut = RGB2BMP(res, InImg.bmih.biWidth + 100, abs(InImg.bmih.biHeight) + 100);

    strcat(TranslateNegativeName, "_trans_neg.bmp");
    OutputBmpImg(TranslateNegativeName, &TransOut);
    free(res);

    int new_w, new_h;
    res = Rotation(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), 40, &new_w, &new_h);

    RotOut = RGB2BMP(res, new_w, new_h);

    strcat(RotationName, "_rot.bmp");
    OutputBmpImg(RotationName, &RotOut);
    free(res);

    const double ScaleCoef = 0.5;

    res = Scale(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), ScaleCoef);

    ScaleOut = RGB2BMP(res, (double)InImg.bmih.biWidth * ScaleCoef, (double)abs(InImg.bmih.biHeight) * ScaleCoef);

    strcat(ScaleName, "_scale.bmp");
    OutputBmpImg(ScaleName, &ScaleOut);
    free(res);

    const double ShearCoef = -0.5;
    res = Shear(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), DX, ShearCoef);

    ShearOut = RGB2BMP(
        res,
        InImg.bmih.biWidth,
        (double)abs(InImg.bmih.biHeight) + (double)abs(ShearCoef) * (double)InImg.bmih.biWidth
    );

    strcat(ShearName, "_shear.bmp");
    OutputBmpImg(ShearName, &ShearOut);
    free(res);

    res = Mirror(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), DY);

    MirOut = RGB2BMP(res, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    strcat(MirrorName, "_mir.bmp");
    OutputBmpImg(MirrorName, &MirOut);
    free(res);

    free(rgb);
    return 0;
}