//
//  RootViewController.m
//  demo
//
//  Created by zhaozhenyang on 2024/4/20.
//

#import <Photos/Photos.h>
#import "RootViewController.h"
#import "PreviewViewController.h"
#import "CameraViewController.h"
#import "QBarSDK.h"

@interface RootViewController () <UINavigationControllerDelegate, UIImagePickerControllerDelegate>

@end

@implementation RootViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (IBAction)openCamera {
    UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"等接入模型后再开放" message:@"" preferredStyle:UIAlertControllerStyleAlert];
    [controller addAction:[UIAlertAction actionWithTitle:@"知道了" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        return;
    }]];
    [self presentViewController:controller animated:YES completion:nil];
    
//    CameraViewController *vc = [[CameraViewController alloc] initWithSDKCapability:_allSDKCapabilities[_selectedSDKCapabilityIndex]];
//    [self.navigationController pushViewController:vc animated:true];
}

- (IBAction)selectImage {
    __weak typeof(self) weakSelf = self;
    void (^handler)(PHAuthorizationStatus status) = ^(PHAuthorizationStatus status) {
        if (status == PHAuthorizationStatusAuthorized) {
            dispatch_async(dispatch_get_main_queue(), ^{
                __strong typeof(self) strongSelf = weakSelf;
                UIImagePickerController *controller = [[UIImagePickerController alloc] init];
                controller.delegate = strongSelf;
                controller.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
                [strongSelf.navigationController presentViewController:controller animated:YES completion:nil];
            });
        }
    };
    if (@available(iOS 14, *)) {
        [PHPhotoLibrary requestAuthorizationForAccessLevel:PHAccessLevelReadWrite handler:handler];
    } else {
        [PHPhotoLibrary requestAuthorization:handler];
    }
}

#pragma mark - UIImagePickerController Delegate

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<UIImagePickerControllerInfoKey, id> *)info {
    UIImage *image = info[UIImagePickerControllerOriginalImage];
    QBarSDK *qbar = [[QBarSDK alloc] init];
    NSArray<DetectedObject *> *detectedObjects = [qbar scanImage:[GrayscaleImage imageWithUIImage:image]];
    PreviewViewController *vc = [[PreviewViewController alloc] initWithImage:image detectedObjects:detectedObjects];
    [self.navigationController pushViewController:vc animated:NO];
    [picker dismissViewControllerAnimated:YES completion:nil];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
    [picker dismissViewControllerAnimated:YES completion:nil];
}

@end
