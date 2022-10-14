#include <stdio.h>
#include <stdlib.h>
#include "oimg.h"

void OutputBmpImg(char *filename, BITMAPFILE *imgfp){
    FILE *fp = fopen(filename, "wb");

    // Output bitmap file header
    int delta = imgfp->bmih.biSize - 40;
    imgfp->bmfh.bfSize -= delta;
    imgfp->bmfh.bfOffbits -= delta;
    fwrite(&(imgfp->bmfh.bfType), sizeof(unsigned short), 1, fp);
    fwrite(&(imgfp->bmfh.bfSize), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmfh.bfReserved1), sizeof(unsigned short), 1, fp);
    fwrite(&(imgfp->bmfh.bfReserved2), sizeof(unsigned short), 1, fp);
    fwrite(&(imgfp->bmfh.bfOffbits), sizeof(unsigned int), 1, fp);

    // Output bitmap info header
    imgfp->bmih.biSize = 40;
    fwrite(&(imgfp->bmih.biSize), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biWidth), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biHeight), sizeof(int), 1, fp);
    fwrite(&(imgfp->bmih.biPlanes), sizeof(unsigned short), 1, fp);
    fwrite(&(imgfp->bmih.biBitCount), sizeof(unsigned short), 1, fp);
    fwrite(&(imgfp->bmih.biCompression), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biSizeImage), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biXPelsPerMeter), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biYPelsPerMeter), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biClrUsed), sizeof(unsigned int), 1, fp);
    fwrite(&(imgfp->bmih.biClrImportant), sizeof(unsigned int), 1, fp);

    // Output palette 
    fwrite(imgfp->aColors, sizeof(RGBQUAD), imgfp->bmih.biClrUsed, fp);

    // Output bitmap data
    int RowSize = ((imgfp->bmih.biBitCount * imgfp->bmih.biWidth + 31) >> 5) << 2;
    int skip = 4 - (((imgfp->bmih.biBitCount * imgfp->bmih.biWidth) >> 3) & 3);
    if (skip == 4)
        skip = 0;
    int BytesPerPixel = imgfp->bmih.biBitCount >> 3;
    int RowLen = BytesPerPixel * imgfp->bmih.biWidth; // Row Length that throw away 0s
    int h = abs(imgfp->bmih.biHeight);
    unsigned char zero = 0;
    for (int i = 0; i < h; i++){
        fwrite(
            (imgfp->aBitmapBits) + i * RowLen,
            1,
            (imgfp->bmih.biBitCount * imgfp->bmih.biWidth) >> 3,
            fp
        );
        for (int j = 0; j < skip; j++)
            fwrite(&zero, 1, 1, fp);
    }
    fclose(fp);
    return;
}