//
//  CameraViewController.m
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import <AVFoundation/AVFoundation.h>
#import "CameraViewController.h"
#import "PreviewViewController.h"
#import "QBarSDK.h"

#pragma mark - CameraPreviewView

@interface CameraPreviewView : UIView
@end

@implementation CameraPreviewView

+ (Class)layerClass {
    return AVCaptureVideoPreviewLayer.class;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    }
    return self;
}

- (AVCaptureVideoPreviewLayer *)previewLayer {
    return (AVCaptureVideoPreviewLayer *)self.layer;
}

- (void)setSession:(AVCaptureSession *)session {
    self.previewLayer.session = session;
}

@end


#pragma mark - CameraViewController

@interface CameraViewController () <AVCaptureVideoDataOutputSampleBufferDelegate>
@property (nonatomic, strong) CameraPreviewView *cameraView;
@property (nonatomic, strong) AVCaptureDevice *videoCaptureDevice;
@property (nonatomic, strong) AVCaptureSession *captureSession;
@property (nonatomic, strong) dispatch_queue_t captureSessionQueue;
@property (nonatomic, strong) QBarSDK *qbar;
@property (nonatomic, assign) BOOL shouldScan;
@property (nonatomic, assign) NSUInteger scanCounter;
@end

@implementation CameraViewController

- (instancetype)init {
    if (self = [super init]) {
        _qbar = [[QBarSDK alloc] init];
        _shouldScan = NO;
        _scanCounter = 0;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    [self initCameraView];
    [self initBackButton];
    [self initCamera];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.navigationController.navigationBarHidden = YES;
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    [self startCapture];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    self.navigationController.navigationBarHidden = NO;
    [self stopCapture];
}

- (void)popViewController {
    [self.navigationController popViewControllerAnimated:true];
}

- (void)initBackButton {
    UIButton *button = [[UIButton alloc] initWithFrame:CGRectMake(0, CGRectGetMaxY(self.navigationController.navigationBar.frame) - 44, 72, 44)];
    [button setTitle:@"返回" forState:UIControlStateNormal];
    [button setTitleColor:[UIColor linkColor] forState:UIControlStateNormal];
    button.titleLabel.font = [UIFont systemFontOfSize:16];
    button.backgroundColor = [UIColor clearColor];
    [button addTarget:self action:@selector(popViewController) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:button];
}

- (void)initCameraView {
    _cameraView = [[CameraPreviewView alloc] initWithFrame:self.view.bounds];
    _cameraView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self.view addSubview:_cameraView];
}

- (void)initCamera {
    __weak typeof(self) weakSelf = self;
    [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
        if (granted) {
            dispatch_async(dispatch_get_main_queue(), ^{
                __strong typeof(self) strongSelf = weakSelf;
                [strongSelf initCaptureSession];
            });
        }
    }];
}

- (void)initCaptureSession {
    // 获取摄像头
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera] mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionBack];
    if (discoverySession.devices.count == 0) {
        return;
    }
    _videoCaptureDevice = discoverySession.devices.firstObject;
    // 输入
    NSError *error = nil;
    AVCaptureDeviceInput *deviceInput = [AVCaptureDeviceInput deviceInputWithDevice:_videoCaptureDevice error:&error];
    if (!deviceInput){
        return;
    }
    // 输出
    _captureSessionQueue = dispatch_queue_create("com.tencent.mobileocr.demo.capture", DISPATCH_QUEUE_SERIAL);
    AVCaptureVideoDataOutput *videoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
    videoDataOutput.alwaysDiscardsLateVideoFrames = YES;
    videoDataOutput.videoSettings = @{(__bridge id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)};
    [videoDataOutput setSampleBufferDelegate:self queue:_captureSessionQueue];
    // Session
    _captureSession = [[AVCaptureSession alloc] init];
    _captureSession.sessionPreset = AVCaptureSessionPreset1280x720;
    [_captureSession addInput:deviceInput];
    [_captureSession addOutput:videoDataOutput];
    [_cameraView setSession:_captureSession];
}

- (void)startCapture {
    dispatch_async(_captureSessionQueue, ^{
        if (!self.captureSession.isRunning) {
            [self.captureSession startRunning];
        }
    });
    // 仅测试用：待相机画面稳定后，再开启识别
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), _captureSessionQueue, ^{
        self.shouldScan = YES;
        self.scanCounter = 0;
    });
}

- (void)stopCapture {
    dispatch_async(_captureSessionQueue, ^{
        self.shouldScan = NO;
        if (self.captureSession.isRunning) {
            [self.captureSession stopRunning];
        }
    });
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
    if (!_shouldScan) {
        return;
    }
    NSLog(@"Scaning... [%li]", ++_scanCounter);
//    NSArray<DetectedObject *> *detectedObjects = [_qbar scanImage:[GrayscaleImage imageWithCMSampleBufferRef:sampleBuffer]];
//    if (detectedObjects.count > 0) {
//        _shouldScan = NO;
//        [_captureSession stopRunning];
//        dispatch_async(dispatch_get_main_queue(), ^{
//            PreviewViewController *vc = [[PreviewViewController alloc] initWithImage:nil detectedObjects:detectedObjects];
//            [self.navigationController pushViewController:vc animated:YES];
//        });
//    }
}

@end
