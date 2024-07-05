// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __HYBRIDBINARIZER_H__
#define __HYBRIDBINARIZER_H__
/*
 *  HybridBinarizer.h
 *  zxing
 *
 *  Copyright 2010 ZXing authors All rights reserved.
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

#include <vector>
#include <zxing/Binarizer.h>
#include <zxing/common/ByteMatrix.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/BitArray.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/ErrorHandler.h>

// Macro to use max-min in function calculateBlackPoints
#ifndef USE_MAX_MIN
#define USE_MAX_MIN 0
#endif

#ifndef USE_GOOGLE_CODE
#define USE_GOOGLE_CODE 0
#endif

#define USE_LEVEL_BINARIZER 1

namespace zxing {

class HybridBinarizer : public GlobalHistogramBinarizer {
private:
    
    Ref<ByteMatrix> grayByte_;
    ArrayRef<int> blockIntegral_;
    ArrayRef<BINARIZER_BLOCK> blocks_;
    
    ArrayRef<int> blackPoints_;
    int level_;
    
    int width_;
    int height_;
    int subWidth_;
    int subHeight_;
    
public:
    HybridBinarizer(Ref<LuminanceSource> source);
    
    virtual ~HybridBinarizer();
    
    virtual Ref<BitMatrix> getBlackMatrix(ErrorHandler &err_handler);
    virtual Ref<BitArray> getBlackRow(int y, Ref<BitArray> row, ErrorHandler &err_handler);
    
    Ref<Binarizer> createBinarizer(Ref<LuminanceSource> source);
    
private:
    
#ifdef USE_LEVEL_BINARIZER
    int initIntegral();
    int initBlockIntegral();
    int initBlocks();
    
    ArrayRef<int> getBlackPoints(int level);
    int getBlockThreshold(int x, int y, int subWidth, int sum, int min, int max, int minDynamicRange, int SIZE_POWER);
#else
    ArrayRef<int> calculateBlackPoints(Ref<ByteMatrix>& luminances,
                                       int subWidth,
                                       int subHeight,
                                       int width,
                                       int height);
#endif
    
#ifdef USE_LEVEL_BINARIZER
    void calculateThresholdForBlock(Ref<ByteMatrix>& luminances,
                                    int subWidth,
                                    int subHeight,
                                    int width,
                                    int height,
                                    int SIZE_POWER,
                                    // arrayRef<int> &blackPoints,
                                    Ref<BitMatrix> const& matrix,
                                    ErrorHandler & err_handler);
#else
    void calculateThresholdForBlock(Ref<ByteMatrix>& luminances,
                                    int subWidth,
                                    int subHeight,
                                    int width,
                                    int height,
                                    ArrayRef<int> &blackPoints,
                                    Ref<BitMatrix> const& matrix,
                                    ErrorHandler & err_handler);
#endif
    
    void thresholdBlock(Ref<ByteMatrix>& luminances,
                        int xoffset,
                        int yoffset,
                        int threshold,
                        int stride,
                        Ref<BitMatrix> const& matrix,
                        ErrorHandler & err_handler);
    
    void thresholdIrregularBlock(Ref<ByteMatrix>& luminances,
                                 int xoffset,
                                 int yoffset,
                                 int blockWidth,
                                 int blockHeight,
                                 int threshold,
                                 int stride,
                                 Ref<BitMatrix> const& matrix,
                                 ErrorHandler & err_handler);
    
#ifdef USE_SET_INT
    void thresholdFourBlocks(Ref<ByteMatrix>& luminances,
                             int xoffset,
                             int yoffset,
                             int* thresholds,
                             int stride,
                             Ref<BitMatrix> const& matrix);
#endif
    
    // Add for binarize image when call getBlackMatrix
    // By Skylook
    int binarizeByBlock(int blockLevel, ErrorHandler & err_handler);
    
};

}  // namespace zxing

#endif
