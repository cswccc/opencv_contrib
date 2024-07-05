//
//  QBarSDK.mm
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import "QBarSDK.h"
#include <memory>
#include "opencv2/QBarLib.h"

@interface QBarSDK ()
@property (nonatomic, assign) QBar qbar;
@end

@implementation QBarSDK

- (instancetype)init {
    if (self = [super init]) {
        _qbar.SetReaders({ONED_BARCODE, QRCODE, PDF417, DATAMATRIX});
    }
    return self;
}

- (NSArray<DetectedObject *> *)scanImage:(GrayscaleImage *)image {
    QBAR_RESULT result = _qbar.Decode(image.data, image.width, image.height);
    return [self createDetectedObjectsWithQBarResult:result];
}

- (NSArray<DetectedObject *> *)createDetectedObjectsWithQBarResult:(const QBAR_RESULT &)result {
    NSMutableArray *outlinePoints = [NSMutableArray array];
    for (const auto &point : result.points) {
        [outlinePoints addObject:[NSValue valueWithCGPoint:CGPointMake(point.x, point.y)]];
    }
    DetectedObject *object = [[DetectedObject alloc] init];
    object.content = [NSString stringWithUTF8String:result.data.c_str()];
    object.outlinePoints = outlinePoints;
    return [NSArray arrayWithObject:object];
}

@end
