//
//  Image.h
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

@interface GrayscaleImage : NSObject

@property (nonatomic, readonly) uint8_t *data;
@property (nonatomic, readonly) int width;
@property (nonatomic, readonly) int height;

+ (instancetype)imageWithUIImage:(UIImage *)originalImage;
+ (instancetype)imageWithCMSampleBufferRef:(CMSampleBufferRef)sampleBuffer;

@end
