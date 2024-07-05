//
//  Image.m
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import "Image.h"
#import <Accelerate/Accelerate.h>

@interface GrayscaleImage ()
@property (nonatomic, readwrite) uint8_t *data;
@property (nonatomic, readwrite) int width;
@property (nonatomic, readwrite) int height;
@end

@implementation GrayscaleImage

- (instancetype)init {
    if (self = [super init]) {
        _data = NULL;
        _width = 0;
        _height = 0;
    }
    return self;
}

- (void)dealloc {
    if (_data) {
        free(_data);
    }
}

+ (instancetype)imageWithUIImage:(UIImage *)originalImage {
    CGDataProviderRef provider = CGImageGetDataProvider(originalImage.CGImage);
    NSData *data = (id)CFBridgingRelease(CGDataProviderCopyData(provider));
    int64_t count = originalImage.size.width * originalImage.size.height;
    uint8_t *grayscaleData = malloc(count);
    const uint8_t *bytes = (const uint8_t *)data.bytes;
    int64_t p = 0;
    uint8_t *pg = grayscaleData;
    for (int i = 0; i < count; i++, pg++, p += 4) {
        *pg = (uint8_t)(0.2989 * bytes[p] + 0.587 * bytes[p + 1] + 0.114 * bytes[p + 2]);
    }
    GrayscaleImage *image = [[GrayscaleImage alloc] init];
    image.data = grayscaleData;
    image.width = (int)originalImage.size.width;
    image.height = (int)originalImage.size.height;
    return image;
}


+ (instancetype)imageWithCMSampleBufferRef:(CMSampleBufferRef)sampleBuffer {
    CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    uint8_t *originalImageData = (uint8_t *)CVPixelBufferGetBaseAddress(pixelBuffer);
    uint8_t *grayscaleData = malloc(width * height);
    uint8_t *dp = 0;
    uint8_t *sp = 0;
    for (size_t y = 0; y < height; ++y) {
        sp = originalImageData + y * CVPixelBufferGetBytesPerRow(pixelBuffer);
        for (size_t x = 0; x < width; ++x, sp += 4) {
            dp = grayscaleData + (x * height + height - y - 1);
            *dp = (uint8_t)(0.2989 * sp[2] + 0.587 * sp[1] + 0.114 * sp[0]);
        }
    }
    CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    GrayscaleImage *image = [[GrayscaleImage alloc] init];
    image.data = grayscaleData;
    image.width = (int)height;
    image.height = (int)width;
    return image;
}

@end
