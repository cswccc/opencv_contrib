//
//  PreviewViewController.h
//  demo
//
//  Created by zhaozhenyang on 2024/4/24.
//

#import <UIKit/UIKit.h>
#import "DetectedObject.h"

@interface PreviewViewController : UIViewController

@property (nonatomic, strong, readonly) UIImage *image;

@property (nonatomic, strong, readonly) NSArray<DetectedObject *> *detectedObjects;

- (instancetype)initWithImage:(UIImage *)image detectedObjects:(NSArray<DetectedObject *> *)detectedObjects;

@end
