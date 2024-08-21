
#include <zxing/common/ByteMatrix.hpp>
#include <zxing/common/IllegalArgumentException.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

using std::ostream;
using std::ostringstream;

using zxing::ByteMatrix;
using zxing::ArrayRef;
using zxing::Ref;
using zxing::ErrorHandler;

void ByteMatrix::init(int width, int height) {
    if (width < 1 || height < 1)
    {
        printf("%s %d %s : Both dimensions must be greater than 0" , __FILE__, __func__, __LINE__);
        return;
    }
    this->width = width;
    this->height = height;
    
    bytes = new unsigned char[width * height];
    row_offsets = new int[height];
    row_offsets[0] = 0;
    for (int i = 1; i < height; i++) {
        row_offsets[i] = row_offsets[i-1] + width;
    }
}

ByteMatrix::ByteMatrix(int dimension) {
    init(dimension, dimension);
}

ByteMatrix::ByteMatrix(int width, int height) {
    init(width, height);
}

ByteMatrix::ByteMatrix(int width, int height, ArrayRef<char> source) {
    init(width, height);
    int size=width*height;
    memcpy(&bytes[0], &source[0], size);
}

ByteMatrix::~ByteMatrix() {
    if (bytes)
        delete [] bytes;
    if (row_offsets)
        delete [] row_offsets;
}

unsigned char* ByteMatrix::getByteRow(int y, ErrorHandler & err_handler) {
    if (y < 0 || y >= getHeight())
    {
        err_handler = IllegalArgumentErrorHandler("Requested row is outside the image.");
        return NULL;
    }
    return &bytes[row_offsets[y]];
}
