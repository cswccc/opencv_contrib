//
//  QBarSDK.h
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import <UIKit/UIKit.h>
#import "Image.h"
#import "DetectedObject.h"

@interface QBarSDK : NSObject

- (NSArray<DetectedObject *> *)scanImage:(GrayscaleImage *)image;

@end
