#ifndef __LINESSAMPLER_H__
#define __LINESSAMPLER_H__

/*
 * Copyright 2010 ZXing authors All rights reserved.
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

#include <map>
#include <zxing/common/BitMatrix.hpp>
#include <zxing/ResultPoint.hpp>
#include <zxing/common/Point.hpp>
#include <zxing/ErrorHandler.hpp>

namespace zxing {
namespace pdf417 {
namespace detector {

class PossibleCodeWords
{
public:
    PossibleCodeWords(const std::vector<std::vector<std::map<int, int> > > &vcVotes, int iRowCount);
    ~PossibleCodeWords() {};
    
    int GetNextPossible(std::vector<std::vector<int> > &vcCodeWords);
    
private:
    int m_iHeight;
    int m_iWidth;
    int m_iForIndex;
    std::vector<int> m_vcBadPointList;
    std::vector<int> m_vcSecondIndex;
    std::vector<std::vector<std::map<int, int> > > m_vcVotes;
    std::vector<std::vector<int> > m_vcCodeWords;
};

class LinesSampler {
private:
    static const int MODULES_IN_SYMBOL = 17;
    static const int BARS_IN_SYMBOL = 8;
    static const int POSSIBLE_SYMBOLS = 2787;
    std::vector<float> RATIOS_TABLE;
    std::vector<float> init_ratios_table();
    static const int BARCODE_START_OFFSET = 2;
    const float MAX_EMPTY_LINE_RATIOS = 0.3;
    
    Ref<BitMatrix> linesMatrix_;
    // int symbolsPerLine_;
    int dimension_;
    PossibleCodeWords * m_ptPossibleCodeWords;
    
    void codewordsToBitMatrix(const std::vector<std::vector<int> > &codewords,
                              Ref<BitMatrix> &matrix);
    int calculateClusterNumber(int codeword);
    
    void computeSymbolWidths(std::vector<float>& symbolWidths,
                             const int symbolsPerLine, Ref<BitMatrix> linesMatrix);
   
    ErrorHandler linesMatrixToCodewords(std::vector<std::vector<int> > &clusterNumbers,
                                        const int symbolsPerLine,
                                        const std::vector<float> &symbolWidths,
                                        Ref<BitMatrix> linesMatrix,
                                        std::vector<std::vector<int> > &codewords);
    std::vector<std::vector<std::map<int, int> > >
    distributeVotes(const int symbolsPerLine,
                    const std::vector<std::vector<int> >& codewords,
                    const std::vector<std::vector<int> >& clusterNumbers);
    std::vector<int>
    findMissingLines(const int symbolsPerLine,
                     std::vector<std::vector<int> > &detectedCodeWords, int &iEmptyLine);
    int decodeRowCount(const int symbolsPerLine,
                       std::vector<std::vector<int> > &detectedCodeWords,
                       std::vector<int> &insertLinesAt,
                       int &iEmptyLine);
    
    int round(float d);
    Point intersection(Line a, Line b);
    int GetBitCountSum(const std::vector<int> &vcModuleBitCount);
    std::vector<int> SampleBitCounts(const std::vector<int> &vcModuleBitCount);
    int GetBitValue(const std::vector<int> &vcModuleBitCount);
    
public:
    LinesSampler(Ref<BitMatrix> linesMatrix, int dimension);
    ~LinesSampler()
    {
        if (m_ptPossibleCodeWords != NULL)
        {
            delete m_ptPossibleCodeWords;
            m_ptPossibleCodeWords = NULL;
        }
    }
    void SetLineMatrix(Ref<BitMatrix> linesMatrix);
    ErrorHandler sample(Ref<BitMatrix> & bit_matrix);
    Ref<BitMatrix> GetNextPossibleGrid(ErrorHandler &err_handler);
};

}  // namespace detector
}  // namespace pdf417
}  // namespace zxing

#endif  // __LINESSAMPLER_H__
