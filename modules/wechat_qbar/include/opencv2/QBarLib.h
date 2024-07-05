#ifndef QBARLIB_H
#define QBARLIB_H

#include "QBarStruct.h"
#include <QBarDecoder.h>
#include <QBarEncoder.h>
#include <unordered_set>

#ifdef _WIN32
#define __attribute__()
#define __attribute__(x)
#endif

#ifdef _HAS_STD_BYTE
#undef _HAS_STD_BYTE
#endif
#define _HAS_STD_BYTE 0

namespace cv {
namespace QBarAI {
class QBar
{
public:
    /// 初始化.
    /// @param mode：初始化QBar库的模式信息
    /// @return 0:函数正常执行,<0：函数非正常执行,-1：未设置QBAR_SEARCH_MODE
    int Init(QBAR_MODE mode);

    /// 设置识别一维码、二维码的参数.
    /// @param readers：传入需要识别的类型向量
    /// @return 0:函数正常执行,<0：函数非正常执行,-1：未执行Init函数即执行本函数, -2：传入参数错误或传入reader为空
    void SetReaders(const std::unordered_set<QBAR_READER> &readers);

    QBAR_RESULT Decode(cv::Mat &image);
    std::vector<QBAR_RESULT> ScanImage(cv::Mat &image);
    /// 获取当前库版本号.
    /// @return 当前版本号
    static std::string GetVersion();

    QBar();
    ~QBar();
    
private:
    QBarDecoder * qbarDecode_;
};

class QBarGen
{
public:
    QBarGen();
    ~QBarGen();
    
    /// 生成一、二维码.
    /// @param code：返回生成二维码/一维码的点阵图
    /// @param content：需要编码成二维码/一维码的内容
    /// @param config：生成二维码/一维码的参数
    /// @return >0:函数正常执行,<0:函数非正常执行
    static int Encode(QBAR_IMAGE& code, const std::string & content, const QBAR_ENCODE_CONFIG & config);
    
    static int EncodeQRCodeImage(const std::string &content, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    static int EncodeRoundQRCodeImage(const std::string &content, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    static int EncodePersonalQRCodeImage(const std::string& content, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    
    // ====== QRCODE VISIALIZE ===== //
    
    static void MakeQRCodeImage(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    static int MakeRoundQRCode(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    static int MakePersonQRCode(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    static int MakePersonColorQRCode(const QBAR_IMAGE& code, const QBAR_ENCODE_CONFIG& config, QBarImageInfo& buf);
    static void GetDominantColors(QBarImageInfo& image, std::vector<std::vector<int>>& domi_colors);
};
}
}
#endif
