//
//  PreviewViewController.m
//  demo
//
//  Created by zhaozhenyang on 2024/4/24.
//

#import "PreviewViewController.h"

#define DETAIL_PANEL_HEIGHT 160

@interface DetectedObjectView : UIView
@property (nonatomic, strong) DetectedObject *detectedObject;
@property (nonatomic, copy) void (^didTap)(DetectedObjectView *);
@property (nonatomic, assign) BOOL selected;
@end

@implementation DetectedObjectView

- (instancetype)initWithDetectedObject:(DetectedObject *)detectedObject {
    if (self = [super init]) {
        self.detectedObject = detectedObject;
        self.backgroundColor = [UIColor clearColor];
        [self addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap)]];
    }
    return self;
}

- (void)setSelected:(BOOL)selected {
    _selected = selected;
    [self setNeedsDisplay];
}

- (void)handleTap {
    if (_didTap) {
        _didTap(self);
    }
}

- (CGPoint)coordinateForDrawingOutlinePoint:(CGPoint)point {
    CGRect bounds = _detectedObject.bounds;
    return CGPointMake((point.x - bounds.origin.x) / bounds.size.width * self.frame.size.width,
                       (point.y - bounds.origin.y) / bounds.size.height * self.frame.size.height);
}

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    CGContextRef context =UIGraphicsGetCurrentContext();
    if (_selected) {
        CGContextSetRGBStrokeColor(context, 0.0, 0.0, 1.0, 1.0);
    } else {
        CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 1.0);
    }
    CGContextSetLineWidth(context, 1.0);
    CGPoint point = [self coordinateForDrawingOutlinePoint:[_detectedObject.outlinePoints[0] CGPointValue]];
    CGContextMoveToPoint(context, point.x, point.y);
    for (NSInteger i = 1; i < _detectedObject.outlinePoints.count; ++i) {
        point = [self coordinateForDrawingOutlinePoint:[_detectedObject.outlinePoints[i] CGPointValue]];
        CGContextAddLineToPoint(context, point.x, point.y);
    }
    CGContextClosePath(context);
    CGContextStrokePath(context);
}

@end


@interface PreviewViewController ()
@property (nonatomic, strong) UIImageView *imageView;
@property (nonatomic, strong) UIView *detailPanel;
@property (nonatomic, strong) UILabel *indexLabel;
@property (nonatomic, strong) UIButton *prevButton;
@property (nonatomic, strong) UIButton *nextButton;
@property (nonatomic, strong) UITextView *contentView;
@property (nonatomic, strong) NSArray<DetectedObjectView *> *detectedObjectViews;
@property (nonatomic, assign) NSInteger selectedIndex;
@end

@implementation PreviewViewController

- (instancetype)initWithImage:(UIImage *)image detectedObjects:(NSArray<DetectedObject *> *)detectedObjects {
    if (self = [super init]) {
        _image = image;
        _detectedObjects = detectedObjects;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"返回" style:UIBarButtonItemStylePlain target:self action:@selector(popViewController)];
    [self initImageView];
    [self initDetailPanel];
    [self initDetectedObjectViews];
    [self selectDetectedObjectAtIndex:0];
}

- (void)popViewController {
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)initImageView {
    CGFloat top = CGRectGetMaxY(self.navigationController.navigationBar.frame);
    _imageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, top, self.view.frame.size.width, self.view.frame.size.height - top - DETAIL_PANEL_HEIGHT)];
    _imageView.image = _image;
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    _imageView.userInteractionEnabled = YES;
    _imageView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self.view addSubview:_imageView];
}

- (UILabel *)createLabelWithTitle:(NSString *)title alignment:(NSTextAlignment)alignment frame:(CGRect)frame {
    UILabel *label = [[UILabel alloc] initWithFrame:frame];
    label.text = title;
    label.font = [UIFont systemFontOfSize:12];
    label.textAlignment = alignment;
    label.textColor = [UIColor darkGrayColor];
    label.backgroundColor = [UIColor clearColor];
    return label;
}

- (UIButton *)createButtonWithTitle:(NSString *)title frame:(CGRect)frame {
    UIButton *button = [[UIButton alloc] initWithFrame:frame];
    [button setTitle:title forState:UIControlStateNormal];
    [button setTitleColor:[UIColor linkColor] forState:UIControlStateNormal];
    button.titleLabel.font = [UIFont systemFontOfSize:14];
    button.backgroundColor = [UIColor clearColor];
    return button;
}

- (void)initDetailPanel {
    // Detail Panel
    _detailPanel = [[UIView alloc] initWithFrame:CGRectMake(0, self.view.frame.size.height - DETAIL_PANEL_HEIGHT, self.view.frame.size.width, DETAIL_PANEL_HEIGHT)];
    _detailPanel.backgroundColor = [UIColor colorWithWhite:0.9 alpha:1.0];
    _detailPanel.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleTopMargin;
    [self.view addSubview:_detailPanel];
    // Index Label
    _indexLabel = [self createLabelWithTitle:@"" alignment:NSTextAlignmentCenter frame:CGRectMake(0, 0, self.view.frame.size.width, 30)];
    _indexLabel.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [_detailPanel addSubview:_indexLabel];
    // Prev Button
    UIButton *prevButton = [self createButtonWithTitle:@"< 上一个" frame:CGRectMake(0, 0, 72, 30)];
    prevButton.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleRightMargin;
    prevButton.hidden = _detectedObjects.count == 0;
    [prevButton addTarget:self action:@selector(selectPreviousDetectedObject) forControlEvents:UIControlEventTouchUpInside];
    [_detailPanel addSubview:prevButton];
    // Next Button
    UIButton *nextButton = [self createButtonWithTitle:@"下一个 >" frame:CGRectMake(_detailPanel.frame.size.width - 72, 0, 72, 30)];
    nextButton.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleLeftMargin;
    nextButton.hidden = _detectedObjects.count == 0;
    [nextButton addTarget:self action:@selector(selectNextDetectedObject) forControlEvents:UIControlEventTouchUpInside];
    [_detailPanel addSubview:nextButton];
    // Content View
    _contentView = [[UITextView alloc] initWithFrame:CGRectMake(0, 30, _detailPanel.frame.size.width, _detailPanel.frame.size.height - 30)];
    _contentView.editable = NO;
    _contentView.font = [UIFont systemFontOfSize:14];
    _contentView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleTopMargin;
    [_detailPanel addSubview:_contentView];
}

- (DetectedObjectView *)createViewWithDetectedObject:(DetectedObject *)object {
    DetectedObjectView *view = [[DetectedObjectView alloc] initWithDetectedObject:object];
    __weak typeof(self) weakSelf;
    view.didTap = ^(DetectedObjectView *detectedObjectView) {
        __strong typeof(self) strongSelf = weakSelf;
        NSInteger index = [strongSelf.detectedObjectViews indexOfObject:detectedObjectView];
        [self selectDetectedObjectAtIndex:index];
    };
    return view;
}

- (void)initDetectedObjectViews {
    NSMutableArray *views = [NSMutableArray array];
    for (NSInteger i = 0; i < _detectedObjects.count; ++i) {
        DetectedObjectView *view = [self createViewWithDetectedObject:_detectedObjects[i]];
        [views addObject:view];
        [self.imageView addSubview:view];
    }
    self.detectedObjectViews = views;
    [self layoutDetectedObjectViews];
}

- (void)layoutDetectedObjectViews {
    // 计算图片在ImageView中渲染的坐标与尺寸
    CGFloat wScale = _image.size.width / _imageView.frame.size.width;
    CGFloat hScale = _image.size.height / _imageView.frame.size.height;
    CGFloat scale = MAX(wScale, hScale);
    CGRect imageRect;
    imageRect.size.width = _image.size.width / scale;
    imageRect.size.height = _image.size.height / scale;
    imageRect.origin.x = (_imageView.frame.size.width - imageRect.size.width) / 2;
    imageRect.origin.y = (_imageView.frame.size.height - imageRect.size.height) / 2;
    // 计算每一个检测物的外边界的坐标与尺寸
    for (NSInteger i = 0; i < _detectedObjectViews.count; ++i) {
        DetectedObjectView *view = _detectedObjectViews[i];
        CGRect bounds = view.detectedObject.bounds;
        view.frame = CGRectMake(imageRect.origin.x + bounds.origin.x / scale,
                                imageRect.origin.y + bounds.origin.y / scale,
                                bounds.size.width / scale,
                                bounds.size.height / scale);
    }
}

- (void)selectPreviousDetectedObject {
    if (_detectedObjects.count > 0) {
        NSInteger index = (_selectedIndex > 0) ? (_selectedIndex - 1) : (_detectedObjects.count - 1);
        [self selectDetectedObjectAtIndex:index];
    }
}

- (void)selectNextDetectedObject {
    if (_detectedObjects.count > 0) {
        NSInteger index = (_selectedIndex < (_detectedObjects.count - 1)) ? (_selectedIndex + 1) : 0;
        [self selectDetectedObjectAtIndex:index];
    }
}

- (void)selectDetectedObjectAtIndex:(NSInteger)index {
    _selectedIndex = index;
    if (index < _detectedObjects.count) {
        _indexLabel.text = [NSString stringWithFormat:@"%li / %li", (index + 1), _detectedObjects.count];
        _contentView.text = _detectedObjectViews[index].detectedObject.content;
    } else {
        _indexLabel.text = @"未能识别";
        _contentView.text = @"";
    }
    for (NSInteger i = 0; i < _detectedObjectViews.count; ++i) {
        _detectedObjectViews[i].selected = (i == index);
    }
}

@end
