//
//  dmtximage.cpp
//  test_dm
//
//  Created by wechatcv on 2022/5/5.
//

#include "dmtximage.hpp"
#include "common.hpp"

namespace dmtx {

DmtxImage::~DmtxImage()
{
    
}

int DmtxImage::dmtxImageCreate(unsigned char *pxl, int width, int height)
{
    if (pxl == NULL || width < 1 || height < 1)
        return -1;
    
    this->pxl = pxl;
    this->width = width;
    this->height = height;
    
    this->bitsPerPixel = 8;  // gray scale
    this->bytesPerPixel = this->bitsPerPixel / 8;
    this->rowPadBytes = 0;
    this->rowSizeBytes = this->width * this->bytesPerPixel + this->rowPadBytes;
    this->imageFlip = DmtxFlipNone;
    
    return 0;
}

int DmtxImage::dmtxImageGetProp(int prop)
{
    switch (prop) {
        case DmtxPropWidth:
            return this->width;
        case DmtxPropHeight:
            return this->height;
        case DmtxPropBitsPerPixel:
            return this->bitsPerPixel;
        case DmtxPropBytesPerPixel:
            return this->bytesPerPixel;
        case DmtxPropRowPadBytes:
            return this->rowPadBytes;
        case DmtxPropRowSizeBytes:
            return this->rowSizeBytes;
        case DmtxPropImageFlip:
            return this->imageFlip;
        default:
            break;
    }
    
    return -1;
}

unsigned int DmtxImage::dmtxImageGetPixelValue(int x, int y, int *value)
{
    int offset = dmtxImageGetByteOffset(x, y);
    if (offset == -1)
        return DmtxFail;
    
    *value = this->pxl[offset];
    
    return DmtxPass;
}

int DmtxImage::dmtxImageGetByteOffset(int x, int y)
{
    if (this->imageFlip & DmtxFlipX) return -1;
    
    if (dmtxImageContainsInt(0, x, y) == DmtxFail)
        return -1;
    
    if (this->imageFlip & DmtxFlipY)
        return (y * this->rowSizeBytes + x * this->bytesPerPixel);
    
    return ((this->height - y - 1) * this->rowSizeBytes + x * this->bytesPerPixel);
}

unsigned int DmtxImage::dmtxImageContainsInt(int margin, int x, int y)
{
    if (x - margin >= 0 && x + margin < this->width &&
       y - margin >= 0 && y + margin < this->height)
        return DmtxPass;
    
    return DmtxFail;
}

}  // namespace dmtx
