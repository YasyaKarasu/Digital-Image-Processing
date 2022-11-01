#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "imglib.h"
BITMAPFILE tonein, histin, toneout, histout;
RGBQUAD **rgb, **res;
char FileName[200], ToneName[200], HistName[200];

int main()
{
    memset(FileName, 0, sizeof(FileName));
    printf("Please input Filename: ");
    scanf("%s", FileName);
    memcpy(ToneName, FileName, sizeof(FileName));
    memcpy(HistName, FileName, sizeof(FileName));
    *strchr(ToneName, '.') = '\0';
    *strchr(HistName, '.') = '\0';

    InputBmpImg(FileName, &tonein);
    rgb = Raw2RGB(&tonein);
    res = ToneMapping(rgb, tonein.bmih.biWidth, abs(tonein.bmih.biHeight));

    toneout = RGB2BMP(res, tonein.bmih.biWidth, abs(tonein.bmih.biHeight));

    strcat(ToneName, "_tone.bmp");
    OutputBmpImg(ToneName, &toneout);

    free(rgb);
    free(res);

    InputBmpImg(FileName, &histin);
    rgb = Raw2RGB(&histin);
    res = HistogramEqualization(rgb, histin.bmih.biWidth, abs(histin.bmih.biHeight));

    histout = RGB2BMP(res, histin.bmih.biWidth, abs(histin.bmih.biHeight));

    strcat(HistName, "_hist.bmp");
    OutputBmpImg(HistName, &histout);

    free(rgb);
    free(res);
    return 0;
}