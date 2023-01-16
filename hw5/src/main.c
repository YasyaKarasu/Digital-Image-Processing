#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imglib.h"

#define abs(x) ((x) > 0 ? (x) : (-(x)))

BITMAPFILE InImg, NoiseOut, MeanOut, LaplacianOut;
RGBQUAD **rgb, **res;
char FileName[200], NoiseName[200], MeanName[200], LaplacianName[200];

int main()
{
    memset(FileName, 0, sizeof(FileName));
    printf("Please input Filename: ");
    scanf("%s", FileName);
    memcpy(MeanName, FileName, sizeof(FileName));
    memcpy(LaplacianName, FileName, sizeof(FileName));
    memcpy(NoiseName, FileName, sizeof(FileName));
    *strchr(MeanName, '.') = '\0';
    *strchr(LaplacianName, '.') = '\0';
    *strchr(NoiseName, '.') = '\0';

    InputBmpImg(FileName, &InImg);

    rgb = Raw2RGB(&InImg);
    res = SaltandPepper(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight), 0.01);

    NoiseOut = RGB2BMP(res, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    strcat(NoiseName, "_noise.bmp");
    OutputBmpImg(NoiseName, &NoiseOut);
    free(res);
    free(rgb);

    rgb = Raw2RGB(&NoiseOut);

    res = MeanFiltering(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    MeanOut = RGB2BMP(res, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    strcat(MeanName, "_mean.bmp");
    OutputBmpImg(MeanName, &MeanOut);
    free(res);
    free(rgb);

    rgb = Raw2RGB(&InImg);
    res = LaplacianEnhancement(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    LaplacianOut = RGB2BMP(res, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    strcat(LaplacianName, "_lap.bmp");
    OutputBmpImg(LaplacianName, &LaplacianOut);
    free(res);
    free(rgb);
    
    return 0;
}