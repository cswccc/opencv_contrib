//
//  DetectedObject.m
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import "DetectedObject.h"

@implementation DetectedObject

- (void)setOutlinePoints:(NSArray<NSValue *> *)outlinePoints {
    _outlinePoints = outlinePoints;
    [self resetBounds];
}

- (void)resetBounds {
    _bounds = CGRectZero;
    if (_outlinePoints.count > 0) {
        CGPoint point = [_outlinePoints[0] CGPointValue];
        CGFloat minX = point.x;
        CGFloat maxX = point.x;
        CGFloat minY = point.y;
        CGFloat maxY = point.y;
        for (NSInteger i = 1; i < _outlinePoints.count; ++i) {
            point = [_outlinePoints[i] CGPointValue];
            minX = MIN(minX, point.x);
            maxX = MAX(maxX, point.x);
            minY = MIN(minY, point.y);
            maxY = MAX(maxY, point.y);
        }
        _bounds = CGRectMake(minX, minY, maxX - minX, maxY - minY);
    }
}

@end
