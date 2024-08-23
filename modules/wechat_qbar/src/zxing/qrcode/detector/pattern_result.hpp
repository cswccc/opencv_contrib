#ifndef __POSSIBLE_DETECTOR_RESULT_H__
#define __POSSIBLE_DETECTOR_RESULT_H__

/*
 *  DetectorResult.hpp
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

#include <zxing/common/counted.hpp>
#include <zxing/common/array.hpp>
#include <zxing/common/bit_matrix.hpp>
#include <zxing/result_point.hpp>
#include <zxing/qrcode/detector/alignment_pattern.hpp>
#include <zxing/qrcode/detector/finder_pattern_info.hpp>
#include <zxing/qrcode/detector/finder_pattern.hpp>
#include <zxing/common/detector_result.hpp>

#include <vector>
#include "opencv2/core.hpp"

namespace zxing {
namespace qrcode {
class PatternResult : public Counted
{
public:
    Ref<FinderPatternInfo> finderPatternInfo;
    std::vector<Ref<AlignmentPattern> > possibleAlignmentPatterns;
    
    std::vector<std::vector<cv::Point2f>> moreAlignmentPoints;
    
    std::vector<cv::Point2f> topLeftPoints;
    std::vector<cv::Point2f> topRightPoints;
    std::vector<cv::Point2f> bottomLeftPoints;
    
    Ref<AlignmentPattern> confirmedAlignmentPattern;
    int possibleDimension;
    std::vector<int> possibleDimensions;
    unsigned int possibleVersion;
    
    float possibleFix;
    float possibleModuleSize;
    
    PatternResult(Ref<FinderPatternInfo> info);
    void setConfirmedAlignmentPattern(int index);
    int getPossibleAlignmentCount(){return possibleAlignmentPatterns.size();}
    
public:
    unsigned int getPossibleVersion(){return possibleVersion;}
    float getPossibleFix(){return possibleFix;}
    float getPossibleModuleSize(){return possibleModuleSize;}
    int getDimension(){return possibleDimension;}
};
}  // namespace qrcode
}  // namespace zxing

#endif  // QBAR_AI_QBAR_ZXING_QRCODE_DETECTOR_PATTERNRESULT_H_
