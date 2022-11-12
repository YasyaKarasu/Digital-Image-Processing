#include <stdio.h>
#include <stdlib.h>
#include "iimg.h"

void InputBmpImg(char *filename, BITMAPFILE *imgfp){
    FILE *fp = fopen(filename, "rb");
    
    // Input bitmap file header
    fread(&(imgfp->bmfh.bfType), sizeof(unsigned short), 1, fp);
    fread(&(imgfp->bmfh.bfSize), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmfh.bfReserved1), sizeof(unsigned short), 1, fp);
    fread(&(imgfp->bmfh.bfReserved2), sizeof(unsigned short), 1, fp);
    fread(&(imgfp->bmfh.bfOffbits), sizeof(unsigned int), 1, fp);

    // Input bitmap info header
    fread(&(imgfp->bmih.biSize), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biWidth), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biHeight), sizeof(int), 1, fp);
    fread(&(imgfp->bmih.biPlanes), sizeof(unsigned short), 1, fp);
    fread(&(imgfp->bmih.biBitCount), sizeof(unsigned short), 1, fp);
    fread(&(imgfp->bmih.biCompression), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biSizeImage), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biXPelsPerMeter), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biYPelsPerMeter), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biClrUsed), sizeof(unsigned int), 1, fp);
    fread(&(imgfp->bmih.biClrImportant), sizeof(unsigned int), 1, fp);

    // Jump to palette according to the biSize in bitmap info header
    fseek(fp, -40, SEEK_CUR);
    fseek(fp, imgfp->bmih.biSize, SEEK_CUR);
    fread(imgfp->aColors, sizeof(RGBQUAD), imgfp->bmih.biClrUsed, fp);
    
    // Input bitmap data
    int RowSize = ((imgfp->bmih.biBitCount * imgfp->bmih.biWidth + 31) >> 5) << 2; // Row Size in actual BMP file
    imgfp->aBitmapBits = (unsigned char*)malloc(imgfp->bmfh.bfSize);
    int skip = 4 - (((imgfp->bmih.biBitCount * imgfp->bmih.biWidth) >> 3) & 3);
    if (skip == 4)
        skip = 0;
    int BytesPerPixel = imgfp->bmih.biBitCount >> 3;
    int RowLen = BytesPerPixel * imgfp->bmih.biWidth; // Row Length that throw away 0s
    if (imgfp->bmih.biHeight < 0){
        int h = -imgfp->bmih.biHeight;
        for (int i = 0; i < h; i++){
            fread(
                imgfp->aBitmapBits + i * RowLen,
                1,
                (imgfp->bmih.biBitCount * imgfp->bmih.biWidth) >> 3,
                fp
            );
            fseek(fp, skip, SEEK_CUR);
        }
    }
    else {
        for (int i = 0; i < imgfp->bmih.biHeight; i++){
            fread(
                (imgfp->aBitmapBits) + (imgfp->bmih.biHeight - i - 1) * RowLen,
                1,
                (imgfp->bmih.biBitCount * imgfp->bmih.biWidth) >> 3,
                fp
            );
        }
        fseek(fp, skip, SEEK_CUR);
    }
    imgfp->bmih.biHeight = -abs(imgfp->bmih.biHeight);
    fclose(fp);
    return;
}