#pragma  once

#include <zxing/ZXing.hpp>
#include <zxing/common/Counted.hpp>
#include <zxing/Binarizer.hpp>
#include <zxing/common/GlobalHistogramBinarizer.hpp>
#include <zxing/common/HybridBinarizer.hpp>
#include <zxing/common/FastWindowBinarizer.hpp>
#include <zxing/common/SimpleAdaptiveBinarizer.hpp>

namespace cv {
namespace QBarAI {
class BinarizerMgr
{
    enum BINARIZER
    {
        Hybrid = 0,
        FastWindow = 1,
        SimpleAdaptive = 2,
        GlobalHistogram = 3,
        OTSU = 4,
        Niblack = 5,
        Adaptive = 6,
        HistogramBackground = 7
    };
    
public:
    BinarizerMgr();
    ~BinarizerMgr();
    
    zxing::Ref<zxing::Binarizer> Binarize(zxing::Ref<zxing::LuminanceSource> source);
    
    void SwitchBinarizer();
    
    int GetCurBinarizer();
    
    void SetNextOnceBinarizer(int iBinarizerIndex);
    
private:
    int m_iNowRotateIndex;
    int m_iNextOnceBinarizer;
    std::vector<BINARIZER> m_vecRotateBinarizer;
};
}  // namespace QBarAI
}  // namesapce cv