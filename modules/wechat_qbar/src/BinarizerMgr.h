#pragma  once

#include <zxing/ZXing.h>
#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/common/FastWindowBinarizer.h>
#include <zxing/common/SimpleAdaptiveBinarizer.h>

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