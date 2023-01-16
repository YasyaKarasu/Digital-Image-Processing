#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imglib.h"

#define abs(x) ((x) > 0 ? (x) : (-(x)))

BITMAPFILE InImg, BilatOut;
RGBQUAD **rgb, **res;
char FileName[200], BilatName[200];

int main()
{
    memset(FileName, 0, sizeof(FileName));
    printf("Please input Filename: ");
    scanf("%s", FileName);
    memcpy(BilatName, FileName, sizeof(FileName));
    *strchr(BilatName, '.') = '\0';

    InputBmpImg(FileName, &InImg);

    rgb = Raw2RGB(&InImg);
    res = BilateralFiltering(rgb, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    BilatOut = RGB2BMP(res, InImg.bmih.biWidth, abs(InImg.bmih.biHeight));

    strcat(BilatName, "_bilat.bmp");
    OutputBmpImg(BilatName, &BilatOut);
    free(res);
    free(rgb);

    return 0;
}