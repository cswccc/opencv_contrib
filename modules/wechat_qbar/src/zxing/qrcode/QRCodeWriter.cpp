// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
/*
 *  QRCodeWriter.cpp
 *  zxing
 *
 *  Copyright 2013 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zxing/qrcode/QRCodeWriter.h>
#include <zxing/qrcode/encoder/IntMatrix.h>
#include <zxing/qrcode/encoder/ReedSolomonEncoder.h>
#include <zxing/Exception.h>
#include <zxing/qrcode/Version.h>
#include <zxing/qrcode/QRCodeWriter.h>
#include <zxing/qrcode/encoder/BitArrayWriter.h>

#include<iostream>
#include <stdio.h>

using zxing::BitArrayWriter;
using zxing::common::CharacterSetECI;
using zxing::ErrorHandler;

namespace zxing {
namespace qrcode {

QRCodeWriter::QRCodeWriter() :
ecLevel_(ErrorCorrectionLevel::L),
maskPattern_(-1),
matrix_(0, 0) {
}

Mode* QRCodeWriter::getMode() {
    return mode_;
}

ErrorCorrectionLevel QRCodeWriter::getECLevel() {
    return ecLevel_;
}

Version* QRCodeWriter::getVersion() {
    return version_;
}

int QRCodeWriter::getMaskPattern() {
    return maskPattern_;
}

IntMatrix QRCodeWriter::getMatrix() {
    return matrix_;
}

void QRCodeWriter::setMode(Mode* value) {
    mode_ = value;
}

void QRCodeWriter::setECLevel(ErrorCorrectionLevel& value) {
    ecLevel_ = value;
}

void QRCodeWriter::setVersion(Version* version) {
    version_ = version;
}

void QRCodeWriter::setMaskPattern(int value) {
    maskPattern_ = value;
}

void QRCodeWriter::setMatrix(IntMatrix& value) {
    matrix_ = value;
}

// Check if "mask_pattern" is valid.
bool QRCodeWriter::isValidMaskPattern(int maskPattern) {
    return maskPattern >= 0 && maskPattern < NUM_MASK_PATTERNS;
}

int POSITION_DETECTION_PATTERN[7][7] = {
    {1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1},
};

int POSITION_ADJUSTMENT_PATTERN[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
};

// From Appendix E. Table 1, JIS0510X:2004 (p 71). The table was double-checked by komatsu.
int POSITION_ADJUSTMENT_PATTERN_COORDINATE_TABLE[40][7] = {
    {-1, -1, -1, -1,  -1,  -1,  -1},  // Version 1
    { 6, 18, -1, -1,  -1,  -1,  -1},  // Version 2
    { 6, 22, -1, -1,  -1,  -1,  -1},  // Version 3
    { 6, 26, -1, -1,  -1,  -1,  -1},  // Version 4
    { 6, 30, -1, -1,  -1,  -1,  -1},  // Version 5
    { 6, 34, -1, -1,  -1,  -1,  -1},  // Version 6
    { 6, 22, 38, -1,  -1,  -1,  -1},  // Version 7
    { 6, 24, 42, -1,  -1,  -1,  -1},  // Version 8
    { 6, 26, 46, -1,  -1,  -1,  -1},  // Version 9
    { 6, 28, 50, -1,  -1,  -1,  -1},  // Version 10
    { 6, 30, 54, -1,  -1,  -1,  -1},  // Version 11
    { 6, 32, 58, -1,  -1,  -1,  -1},  // Version 12
    { 6, 34, 62, -1,  -1,  -1,  -1},  // Version 13
    { 6, 26, 46, 66,  -1,  -1,  -1},  // Version 14
    { 6, 26, 48, 70,  -1,  -1,  -1},  // Version 15
    { 6, 26, 50, 74,  -1,  -1,  -1},  // Version 16
    { 6, 30, 54, 78,  -1,  -1,  -1},  // Version 17
    { 6, 30, 56, 82,  -1,  -1,  -1},  // Version 18
    { 6, 30, 58, 86,  -1,  -1,  -1},  // Version 19
    { 6, 34, 62, 90,  -1,  -1,  -1},  // Version 20
    { 6, 28, 50, 72,  94,  -1,  -1},  // Version 21
    { 6, 26, 50, 74,  98,  -1,  -1},  // Version 22
    { 6, 30, 54, 78, 102,  -1,  -1},  // Version 23
    { 6, 28, 54, 80, 106,  -1,  -1},  // Version 24
    { 6, 32, 58, 84, 110,  -1,  -1},  // Version 25
    { 6, 30, 58, 86, 114,  -1,  -1},  // Version 26
    { 6, 34, 62, 90, 118,  -1,  -1},  // Version 27
    { 6, 26, 50, 74,  98, 122,  -1},  // Version 28
    { 6, 30, 54, 78, 102, 126,  -1},  // Version 29
    { 6, 26, 52, 78, 104, 130,  -1},  // Version 30
    { 6, 30, 56, 82, 108, 134,  -1},  // Version 31
    { 6, 34, 60, 86, 112, 138,  -1},  // Version 32
    { 6, 30, 58, 86, 114, 142,  -1},  // Version 33
    { 6, 34, 62, 90, 118, 146,  -1},  // Version 34
    { 6, 30, 54, 78, 102, 126, 150},  // Version 35
    { 6, 24, 50, 76, 102, 128, 154},  // Version 36
    { 6, 28, 54, 80, 106, 132, 158},  // Version 37
    { 6, 32, 58, 84, 110, 136, 162},  // Version 38
    { 6, 26, 54, 82, 110, 138, 166},  // Version 39
    { 6, 30, 58, 86, 114, 142, 170},  // Version 40
};

// Type info cells at the left top corner.
int TYPE_INFO_COORDINATES[15][2] = {
    {8, 0},
    {8, 1},
    {8, 2},
    {8, 3},
    {8, 4},
    {8, 5},
    {8, 7},
    {8, 8},
    {7, 8},
    {5, 8},
    {4, 8},
    {3, 8},
    {2, 8},
    {1, 8},
    {0, 8},
};

// From Appendix D in JISX0510:2004 (p. 67)
int VERSION_INFO_POLY = 0x1f25;  // 1 1111 0010 0101

// From Appendix C in JISX0510:2004 (p.65).
int TYPE_INFO_POLY = 0x537;
int TYPE_INFO_MASK_PATTERN = 0x5412;

// Penalty weights from section 6.8.2.1
int N1 = 3;
int N2 = 3;
int N3 = 40;
int N4 = 10;

// Check if "value" is empty.
static bool _isEmpty(int value) {
    return value == -1;
}

// Set all cells to -1.  -1 means that the cell is empty (not set yet).
static void _clearMatrix(IntMatrix& matrix) {
    matrix.clear();
    for (int y = 0; y < matrix.getHeight(); ++y) {
        for (int x = 0; x < matrix.getWidth(); ++x) {
            matrix.set(x, y, -1);
        }
    }
}

static void _embedPositionDetectionPattern(int xStart, int yStart, IntMatrix& matrix) {
    for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 7; ++x) {
            matrix.set(xStart + x, yStart + y, POSITION_DETECTION_PATTERN[y][x]);
        }
    }
}

static int _embedHorizontalSeparationPattern(int xStart, int yStart, IntMatrix& matrix) {
    for (int x = 0; x < 8; ++x) {
        if (!_isEmpty(matrix.get(xStart + x, yStart))) {
            return -1;
        }
        matrix.set(xStart + x, yStart, 0);
    }
    return 0;
}

static int _embedVerticalSeparationPattern(int xStart, int yStart, IntMatrix& matrix) {
    for (int y = 0; y < 7; ++y) {
        if (!_isEmpty(matrix.get(xStart, yStart + y))) {
            return -1;
        }
        matrix.set(xStart, yStart + y, 0);
    }
    return 0;
}

// Embed position detection patterns and surrounding vertical/horizontal
// separators.
static int _embedPositionDetectionPatternsAndSeparators(IntMatrix& matrix) {
    // Embed three big squares at corners.
    int pdpWidth = 7;
    // Left top corner.
    _embedPositionDetectionPattern(0, 0, matrix);
    // Right top corner.
    _embedPositionDetectionPattern(matrix.getWidth() - pdpWidth, 0, matrix);
    // Left bottom corner.
    _embedPositionDetectionPattern(0, matrix.getWidth() - pdpWidth, matrix);
    
    // Embed horizontal separation patterns around the squares.
    int hspWidth = 8;
    // Left top corner.
    if (_embedHorizontalSeparationPattern(0, hspWidth - 1, matrix) != 0) return -1;
    // Right top corner.
    if (_embedHorizontalSeparationPattern(matrix.getWidth() - hspWidth,
                                         hspWidth - 1, matrix) != 0) return -1;
    // Left bottom corner.
    if (_embedHorizontalSeparationPattern(0, matrix.getWidth() - hspWidth, matrix) != 0) return -1;
    
    // Embed vertical separation patterns around the squares.
    int vspSize = 7;
    // Left top corner.
    if (_embedVerticalSeparationPattern(vspSize, 0, matrix) != 0) return -1;
    // Right top corner.
    if (_embedVerticalSeparationPattern(matrix.getHeight() - vspSize - 1, 0, matrix) != 0) return -1;
    // Left bottom corner.
    if (_embedVerticalSeparationPattern(vspSize, matrix.getHeight() - vspSize, matrix) != 0) return -1;
    
    return 0;
}

// Embed the lonely dark dot at left bottom corner. JISX0510:2004 (p.46)
static int _embedDarkDotAtLeftBottomCorner(IntMatrix& matrix) {
    if (matrix.get(8, matrix.getHeight() - 8) == 0) {
        return -1;
    }
    matrix.set(8, matrix.getHeight() - 8, 1);
    
    return 0;
}

// Note that we cannot unify the function with embedPositionDetectionPattern()
// despite they are almost identical, since we cannot write a function that
// takes 2D arrays in different sizes in C/C++. We should live with the fact.
static void _embedPositionAdjustmentPattern(int xStart, int yStart, IntMatrix& matrix) {
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            matrix.set(xStart + x, yStart + y, POSITION_ADJUSTMENT_PATTERN[y][x]);
        }
    }
}

// Embed position adjustment patterns if need be.
static void _maybeEmbedPositionAdjustmentPatterns(Version* version, IntMatrix& matrix) {
    if (version->getVersionNumber() < 2) {
        // The patterns appear if version >= 2
        return;
    }
    int index = version->getVersionNumber() - 1;
    int* coordinates = POSITION_ADJUSTMENT_PATTERN_COORDINATE_TABLE[index];
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            int y = coordinates[i];
            int x = coordinates[j];
            if (x == -1 || y == -1) {
                continue;
            }
            // If the cell is unset, we embed the position adjustment pattern here.
            if (_isEmpty(matrix.get(x, y))) {
                // -2 is necessary since the x/y coordinates point to the center of
                // the pattern, not the left top corner.
                _embedPositionAdjustmentPattern(x - 2, y - 2, matrix);
            }
        }
    }
}

static void _embedTimingPatterns(IntMatrix& matrix) {
    // -8 is for skipping position detection patterns (size 7), and two
    // horizontal/vertical separation patterns (size 1). Thus, 8 = 7 + 1.
    for (int i = 8; i < matrix.getWidth() - 8; ++i) {
        int bit = (i + 1) % 2;
        // Horizontal line.
        if (_isEmpty(matrix.get(i, 6))) {
            matrix.set(i, 6, bit);
        }
        // Vertical line.
        if (_isEmpty(matrix.get(6, i))) {
            matrix.set(6, i, bit);
        }
    }
}

// Embed basic patterns.
// The basic patterns are:
// - Position detection patterns
// - Timing patterns
// - Dark dot at the left bottom corner
// - Position adjustment patterns, if need be
static int _embedBasicPatterns(Version* version, IntMatrix& matrix) {
    // Let's get started with embedding big squares at corners.
    _embedPositionDetectionPatternsAndSeparators(matrix);
    // Then, embed the dark dot at the left bottom corner.
    if (_embedDarkDotAtLeftBottomCorner(matrix) != 0) return -1;
    
    // Position adjustment patterns appear if version >= 2.
    _maybeEmbedPositionAdjustmentPatterns(version, matrix);
    // Timing patterns should be embedded after position adj. patterns.
    _embedTimingPatterns(matrix);
    
    return 0;
}

// Return the position of the most significant bit set (to one) in the "value".
// The most significant bit is position 32. If there is no bit set, return 0.
// Examples:
// - findMSBSet(0) => 0
// - findMSBSet(1) => 1
// - findMSBSet(255) => 8
static int _findMSBSet(int value) {
    int numDigits = 0;
    while (value != 0) {
        value >>= 1;
        ++numDigits;
    }
    return numDigits;
}

// Calculate BCH (Bose-Chaudhuri-Hocquenghem) code for "value" using polynomial
// "poly". The BCH code is used for encoding type information and version
// information.
// Example: Calculation of version information of 7.
// f(x) is created from 7.
//   - 7 = 000111 in 6 bits
//   - f(x) = x^2 + x^1 + x^0
// g(x) is given by the standard (p. 67)
//   - g(x) = x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
// Multiply f(x) by x^(18 - 6)
//   - f'(x) = f(x) * x^(18 - 6)
//   - f'(x) = x^14 + x^13 + x^12
// Calculate the remainder of f'(x) / g(x)
//         x^2
//         __________________________________________________
//   g(x))x^14 + x^13 + x^12
//         x^14 + x^13 + x^12 + x^11 + x^10 + x^7 + x^4 + x^2
//         --------------------------------------------------
//                              x^11 + x^10 + x^7 + x^4 + x^2
//
// The remainder is x^11 + x^10 + x^7 + x^4 + x^2
// Encode it in binary: 110010010100
// The return value is 0xc94 (1100 1001 0100)
//
// Since all coefficients in the polynomials are 1 or 0, we can do the
// calculation by bit operations. We don't care if coefficients are positive or
// negative.
static int _calculateBCHCode(int value, int poly) {
    // If poly is "1 1111 0010 0101" (version info poly), msbSetInPoly is 13.
    // We'll subtract 1 from 13 to make it 12.
    int msbSetInPoly = _findMSBSet(poly);
    value <<= msbSetInPoly - 1;
    // Do the division business using exclusive-or operations.
    while (_findMSBSet(value) >= msbSetInPoly) {
        value ^= poly << (_findMSBSet(value) - msbSetInPoly);
    }
    // Now the "value" is the remainder (i.e. the BCH code)
    return value;
}

// Make bit vector of type information. On success, store the result in "bits"
// and return true. Encode error correction level and mask pattern. See 8.9 of
// JISX0510:2004 (p.45) for details.
int _makeTypeInfoBits(ErrorCorrectionLevel& ecLevel, int maskPattern, BitArrayWriter& bits) {
    if (!QRCodeWriter::isValidMaskPattern(maskPattern)) {
        return -1;
    }
    int typeInfo = (ecLevel.bits() << 3) | maskPattern;
    bits.appendBits(typeInfo, 5);
    
    int bchCode = _calculateBCHCode(typeInfo, TYPE_INFO_POLY);
    bits.appendBits(bchCode, 10);
    
    BitArrayWriter maskBits(0);
    maskBits.appendBits(TYPE_INFO_MASK_PATTERN, 15);
    bits.bitXor(maskBits);
    
    if (bits.getSize() != 15) {
        // Just in case.
        return -1;
    }
    return 0;
}

// Embed type information.
static int _embedTypeInfo(ErrorCorrectionLevel& ecLevel, int maskPattern, IntMatrix& matrix) {
    BitArrayWriter typeInfoBits(0);
    if (_makeTypeInfoBits(ecLevel, maskPattern, typeInfoBits) != 0) return -1;
    
    for (int i = 0; i < typeInfoBits.getSize(); ++i) {
        // Place bits in LSB to MSB order.  LSB (least significant bit) is the
        // last value in "typeInfoBits".
        bool bit = typeInfoBits.get(typeInfoBits.getSize() - 1 - i);
        
        // Type info bits at the left top corner. See 8.9 of JISX0510:2004 (p.46).
        int x1 = TYPE_INFO_COORDINATES[i][0];
        int y1 = TYPE_INFO_COORDINATES[i][1];
        matrix.set(x1, y1, bit);
        
        if (i < 8)
        {
            // Right top corner.
            int x2 = matrix.getWidth() - i - 1;
            int y2 = 8;
            matrix.set(x2, y2, bit);
        }
        else
        {
            // Left bottom corner.
            int x2 = 8;
            int y2 = matrix.getHeight() - 7 + (i - 8);
            matrix.set(x2, y2, bit);
        }
    }
    return 0;
}

// Make bit vector of version information. On success, store the result in
// "bits" and return true.  See 8.10 of JISX0510:2004 (p.45) for details.
static int _makeVersionInfoBits(Version* version, BitArrayWriter& bits) {
    bits.appendBits(version->getVersionNumber(), 6);
    int bchCode = _calculateBCHCode(version->getVersionNumber(), VERSION_INFO_POLY);
    bits.appendBits(bchCode, 12);
    
    if (bits.getSize() != 18) {
        // Just in case.
        return -1;
    }
    return 0;
}

// Embed version information if need be. On success, modify the matrix and
// return true.  See 8.10 of JISX0510:2004 (p.47) for how to embed version
// information.
static int _maybeEmbedVersionInfo(Version* version, IntMatrix& matrix) {
    if (version->getVersionNumber() < 7) {
        // Version info is necessary if version >= 7.
        return 0;  // Don't need version info.
    }
    BitArrayWriter versionInfoBits(0);
    if (_makeVersionInfoBits(version, versionInfoBits) != 0) return -1;
    
    int bitIndex = 6 * 3 - 1;  // It will decrease from 17 to 0.
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 3; ++j) {
            // Place bits in LSB (least significant bit) to MSB order.
            bool bit = versionInfoBits.get(bitIndex);
            bitIndex--;
            // Left bottom corner.
            matrix.set(i, matrix.getHeight() - 11 + j, bit);
            // Right bottom corner.
            matrix.set(matrix.getHeight() - 11 + j, i, bit);
        }
    }
    return 0;
}

// Return the mask bit for "getMaskPattern" at "x" and "y". See 8.8 of
// JISX0510:2004 for mask pattern conditions.
static bool _getDataMaskBit(int maskPattern, int x, int y) {
    int intermediate = 0, temp;
    switch (maskPattern) {
        case 0:
            intermediate = (y + x) & 0x1;
            break;
        case 1:
            intermediate = y & 0x1;
            break;
        case 2:
            intermediate = x % 3;
            break;
        case 3:
            intermediate = (y + x) % 3;
            break;
        case 4:
            intermediate = ((y >> 1) + (x / 3)) & 0x1;
            break;
        case 5:
            temp = y * x;
            intermediate = (temp & 0x1) + (temp % 3);
            break;
        case 6:
            temp = y * x;
            intermediate = ((temp & 0x1) + (temp % 3)) & 0x1;
            break;
        case 7:
            temp = y * x;
            intermediate = ((temp % 3) + ((y + x) & 0x1)) & 0x1;
        default:
            break;
    }
    return intermediate == 0;
}

// Embed "dataBits" using "getMaskPattern". On success, modify the matrix and
// return true. For debugging purposes, it skips masking process if
// "getMaskPattern" is -1.  See 8.7 of JISX0510:2004 (p.38) for how to embed
// data bits.
static int _embedDataBits(BitArrayWriter& dataBits, int maskPattern, IntMatrix& matrix)
{
    int bitIndex = 0;
    int direction = -1;
    // Start from the right bottom cell.
    int x = matrix.getWidth() - 1;
    int y = matrix.getHeight() - 1;
    while (x > 0) {
        // Skip the vertical timing pattern.
        if (x == 6)
        {
            x -= 1;
        }
        while (y >= 0 && y < matrix.getHeight()) {
            for (int i = 0; i < 2; ++i) {
                int xx = x - i;
                // Skip the cell if it's not empty.
                if (!_isEmpty(matrix.get(xx, y)))
                {
                    continue;
                }
                bool bit;
                if (bitIndex < dataBits.getSize())
                {
                    bit = dataBits.get(bitIndex);
                    ++bitIndex;
                }
                else
                {
                    // Padding bit. If there is no bit left, we'll fill the left
                    // cells with 0, as described in 8.4.9 of JISX0510:2004 (p. 24).
                    bit = false;
                }
                
                // Skip masking if mask_pattern is -1.
                if (maskPattern != -1 && _getDataMaskBit(maskPattern, xx, y))
                {
                    bit = !bit;
                }
                matrix.set(xx, y, bit);
            }
            y += direction;
        }
        direction = -direction;  // Reverse the direction.
        y += direction;
        x -= 2;  // Move to the left.
    }
    // All bits should be consumed.
    if (bitIndex != dataBits.getSize()) {
        return -1;
    }
    return 0;
}

// Build 2D matrix of QR Code from "dataBits" with "ecLevel", "version" and
// "getMaskPattern".
static int _buildMatrix(BitArrayWriter& dataBits, ErrorCorrectionLevel& ecLevel, Version* version, int maskPattern, IntMatrix& matrix) {
    _clearMatrix(matrix);
    if (_embedBasicPatterns(version, matrix) != 0) return -1;
    // Type information appear with any version.
    if (_embedTypeInfo(ecLevel, maskPattern, matrix) != 0) return -1;
    // Version info appear if version >= 7.
    if (_maybeEmbedVersionInfo(version, matrix) != 0) return -1;
    // Data should be embedded at end.
    if (_embedDataBits(dataBits, maskPattern, matrix) != 0) return -1;
    
    return 0;
}

// Helper function for applyMaskPenaltyRule1. We need this for doing this
// calculation in both vertical and horizontal orders respectively.
static int _applyMaskPenaltyRule1Internal(IntMatrix& matrix, bool isHorizontal) {
    int penalty = 0;
    int iLimit = isHorizontal ? matrix.getHeight() : matrix.getWidth();
    int jLimit = isHorizontal ? matrix.getWidth() : matrix.getHeight();
    for (int i = 0; i < iLimit; ++i) {
        int numSameBitCells = 0;
        int prevBit = -1;
        for (int j = 0; j < jLimit; ++j) {
            int bit = isHorizontal ? matrix.get(i, j) : matrix.get(j, i);
            if (bit == prevBit)
            {
                numSameBitCells++;
            }
            else
            {
                if (numSameBitCells >= 5)
                {
                    penalty += N1 + (numSameBitCells - 5);
                }
                numSameBitCells = 1;  // Include the cell itself.
                prevBit = bit;
            }
        }
        if (numSameBitCells >= 5)
        {
            penalty += N1 + (numSameBitCells - 5);
        }
    }
    return penalty;
}

// Apply mask penalty rule 1 and return the penalty. Find repetitive cells with
// the same color and give penalty to them. Example: 00000 or 11111.
static int _applyMaskPenaltyRule1(IntMatrix& matrix) {
    return _applyMaskPenaltyRule1Internal(matrix, true) + _applyMaskPenaltyRule1Internal(matrix, false);
}

// Apply mask penalty rule 2 and return the penalty. Find 2x2 blocks with the
// same color and give penalty to them. This is actually equivalent to the
// spec's rule, which is to find MxN blocks and give a penalty proportional
// to (M-1)x(N-1), because this is the number of 2x2 blocks inside such a block.
static int _applyMaskPenaltyRule2(IntMatrix& matrix) {
    int penalty = 0;
    int width = matrix.getWidth();
    int height = matrix.getHeight();
    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            int value = matrix.get(x, y);
            if (matrix.get(x + 1, y) == value && matrix.get(x, y + 1) == value && matrix.get(x + 1, y + 1) == value) {
                penalty++;
            }
        }
    }
    return N2 * penalty;
}

// Apply mask penalty rule 3 and return the penalty. Find consecutive cells
// of 00001011101 or 10111010000, and give penalty to them.  If we find
// patterns like 000010111010000, we give penalties twice (i.e. 40 * 2).
static int _applyMaskPenaltyRule3(IntMatrix& matrix) {
    int penalty = 0;
    int width = matrix.getWidth();
    int height = matrix.getHeight();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Tried to simplify following conditions but failed.
            if (x + 6 < width && matrix.get(x, y) == 1 && matrix.get(x + 1, y) == 0 &&
                matrix.get(x + 2, y) == 1 && matrix.get(x + 3, y) == 1 &&
                matrix.get(x + 4, y) == 1 && matrix.get(x + 5, y) == 0 &&
                matrix.get(x + 6, y) == 1 &&
                ((x + 10 < width && matrix.get(x + 7, y) == 0 &&
                  matrix.get(x + 8, y) == 0 && matrix.get(x + 9, y) == 0 &&
                  matrix.get(x + 10, y) == 0) ||
                 (x - 4 >= 0 && matrix.get(x - 1, y) == 0 &&
                  matrix.get(x - 2, y) == 0 &&
                  matrix.get(x - 3, y) == 0 && matrix.get(x - 4, y) == 0))) {
                penalty += N3;
            }
            if (y + 6 < height && matrix.get(x, y) == 1 &&
                matrix.get(x, y + 1) == 0  &&
                matrix.get(x, y + 2) == 1  && matrix.get(x, y + 3) == 1  &&
                matrix.get(x, y + 4) == 1  && matrix.get(x, y + 5) == 0  &&
                matrix.get(x, y + 6) == 1 &&
                ((y + 10 < height && matrix.get(x, y + 7) == 0 &&
                  matrix.get(x, y + 8) == 0 && matrix.get(x, y + 9) == 0 &&
                  matrix.get(x, y + 10) == 0) ||
                 (y - 4 >= 0 && matrix.get(x, y - 1) == 0 &&
                  matrix.get(x, y - 2) == 0 &&
                  matrix.get(x, y - 3) == 0 && matrix.get(x, y - 4) == 0))) {
                penalty += N3;
            }
        }
    }
    return penalty;
}

// Apply mask penalty rule 4 and return the penalty. Calculate the ratio of dark
// cells and give penalty if the ratio is far from 50%. It gives 10 penalty for
// 5% distance.
static int _applyMaskPenaltyRule4(IntMatrix& matrix) {
    int numDarkCells = 0;
    int width = matrix.getWidth();
    int height = matrix.getHeight();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (matrix.get(x, y) == 1) {
                numDarkCells++;
            }
        }
    }
    int numTotalCells = height * width;
    double darkRatio = static_cast<double>(numDarkCells) / numTotalCells;
    int fivePercentVariances = static_cast<int>(abs(darkRatio - 0.5) * 20.0);
    return fivePercentVariances * N4;
}

// The mask penalty calculation is complicated.  See Table 21 of JISX0510:2004
// (p.45) for details. Basically it applies four rules and sums all penalties.
static int _calculateMaskPenalty(IntMatrix& matrix) {
    return _applyMaskPenaltyRule1(matrix) + _applyMaskPenaltyRule2(matrix) + _applyMaskPenaltyRule3(matrix) + _applyMaskPenaltyRule4(matrix);
}

// return the code point of the table used in alphanumeric mode or -1 if there
// is no corresponding code in the table.
static int _getAlphanumericCode(int code) {
    switch (code) {
        case ' ':
            return 36;
        case '$':
            return 37;
        case '%':
            return 38;
        case '*':
            return 39;
        case '+':
            return 40;
        case '-':
            return 41;
        case '.':
            return 42;
        case '/':
            return 43;
        case ':':
            return 44;
        default:
            if (code >= '0' && code <= '9') {
                return code - '0';
            }
            if (code >= 'A' && code <= 'Z') {
                return code - 'A' + 10;
            }
    }
    return -1;
}

static bool _isOnlyDoubleByteKanji(const std::string& content) {
    int length = content.size();
    if (length % 2 != 0) {
        return false;
    }
    for (int i = 0; i < length; i += 2) {
        int byte1 = content[i] & 0xff;
        if ((byte1 < 0x81 || byte1 > 0x9f) && (byte1 < 0xe0 || byte1 > 0xeb)) {
            return false;
        }
    }
    return true;
}

static Mode* _chooseMode(const std::string& content) {
    // choose Kanji mode if all input are double-byte characters.
    if (_isOnlyDoubleByteKanji(content)) {
        return &Mode::KANJI;
    }
    bool hasNumeric = false;
    bool hasAlphanumeric = false;
    for (int i = 0; i < static_cast<int>(content.size()); ++i) {
        char c = content[i];
        if (c >= '0' && c <= '9')
        {
            hasNumeric = true;
        }
        else if (_getAlphanumericCode(c) != -1)
        {
            hasAlphanumeric = true;
        }
        else
        {
            return &Mode::BYTE;
        }
    }
    if (hasAlphanumeric)
    {
        return &Mode::ALPHANUMERIC;
    }
    if (hasNumeric)
    {
        return &Mode::NUMERIC;
    }
    return &Mode::BYTE;
}

static int _chooseMaskPattern(BitArrayWriter& bits, ErrorCorrectionLevel& ecLevel, Version* version, IntMatrix& matrix) {
    int minPenalty = std::numeric_limits< int >::max();
    int bestMaskPattern = -1;
    // We try all mask patterns to choose the best one.
    for (int maskPattern = 0; maskPattern < QRCodeWriter::NUM_MASK_PATTERNS; maskPattern++) {
        _buildMatrix(bits, ecLevel, version, maskPattern, matrix);
        int penalty = _calculateMaskPenalty(matrix);
        if (penalty < minPenalty) {
            minPenalty = penalty;
            bestMaskPattern = maskPattern;
        }
    }
    return bestMaskPattern;
}

static int _getNumBlocks(ECBlocks& blocks) {
    int total = 0;
    std::vector<ECB*> &ecbArray = blocks.getECBlocks();
    
    // Modified to support stlport : valiantliu
    std::vector<ECB*>::iterator ecBlock;
    for (ecBlock = ecbArray.begin(); ecBlock != ecbArray.end(); ecBlock++)
    {
        total += (*ecBlock)->getCount();
    }
    return total;
}

static int _getTotalECCodewords(ECBlocks& blocks) {
    return blocks.getECCodewords() * _getNumBlocks(blocks);
}

static Version* _chooseVersion(int numInputBits, ErrorCorrectionLevel& ecLevel, ErrorHandler & err_handler) {
    // In the following comments, we use numbers of Version 7-H.
    for (int versionNum = 1; versionNum <= 40; versionNum++) {
        Version* version = Version::getVersionForNumber(versionNum, err_handler);
        if (err_handler.ErrCode()) return NULL;
        // numBytes = 196
        int numBytes = version->getTotalCodewords();
        // getNumECBytes = 130
        ECBlocks& ecBlocks = version->getECBlocksForLevel(ecLevel);
        int numEcBytes = _getTotalECCodewords(ecBlocks);
        // getNumDataBytes = 196 - 130 = 66
        int numDataBytes = numBytes - numEcBytes;
        int totalInputBytes = (numInputBits + 7) / 8;
        if (numDataBytes >= totalInputBytes) {
            return version;
        }
    }
    err_handler = ErrorHandler("Data too big");
    return NULL;
}

// Terminate bits as described in 8.4.8 and 8.4.9 of JISX0510:2004 (p.24).
static int _terminateBits(int numDataBytes, BitArrayWriter& bits) {
    int capacity = numDataBytes << 3;
    if (bits.getSize() > capacity) {
        return -1;
    }
    for (int i = 0; i < 4 && bits.getSize() < capacity; ++i) {
        bits.appendBit(false);
    }
    // Append termination bits. See 8.4.8 of JISX0510:2004 (p.24) for details.
    // If the last byte isn't 8-bit aligned, we'll add padding bits.
    int numBitsInLastByte = bits.getSize() & 0x07;
    if (numBitsInLastByte > 0) {
        for (int i = numBitsInLastByte; i < 8; i++) {
            bits.appendBit(false);
        }
    }
    // If we have more space, we'll fill the space with padding patterns
    // defined in 8.4.9 (p.24).
    int numPaddingBytes = numDataBytes - bits.getSizeInBytes();
    for (int i = 0; i < numPaddingBytes; ++i) {
        bits.appendBits((i & 0x01) == 0 ? 0xEC : 0x11, 8);
    }
    if (bits.getSize() != capacity) {
        return -1;
    }
    return 0;
}

// Get number of data bytes and number of error correction bytes for
// block id "blockID". Store the result in "numDataBytesInBlock", and
// "numECBytesInBlock". See table 12 in 8.5.1 of JISX0510:2004 (p.30)
static int _getNumDataBytesAndNumECBytesForBlockID(int numTotalBytes, int numDataBytes, int numRSBlocks, int blockID,
                                                   ArrayRef<int>& numDataBytesInBlock, ArrayRef<int>& numECBytesInBlock) {
    if (blockID >= numRSBlocks) {
        return -1;
    }
    // numRsBlocksInGroup2 = 196 % 5 = 1
    int numRsBlocksInGroup2 = numTotalBytes % numRSBlocks;
    // numRsBlocksInGroup1 = 5 - 1 = 4
    int numRsBlocksInGroup1 = numRSBlocks - numRsBlocksInGroup2;
    // numTotalBytesInGroup1 = 196 / 5 = 39
    int numTotalBytesInGroup1 = numTotalBytes / numRSBlocks;
    // numTotalBytesInGroup2 = 39 + 1 = 40
    int numTotalBytesInGroup2 = numTotalBytesInGroup1 + 1;
    // numDataBytesInGroup1 = 66 / 5 = 13
    int numDataBytesInGroup1 = numDataBytes / numRSBlocks;
    // numDataBytesInGroup2 = 13 + 1 = 14
    int numDataBytesInGroup2 = numDataBytesInGroup1 + 1;
    // numEcBytesInGroup1 = 39 - 13 = 26
    int numEcBytesInGroup1 = numTotalBytesInGroup1 - numDataBytesInGroup1;
    // numEcBytesInGroup2 = 40 - 14 = 26
    int numEcBytesInGroup2 = numTotalBytesInGroup2 - numDataBytesInGroup2;
    // Sanity checks.
    // 26 = 26
    if (numEcBytesInGroup1 != numEcBytesInGroup2) {
        return -1;
    }
    // 5 = 4 + 1.
    if (numRSBlocks != numRsBlocksInGroup1 + numRsBlocksInGroup2) {
        return -1;
    }
    // 196 = (13 + 26) * 4 + (14 + 26) * 1
    if (numTotalBytes != ((numDataBytesInGroup1 + numEcBytesInGroup1) * numRsBlocksInGroup1) + ((numDataBytesInGroup2 + numEcBytesInGroup2) * numRsBlocksInGroup2)) {
        //  throw Exception("Total bytes mismatch");
        return -1;
    }
    
    if (blockID < numRsBlocksInGroup1)
    {
        numDataBytesInBlock[0] = numDataBytesInGroup1;
        numECBytesInBlock[0] = numEcBytesInGroup1;
    }
    else
    {
        numDataBytesInBlock[0] = numDataBytesInGroup2;
        numECBytesInBlock[0] = numEcBytesInGroup2;
    }
    return 0;
}

static ArrayRef<int> _generateECBytes(ArrayRef<int>& dataBytes, int numEcBytesInBlock) {
    int numDataBytes = dataBytes->size();
    ArrayRef<int> toEncode = new Array<int>(numDataBytes + numEcBytesInBlock);
    for (int i = 0; i < numDataBytes; i++) {
        toEncode[i] = dataBytes[i];
    }
    
    ReedSolomonEncoder(Ref<GenericGF>(GF_QR_CODE_FIELD_256)).encode(toEncode, numEcBytesInBlock);
    
    ArrayRef<int> ecBytes(new Array<int>(numEcBytesInBlock));
    for (int i = 0; i < numEcBytesInBlock; i++) {
        ecBytes[i] = toEncode[numDataBytes + i];
    }
    return ecBytes;
}

// Interleave "bits" with corresponding error correction bytes. On success,
// store the result in "result". The interleave rule is complicated. See 8.6
// of JISX0510:2004 (p.37) for details.
static BitArrayWriter _interleaveWithECBytes(BitArrayWriter& bits, int numTotalBytes, int numDataBytes, int numRSBlocks) {
    BitArrayWriter result(0);
    
    // "bits" must have "getNumDataBytes" bytes of data.
    if (bits.getSizeInBytes() != numDataBytes) {
        return result;
    }
    
    // Step 1. Divide data bytes into blocks and generate error correction bytes
    // for them. We'll store the divided data bytes blocks and error correction
    // bytes blocks into "blocks".
    int dataBytesOffset = 0;
    int maxNumDataBytes = 0;
    int maxNumEcBytes = 0;
    
    // Since, we know the number of reedsolmon blocks, we can initialize the
    // vector with the number.
    // Modified to support stlport : valiantliu
    std::vector<std::pair<ArrayRef<int>, ArrayRef<int> > > blocks;
    
    ArrayRef<int> numDataBytesInBlock = new Array<int>(1);
    ArrayRef<int> numEcBytesInBlock = new Array<int>(1);
    for (int i = 0; i < numRSBlocks; ++i) {
        _getNumDataBytesAndNumECBytesForBlockID(numTotalBytes, numDataBytes, numRSBlocks, i, numDataBytesInBlock, numEcBytesInBlock);
        
        int size = numDataBytesInBlock[0];
        ArrayRef<int> dataBytes(new Array<int>(size));
        bits.toBytes(8 * dataBytesOffset, dataBytes, 0, size);
        ArrayRef<int> ecBytes = _generateECBytes(dataBytes, numEcBytesInBlock[0]);
        blocks.push_back(std::make_pair(dataBytes, ecBytes));
        
        maxNumDataBytes = (std::max<int>)(maxNumDataBytes, size);
        maxNumEcBytes = (std::max<int>)(maxNumEcBytes, ecBytes->size());
        dataBytesOffset += numDataBytesInBlock[0];
    }
    if (numDataBytes != dataBytesOffset) {
        return result;
    }
    
    // First, place data blocks.
    for (int i = 0; i < maxNumDataBytes; ++i) {
        std::vector<std::pair<ArrayRef<int>, ArrayRef<int> > >::iterator block;
        for (block = blocks.begin(); block != blocks.end(); block++){
            ArrayRef<int> dataBytes = block->first;
            if (i < dataBytes->size()) {
                result.appendBits(dataBytes[i], 8);
            }
        }
    }
    
    // Then, place error correction blocks.
    for (int i = 0; i < maxNumEcBytes; ++i) {
        std::vector<std::pair<ArrayRef<int>, ArrayRef<int> > >::iterator block;
        for (block = blocks.begin(); block != blocks.end(); block++){
            ArrayRef<int> ecBytes = block->second;
            if (i < ecBytes->size()) {
                result.appendBits(ecBytes[i], 8);
            }
        }
    }
    
    if (numTotalBytes != result.getSizeInBytes()) {
        result.clear();
        return result;
    }
    return result;
}

// Append mode info. On success, store the result in "bits".
static void _appendModeInfo(Mode* mode, BitArrayWriter& bits) {
    bits.appendBits(mode->getBits(), 4);
}

// Append length info. On success, store the result in "bits".
static int _appendLengthInfo(int numLetters, Version* version, Mode* mode, BitArrayWriter& bits) {
    int numBits = mode->getCharacterCountBits(version);
    if (numLetters >= (1 << numBits)) {
        return -1;
    }
    bits.appendBits(numLetters, numBits);
    return 0;
}

static void _appendNumericBytes(const std::string& content, BitArrayWriter& bits) {
    int length = content.size();
    int i = 0;
    while (i < length) {
        int num1 = content[i] - '0';
        if (i + 2 < length)
        {
            // Encode three numeric letters in ten bits.
            int num2 = content[i + 1] - '0';
            int num3 = content[i + 2] - '0';
            bits.appendBits(num1 * 100 + num2 * 10 + num3, 10);
            i += 3;
        }
        else if (i + 1 < length)
        {
            // Encode two numeric letters in seven bits.
            int num2 = content[i + 1] - '0';
            bits.appendBits(num1 * 10 + num2, 7);
            i += 2;
        }
        else
        {
            // Encode one numeric letter in four bits.
            bits.appendBits(num1, 4);
            i++;
        }
    }
}

static int _appendAlphanumericBytes(const std::string& content, BitArrayWriter& bits) {
    int length = content.size();
    int i = 0;
    while (i < length) {
        int code1 = _getAlphanumericCode(content[i]);
        if (code1 == -1)
        {
            //  throw Exception();
            return -1;
        }
        if (i + 1 < length)
        {
            int code2 = _getAlphanumericCode(content[i + 1]);
            if (code2 == -1)
            {
                //  throw Exception();
                return -1;
            }
            // Encode two alphanumeric letters in 11 bits.
            bits.appendBits(code1 * 45 + code2, 11);
            i += 2;
        }
        else
        {
            // Encode one alphanumeric letter in six bits.
            bits.appendBits(code1, 6);
            i++;
        }
    }
    return 0;
}

static void _append8BitBytes(const std::string& content, BitArrayWriter& bits) {
    for (size_t i = 0; i < content.size(); i++) {
        char b = content[i];
        bits.appendBits(b, 8);
    }
}

static int _appendKanjiBytes(const std::string& content, BitArrayWriter& bits) {
    int length = static_cast<int>(content.size());
    for (int i = 0; i < length; i += 2) {
        int byte1 = content[i] & 0xff;
        int byte2 = content[i + 1] & 0xff;
        int code = (byte1 << 8) | byte2;
        int subtracted = -1;
        if (code >= 0x8140 && code <= 0x9ffc)
        {
            subtracted = code - 0x8140;
        }
        else if (code >= 0xe040 && code <= 0xebbf)
        {
            subtracted = code - 0xc140;
        }
        if (subtracted == -1)
        {
            //  throw Exception("Invalid byte sequence");
            return -1;
        }
        int encoded = ((subtracted >> 8) * 0xc0) + (subtracted & 0xff);
        bits.appendBits(encoded, 13);
    }
    return 0;
}

// Append "bytes" in "mode" mode (encoding) into "bits". On success, store the
// result in "bits".
static void _appendBytes(const std::string& content, Mode* mode, BitArrayWriter& bits) {
    if (mode == &Mode::NUMERIC)
    {
        _appendNumericBytes(content, bits);
    }
    else if (mode == &Mode::ALPHANUMERIC)
    {
        _appendAlphanumericBytes(content, bits);
    }
    else if (mode == &Mode::BYTE)
    {
        _append8BitBytes(content, bits);
    }
    else
    {
        _appendKanjiBytes(content, bits);
    }
}

static void _appendECI(CharacterSetECI* eci, BitArrayWriter& bits) {
    bits.appendBits(Mode::ECI.getBits(), 4);
    // This is correct for values up to 127, which is all we need now.
    bits.appendBits(eci->getValue(), 8);
}

// Encode "bytes" with the error correction level "ecLevel". The encoding mode
// will be chosen internally by chooseMode(). On success, store the result in
// "qrCode".
// 
// We recommend you to use QRCode.EC_LEVEL_L (the lowest level) for "getECLevel"
// since our primary use is to show QR code on desktop screens. We don't need
// very strong error correction for this purpose.
QRCodeWriter QRCodeWriter::encode(const std::string& content, ErrorCorrectionLevel& ecLevel, int forceVersion, bool flag, const std::string& encoding) {
    QRCodeWriter qrCode;
    flag = true;
    
    // Pick an encoding mode appropriate for the content.
    Mode *mode =  _chooseMode(content);
    // This will store the header information, like mode and length, as well as
    // "header" segments like an ECI segment.
    BitArrayWriter headerBits(0);
    
    // Append ECI segment if applicable.
    if (mode == &Mode::BYTE && encoding != "ISO-8859-1")
    {
        CharacterSetECI* eci = CharacterSetECI::getCharacterSetECIByName(encoding);
        if (eci)
        {
            _appendECI(eci, headerBits);
        }
    }
    
    // (With ECI in place,) Write the mode marker
    _appendModeInfo(mode, headerBits);
    
    // Collect data within the main segment, separately, to count its size if
    // needed. Don't add it to main payload yet.
    BitArrayWriter dataBits(0);
    _appendBytes(content, mode, dataBits);
    
    // Hard part: need to know version to know how many bits length takes. But
    // need to know how many bits it takes to know version. First we take a
    // guess at version by assuming version will be the minimum, 1:
    ErrorHandler err_handler;
    int provisionalBitsNeeded = headerBits.getSize() + mode->getCharacterCountBits(Version::getVersionForNumber(1, err_handler)) + dataBits.getSize();
    if (err_handler.ErrCode())
    {
        flag = false;
        return qrCode;
    }
    
    Version* provisionalVersion = _chooseVersion(provisionalBitsNeeded, ecLevel, err_handler);
    if (err_handler.ErrCode())
    {
        flag = false;
        return qrCode;
    }
    
    // Use that guess to calculate the right version. I am still not sure this
    // works in 100% of cases.
    int bitsNeeded = headerBits.getSize() + mode->getCharacterCountBits(provisionalVersion) + dataBits.getSize();
    Version* version;
    
    {
        version = _chooseVersion(bitsNeeded, ecLevel, err_handler);
        if (err_handler.ErrCode())
        {
            //    throw Exception("bad encode");
            flag = false;
            return qrCode;
        }
        if (forceVersion > 0 && version->getVersionNumber() <= forceVersion)
        {
            version = Version::getVersionForNumber(forceVersion, err_handler);
            if (err_handler.ErrCode())
            {
                //    throw("encode fail");
                flag = false;
                return qrCode;
            }
        }
    }
    
    BitArrayWriter headerAndDataBits(0);
    headerAndDataBits.appendBitArray(headerBits);
    // Find "length" of main segment and write it.
    if (mode == &Mode::BYTE)
    {
        if (_appendLengthInfo(dataBits.getSizeInBytes(), version, mode, headerAndDataBits) != 0)
        {
            flag = false;
            return qrCode;
        }
    }
    else
    {
        if (_appendLengthInfo(content.size(), version, mode, headerAndDataBits) != 0)
        {
            flag = false;
            return qrCode;
        }
    }
    // Put data together into the overall payload
    headerAndDataBits.appendBitArray(dataBits);
    
    ECBlocks& ecBlocks = version->getECBlocksForLevel(ecLevel);
    int numDataBytes = version->getTotalCodewords() - _getTotalECCodewords(ecBlocks);
    
    // Terminate the bits properly.
    if (_terminateBits(numDataBytes, headerAndDataBits) != 0)
    {
        flag = false;
        return qrCode;
    }
    
    // Interleave data bits with error correction code.
    BitArrayWriter finalBits = _interleaveWithECBytes(headerAndDataBits, version->getTotalCodewords(), numDataBytes, _getNumBlocks(ecBlocks));
    
    qrCode.setECLevel(ecLevel);
    qrCode.setMode(mode);
    
    qrCode.setVersion(version);
    
    //  Choose the mask pattern and set to "qrCode".
    int dimension = version->getDimensionForVersion(err_handler);
    if (err_handler.ErrCode()) {
        flag = false;
        return qrCode;
    }
    IntMatrix matrix(dimension, dimension);
    int maskPattern = _chooseMaskPattern(finalBits, ecLevel, version, matrix);
    qrCode.setMaskPattern(maskPattern);
    
    // Build the matrix and set it to "qrCode".
    if (_buildMatrix(finalBits, ecLevel, version, maskPattern, matrix) != 0) {
        flag = false;
        return qrCode;
    }
    qrCode.setMatrix(matrix);
    
    return qrCode;
}

#ifdef CALC_CODE_AREA_SCORE
QRCodeWriter QRCodeWriter::encode(const std::string& content, ErrorCorrectionLevel& ecLevel, int forceVersion, const std::string& encoding, int maskType, ArrayRef<char> bytes) {
    Version* version;
    version = Version::getVersionForNumber(forceVersion);
    
    BitArrayWriter headerAndDataBits(0);
    
    for (int i = 0; i < bytes->size(); i++)
    {
        headerAndDataBits.appendBits(bytes[i], 8);
    }
    
    ECBlocks& ecBlocks = version->getECBlocksForLevel(ecLevel);
    int numDataBytes = version->getTotalCodewords() - _getTotalECCodewords(ecBlocks);
    
    // Terminate the bits properly.
    _terminateBits(numDataBytes, headerAndDataBits);
    
    // Interleave data bits with error correction code.
    BitArrayWriter finalBits = _interleaveWithECBytes(headerAndDataBits, version->getTotalCodewords(), numDataBytes, _getNumBlocks(ecBlocks));
    
    QRCodeWriter qrCode;
    
    qrCode.setECLevel(ecLevel);
    Mode *mode =  _chooseMode(content);
    qrCode.setMode(mode);
    
    qrCode.setVersion(version);
    
    //  Choose the mask pattern and set to "qrCode".
    int dimension = version->getDimensionForVersion();
    IntMatrix matrix(dimension, dimension);
    int maskPattern = maskType;
    if (maskPattern == -1)
        maskPattern = _chooseMaskPattern(finalBits, ecLevel, version, matrix);
    qrCode.setMaskPattern(maskPattern);
    
    // Build the matrix and set it to "qrCode".
    _buildMatrix(finalBits, ecLevel, version, maskPattern, matrix);
    qrCode.setMatrix(matrix);
    
    return qrCode;
}
#endif

QRCodeWriter QRCodeWriter::encodeMicro(const std::string& content, ErrorCorrectionLevel& ecLevel, bool flag, const std::string& encoding)
{
    QRCodeWriter qrCode;
    flag = false;
    
    Mode *mode = _chooseMode(content);
    BitArrayWriter headerBits(0);
    
    // (With ECI in place,) Write the mode marker
    _appendModeInfo(mode, headerBits);
    
    // Collect data within the main segment, separately, to count its size if
    // needed. Don't add it to main payload yet.
    BitArrayWriter dataBits(0);
    _appendBytes(content, mode, dataBits);
    
    ErrorHandler err_handler;
    Version * version = Version::getVersionForNumber(0, err_handler);
    if (err_handler.ErrCode()) {
        //    throw Exception("encode fail");
        flag = false;
        return qrCode;
    }
    
    BitArrayWriter headerAndDataBits(0);
    headerAndDataBits.appendBitArray(headerBits);
    
    //�����versionֻ�����ַ�������ǰ׺
    _appendLengthInfo(content.size(), version, mode, headerAndDataBits);
    
    // Put data together into the overall payload
    headerAndDataBits.appendBitArray(dataBits);
    
    ECBlocks& ecBlocks = version->getECBlocksForLevel(ecLevel);
    int numDataBytes = version->getTotalCodewords() - _getTotalECCodewords(ecBlocks);
    
    // Terminate the bits properly.
    if (_terminateBits(numDataBytes, headerAndDataBits) != 0) {
        flag = false;
        return qrCode;
    }
    
    // Interleave data bits with error correction code.
    BitArrayWriter finalBits = _interleaveWithECBytes(headerAndDataBits, version->getTotalCodewords(), numDataBytes, _getNumBlocks(ecBlocks));
    
    qrCode.setECLevel(ecLevel);
    qrCode.setMode(mode);
    
    qrCode.setVersion(version);
    
    int dimension = 19;
    IntMatrix matrix(dimension, dimension);
    int maskPattern = _chooseMaskPattern(finalBits, ecLevel, version, matrix);
    qrCode.setMaskPattern(maskPattern);
    
    // Build the matrix and set it to "qrCode".
    if (_buildMatrix(finalBits, ecLevel, version, maskPattern, matrix) != 0) {
        flag = false;
        return qrCode;
    }
    
    int aBlackDot[19][19] = {
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };
    
    for (int x = 0; x < matrix.getHeight(); x++)
    {
        for (int y = 0; y < matrix.getWidth(); y++)
        {
            matrix.set(y, x, matrix.get(y, x) | aBlackDot[x][y]);
        }
    }
    qrCode.setMatrix(matrix);
    return qrCode;
}


// ============= QBar Visualize ================ //

static bool is_in_eye(int i, int j, int width, int height) {
    if (i >= 0 && i < 7 && j >= 0 && j < 7) return true;
    if (i >= 0 && i < 7 && j > (width - 8) && j < width) return true;
    if (j >= 0 && j < 7 && i > (height - 8) && i < height) return true;
    return false;
}

static bool is_in_logo(int i, int j, int width, bool is_group) {
    int logo_width_ish = static_cast<int>(width / 4) + 2;
    if (is_group) {
        if (width == 49) logo_width_ish = 13;
        else if (width == 45 ||width == 41 || width == 37) logo_width_ish = 11;
    }
    int logo_offset = ceil((static_cast<int>(width / 2) - static_cast<int>(logo_width_ish / 2)));
    int logo_width = width - logo_offset * 2;
    
    return (
            (i >= logo_offset and j >= logo_offset and i < (logo_offset + logo_width) and j < (logo_offset + logo_width))
           );
}

static bool is_in_logo_border(int row, int col, int width) {
    if (width == 37)
    {
        if (row == 13 && col >= 13 && col <= 23 && col != 17 && col != 19 && col != 14 && col != 22) return true;
        if (col == 13 && row >= 13 && row <= 23 && row != 17 && row != 19 && row != 14 && row != 22) return true;
        if (row == 23 && col >= 13 && col <= 23 && col != 17 && col != 19 && col != 14 && col != 22) return true;
        if (col == 23 && row >= 13 && row <= 23 && row != 17 && row != 19 && row != 14 && row != 22) return true;
    }
    else if (width == 41)
    {
        if (row == 15 && col >= 15 && col <= 25 && col != 17 && col != 20 && col !=23) return true;
        if (col == 15 && row >= 15 && row <= 25 && row != 17 && row != 20 && row !=23) return true;
        if (row == 25 && col >= 15 && col <= 25 && col != 17 && col != 20 && col !=23) return true;
        if (col == 25 && row >= 15 && row <= 25 && row != 17 && row != 20 && row !=23) return true;
    }
    else if (width == 45)
    {
        if (row == 17 && col >= 17 && col <= 27 && col != 19 && col != 22 && col !=25) return true;
        if (col == 17 && row >= 17 && row <= 27 && row != 19 && row != 22 && row !=25) return true;
        if (row == 27 && col >= 17 && col <= 27 && col != 19 && col != 22 && col !=25) return true;
        if (col == 27 && row >= 17 && row <= 27 && row != 19 && row != 22 && row !=25) return true;
    }
    else if (width == 49)
    {
        if (row == 18 && col >= 18 && col <= 30 && col != 20 && col != 23 && col != 26 && col != 29) return true;
        if (col == 18 && row >= 18 && row <= 30 && row != 20 && row != 23 && row != 26 && row != 29) return true;
        if (row == 30 && col >= 18 && col <= 30 && col != 20 && col != 23 && col != 26 && col != 29) return true;
        if (col == 30 && row >= 18 && row <= 30 && row != 20 && row != 23 && row != 26 && row != 29) return true;
    }
    return false;
}

void QRCodeWriter::QBarQRCode(const QBAR_IMAGE& code, const QBarDrawParam& param, cv::Mat& img) {
    int width = code.width;
    int height = code.height;
    
    int box_size = param.module_size_;
    
    int image_width = width * box_size;
    int image_height = height * box_size;
    
    cv::Scalar fg_color, bg_color;
    if (param.image_mode_ == IMAGE_ALPHA)
    {
        fg_color = cv::Scalar(param.fg_color_.R, param.fg_color_.B, param.fg_color_.G, 255);
        bg_color = cv::Scalar(0, 0, 0, 0);
        
        img = cv::Mat(image_height, image_width, CV_8UC4, bg_color);
    }
    else
    {
        fg_color = cv::Scalar(param.fg_color_.R, param.fg_color_.B, param.fg_color_.G);
        bg_color = cv::Scalar(param.bg_color_.R, param.bg_color_.B, param.bg_color_.G);
        
        img = cv::Mat(image_height, image_width, CV_8UC3, bg_color);
    }
    
    // draw eyes
    if (param.eye_shape_ == EYE_CIRCLE)
    {
        cv::Point centers [3] = {
            cv::Point(3.5 * box_size, 3.5 * box_size),
            cv::Point((width - 3.5) * box_size, 3.5 * box_size),
            cv::Point(3.5 * box_size, (height - 3.5) * box_size)
        };
        
        for (int i = 0; i < 3; i++) {
            cv::circle(img, centers[i], 3.5 * box_size, fg_color, cv::FILLED);
            cv::circle(img, centers[i], 2.5 * box_size, bg_color, cv::FILLED);
            cv::circle(img, centers[i], 1.5 * box_size, fg_color, cv::FILLED);
        }
    }
    else if (param.eye_shape_ == EYE_SQUARE)
    {
        cv::Point left_top_points_1[3] = {
            cv::Point(0 * box_size, 0 * box_size),
            cv::Point((width - 7) * box_size, 0 * box_size),
            cv::Point(0 * box_size, (height - 7) * box_size)
        };
        cv::Point right_bottom_points_1[3] = {
            cv::Point(7 * box_size, 7 * box_size),
            cv::Point(width * box_size, 7 * box_size),
            cv::Point(7 * box_size, height * box_size)
        };
        
        cv::Point left_top_points_2[3] = {
            cv::Point(1 * box_size, 1 * box_size),
            cv::Point((width - 6) * box_size, 1 * box_size),
            cv::Point(1 * box_size, (height - 6) * box_size)
        };
        cv::Point right_bottom_points_2[3] = {
            cv::Point(6 * box_size, 6 * box_size),
            cv::Point((width - 1) * box_size, 6 * box_size),
            cv::Point(6 * box_size, (height-1) * box_size)
        };
        
        cv::Point left_top_points_3[3] = {
            cv::Point(2 * box_size, 2 * box_size),
            cv::Point((width - 5) * box_size, 2 * box_size),
            cv::Point(2 * box_size, (height - 5) * box_size)
        };
        cv::Point right_bottom_points_3[3] = {
            cv::Point(5 * box_size, 5 * box_size),
            cv::Point((width - 2) * box_size, 5 * box_size),
            cv::Point(5 * box_size, (height - 2) * box_size)
        };
        
        for (int i = 0; i < 3; i++) {
            cv::rectangle(img, left_top_points_1[i], right_bottom_points_1[i], fg_color, cv::FILLED);
            cv::rectangle(img, left_top_points_2[i], right_bottom_points_2[i], bg_color, cv::FILLED);
            cv::rectangle(img, left_top_points_3[i], right_bottom_points_3[i], fg_color, cv::FILLED);
        }
    }
    
    // draw modules
    if (param.module_shape_ == MODULE_CIRCLE)
    {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i < 8 && j < 8) continue;
                if (i > width - 8 && j < 8) continue;
                if (j > height - 8 && i < 8) continue;
                
                int offset = i * width + j;
                if (static_cast<int>(code.data[offset]) > 0) {
                    cv::Point center((i + 0.5) * box_size, (j + 0.5) * box_size);
                    cv::circle(img, center, 0.5 * box_size, fg_color, cv::FILLED);
                }
            }
        }
    }
    else if (param.module_shape_ == MODULE_SQUARE)
    {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i < 8 && j < 8) continue;
                if (i > width - 8 && j < 8) continue;
                if (j > height - 8 && i < 8) continue;
                
                int offset = i * width + j;
                if (static_cast<int>(code.data[offset]) > 0) {
                    cv::Rect roi(cv::Point(static_cast<int>(j * box_size), static_cast<int>(i * box_size)), cv::Size(box_size, box_size));
                    img(roi) = cv::Scalar(0, 0, 0, 255);
                }
            }
        }
    }
}

// For Personal QRCode

static void hsv2rgb(float h, float s, float v, float &r, float& g, float &b) {
    h = h / 60.0;
    int i = floor(h);
    float f = h - i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));
    
    if (i == 0)
    {
        r = v;
        g = t;
        b = p;
    }
    else if (i == 1)
    {
        r = q;
        g = v;
        b = p;
    }
    else if (i == 2)
    {
        r = p;
        g = v;
        b = t;
    }
    else if (i == 3)
    {
        r = p;
        g = q;
        b = v;
    }
    else if (i == 4)
    {
        r = t;
        g = p;
        b = v;
    }
    else
    {
        r = v;
        g = p;
        b = q;
    }
    
    r = r * 255;
    g = g * 255;
    b = b * 255;
}

static void rgb2hsv(float r, float g, float b, float& h, float& s, float& v) {
    r = r / 255.0;
    g = g / 255.0;
    b = b / 255.0;
    float mx = (std::max)((std::max)(r, g), b);
    float mn = (std::min)((std::min)(r, g), b);
    
    float df = mx - mn;
    if (mx == mn)
        h = 0;
    else if (mx == r && g >= b)
        h = 60 * ((g - b) / df) + 0;
    else if (mx == r && g < b)
        h = 60 * ((g - b) / df) + 360;
    else if (mx == g)
        h = 60 * ((b - r) / df) + 120;
    else if (mx == b)
        h = 60 * ((r - g) / df) + 240;
    
    if (mx == 0)
        s = 0;
    else
        s = df / mx;
    
    v = mx;
}

static int AdjustColor(std::vector<QBAR_COLOR>& colors, std::vector<std::vector<int>>& domi_colors) {
    float r = 100, g = 100, b = 100;
    if (colors.size() > 0) {
        r = colors[0].R;
        g = colors[0].G;
        b = colors[0].B;
    }
    float h1, s1, v1;
    rgb2hsv(r, g, b, h1, s1, v1);
    
    if ((v1 >= 0 && v1 <= 46.0 / 255) || (v1 >= 221.0 / 255 && v1 <= 1.0 && s1 >=0 && s1 <= 0.300001)) {
        if (colors.size() > 1) {
            float temp_h, temp_s, temp_v;
            rgb2hsv(colors[1].R, colors[1].G, colors[1].B, temp_h, temp_s, temp_v);
            if ((temp_v >= 0 && temp_v <= 46.0 / 255) || (temp_v >= 221.0 / 255 && temp_v <= 1.0 && temp_s >= 0 && temp_s <= 0.3)) {
            }
            else
            {
                h1 = temp_h;
                v1 = temp_v;
                s1 = temp_s;
            }
        }
    }
    float h2 = h1;
    float v2, s2;
    if (v1 >= 0.8) v1 = 0.8;
    if (s1 == 0) {
        if (v1 >= 0.3 && v1 <= 0.80001)
        {
            v2 = v1 - 0.3;
        }
        else
        {
            v2 = v1 + 0.3;
        }
        s2 = s1;
    }
    else if (s1 >= 0 && s1 <= 0.700001)
    {
        s2 = s1 + 0.3;
        v2 = v1;
    }
    else
    {
        s2 = s1 - 0.3;
        v2 = v1;
    }
    float r1, g1, b1;
    hsv2rgb(h1, s1, v1, r1, g1, b1);
    float r2, g2, b2;
    hsv2rgb(h2, s2, v2, r2, g2, b2);
    
    r1 = (r1 > 255) ? 255 : r1;
    g1 = (g1 > 255) ? 255 : g1;
    b1 = (b1 > 255) ? 255 : b1;
    r2 = (r2 > 255) ? 255 : r2;
    g2 = (g2 > 255) ? 255 : g2;
    b2 = (b2 > 255) ? 255 : b2;
    domi_colors.clear();
    domi_colors.resize(2);
    domi_colors[0] = {static_cast<int>(r1), static_cast<int>(g1), static_cast<int>(b1)};
    domi_colors[1] = {static_cast<int>(r2), static_cast<int>(g2), static_cast<int>(b2)};
    
    return 0;
}


bool Xsort(const QBAR_COLOR& a, const QBAR_COLOR& b)
{
    return a.count > b.count;
}

void QRCodeWriter::GetDominantColors(cv::Mat& image_rgb, std::vector<std::vector<int>>& domi_colors)
{
    std::vector<QBAR_COLOR> colors;
    
    if (image_rgb.data != nullptr)
    {
        cv::resize(image_rgb, image_rgb, cv::Size(80, 80), 0, 0, cv::INTER_CUBIC);
        
        int sample_count = 80 * 80;
        cv::Mat labels;
        cv::Mat centers;
        
        // convert image to kmeans data
        cv::Mat sample_data = image_rgb.reshape(3, sample_count);
        cv::Mat data;
        sample_data.convertTo(data, CV_32F);
        
        int cluster_count = 8;
        int attempt = 100;

        try {
        // k-means
            cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 5, 0.1);
            cv::kmeans(data, cluster_count, labels, criteria, attempt, cv::KMEANS_PP_CENTERS, centers);

            colors.resize(centers.rows);
            for (int i = 0; i < centers.rows; i++) {
                colors[i].R = static_cast<int>(centers.at<float>(i, 0));
                colors[i].G = static_cast<int>(centers.at<float>(i, 1));
                colors[i].B = static_cast<int>(centers.at<float>(i, 2));

                colors[i].count = 0;
            }

            // count color numbers
            for (int i = 0; i < labels.rows; i++) {
                for (int j = 0; j < labels.cols; j++){
                    size_t color_idx =  size_t(labels.at<uchar>(i, j));
                    if (color_idx < colors.size()){
                        colors[color_idx].count += 1;
                    }
                }
            }

            // sort by count
            std::sort(colors.begin(), colors.end(), Xsort);

            for (size_t i = 0; i < colors.size(); i++)
            {
                if (colors[i].count < 100)
                {
                    colors.erase(colors.begin() + i);
                    i--;
                }
            }
        }catch (std::exception &e) {
            colors.clear();
        }
    }
    /*else
    {
        QBAR_COLOR color;
        color.R = 100;
        color.G = 100;
        color.B = 100;
        color.count = 1000;
    }*/
    
    AdjustColor(colors, domi_colors);
}

void QRCodeWriter::DrawRoundQRCode(cv::Mat& eye_image, cv::Mat& module_image, 
                                   const QBAR_IMAGE& code, int box_size, cv::Mat& img_rgba) {
    int width = code.width;
    int height = code.height;
    
    // draw eyes
    int eye_size = 7 * box_size;
    
    if (eye_image.rows != eye_size && eye_image.cols != eye_size)
        cv::resize(eye_image, eye_image, cv::Size(7 * box_size, 7 * box_size), 0, 0, cv::INTER_CUBIC);
    
    cv::Point eye_positions[3] = {cv::Point(0, 0),
        cv::Point((width - 7) * box_size, 0),
        cv::Point(0, (height - 7) * box_size)
    };
    for (int i = 0; i < 3; i++) {
        cv::Rect roi(eye_positions[i], cv::Size(eye_size, eye_size));
        eye_image.copyTo(img_rgba(roi));
    }
    
    // draw modules
    float size_ratio = 0.8;
    int module_size = static_cast<int>(box_size * size_ratio);
    float delta = 0.2 * box_size;
    
    if (module_image.rows != module_size || module_image.cols != module_size)
        cv::resize(module_image, module_image, cv::Size(module_size, module_size), 0, 0, cv::INTER_CUBIC);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (is_in_eye(i, j, width, height)) continue;
            int offset = i * width + j;
            if (static_cast<int>(code.data[offset]) > 0) {
                cv::Rect roi(cv::Point(static_cast<int>(j * box_size + delta), static_cast<int>(i * box_size + delta)), cv::Size(module_size, module_size));
                module_image.copyTo(img_rgba(roi));
            }
        }
    }
}

void QRCodeWriter::DrawPersonalQRCode(cv::Mat& eye_image, cv::Mat& module_image, cv::Mat& logo_image,
                                      const QBAR_IMAGE& code, int box_size, cv::Mat& img_rgba,
                                      bool is_white, bool is_group) {
    int width = code.width;
    int height = code.height;
    
    // draw eyes
    int eye_size = 7 * box_size;
    
    if (eye_image.rows != eye_size && eye_image.cols != eye_size)
        cv::resize(eye_image, eye_image, cv::Size(7 * box_size, 7 * box_size), 0, 0, cv::INTER_CUBIC);
    
    cv::Point eye_positions[3] = {cv::Point(0, 0),
        cv::Point((width - 7) * box_size, 0),
        cv::Point(0, (height - 7) * box_size)
    };
    for (int i = 0; i < 3; i++) {
        cv::Rect roi(eye_positions[i], cv::Size(eye_size, eye_size));
        eye_image.copyTo(img_rgba(roi));
    }
    
    // draw modules
    float size_ratio = 0.8;
    int module_size = static_cast<int>(box_size * size_ratio);
    float delta = 0.2 * box_size;
    
    if (module_image.rows != module_size || module_image.cols != module_size)
        cv::resize(module_image, module_image, cv::Size(module_size, module_size), 0, 0, cv::INTER_CUBIC);
    
    if (is_white) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (is_in_eye(i, j, width, height)) continue;
                if (is_in_logo_border(i, j, width)) {
                    cv::Rect roi(cv::Point(static_cast<int>(j * box_size + delta), static_cast<int>(i * box_size + delta)), cv::Size(module_size, module_size));
                    module_image.copyTo(img_rgba(roi));
                }
                else
                {
                    if (is_in_logo(i, j, width, is_group)) continue;
                    int offset = i * width + j;
                    if (static_cast<int>(code.data[offset]) > 0) {
                        cv::Rect roi(cv::Point(static_cast<int>(j * box_size + delta), static_cast<int>(i * box_size + delta)), cv::Size(module_size, module_size));
                        module_image.copyTo(img_rgba(roi));
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (is_in_eye(i, j, width, height)) continue;
                if (is_in_logo(i, j, width, is_group)) continue;
                int offset = i * width + j;
                if (static_cast<int>(code.data[offset]) > 0) {
                    cv::Rect roi(cv::Point(static_cast<int>(j * box_size + delta), static_cast<int>(i * box_size + delta)), cv::Size(module_size, module_size));
                    module_image.copyTo(img_rgba(roi));
                }
            }
        }
    }
    
    // draw logo
    int logo_width_ish = static_cast<int>(width / 4) + 2;
    if (is_group) {
        if (width == 49) logo_width_ish = 13;
        else if (width == 45 || width == 41 || width == 37) logo_width_ish = 11;
    }
    int logo_offset = ceil((static_cast<int>(width / 2) - static_cast<int>(logo_width_ish / 2))) + 1;
    int logo_width = (width - logo_offset * 2) * box_size;
    
    cv::Mat logo_image_copy = logo_image.clone();
    if (logo_image.rows != logo_width || logo_image.cols != logo_width)
        cv::resize(logo_image_copy, logo_image_copy, cv::Size(logo_width, logo_width), 0, 0, cv::INTER_CUBIC);
    
    cv::Rect roi(cv::Point(logo_offset * box_size, logo_offset * box_size), cv::Size(logo_width, logo_width));
    logo_image_copy.copyTo(img_rgba(roi));
}

void QRCodeWriter::ApplyColorMask(cv::Mat& img_rgba, std::vector<std::vector<int>>& colors) {
    cv::Scalar color1(colors[0][0], colors[0][1], colors[0][2]);
    cv::Scalar color2(colors[1][0], colors[1][1], colors[1][2]);
    
    int width = img_rgba.cols;
    int height = img_rgba.rows;
    float width_plus_height = width + height;
    for (int i = 0; i < img_rgba.cols; i++) {
        cv::Vec4b *p = img_rgba.ptr<cv::Vec4b>(i);
        for (int j = 0; j < img_rgba.cols; j++) {
            cv::Vec4b &pix = *p ++;
            if (pix[3] > 150) {
                float norm1 = (j + width - i) / width_plus_height;
                float norm2 = 1 - norm1;
                auto new_color = color1 * norm2 + color2 * norm1;
                
                pix[0] = new_color[0];
                pix[1] = new_color[1];
                pix[2] = new_color[2];
            }
        }
    }
}

}  // namespace qrcode
}  // namespace zxing
