//
//  DetectedObject.h
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import <UIKit/UIKit.h>

@interface DetectedObject : NSObject

@property (nonatomic, strong) NSString *type;
@property (nonatomic, strong) NSString *content;
@property (nonatomic, strong) NSArray<NSValue *> *outlinePoints;

@property (nonatomic, readonly) CGRect bounds;

@end
