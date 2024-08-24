import cv2 as cv
import argparse


# Valid combinations of backends and targets
backend_target_pairs = [
    [cv.dnn.DNN_BACKEND_OPENCV, cv.dnn.DNN_TARGET_CPU],
    [cv.dnn.DNN_BACKEND_CUDA,   cv.dnn.DNN_TARGET_CUDA],
    [cv.dnn.DNN_BACKEND_CUDA,   cv.dnn.DNN_TARGET_CUDA_FP16],
    [cv.dnn.DNN_BACKEND_TIMVX,  cv.dnn.DNN_TARGET_NPU],
    [cv.dnn.DNN_BACKEND_CANN,   cv.dnn.DNN_TARGET_NPU]
]


parser = argparse.ArgumentParser(
    description="WeChat QR code detector for detecting and parsing QR code (https://github.com/opencv/opencv_contrib/tree/master/modules/wechat_qrcode)")
parser.add_argument('--input', '-i', type=str,
                    help='Usage: Set path to the input image. Omit for using default camera.')
parser.add_argument('--backend_target', '-bt', type=int, default=0,
                    help='''Choose one of the backend-target pair to run this demo:
                        {:d}: (default) OpenCV implementation + CPU,
                        {:d}: CUDA + GPU (CUDA),
                        {:d}: CUDA + GPU (CUDA FP16),
                        {:d}: TIM-VX + NPU,
                        {:d}: CANN + NPU
                    '''.format(*[x for x in range(len(backend_target_pairs))]))
parser.add_argument('--save', '-s', action='store_true',
                    help='Usage: Specify to save file with results (i.e. bounding box, confidence level). Invalid in case of camera input.')
parser.add_argument('--vis', '-v', action='store_true',
                    help='Usage: Specify to open a new window to show results. Invalid in case of camera input.')
args = parser.parse_args()


def visualize(image, res, points, fps=-1.0):
    text_color = (0, 255, 0)

    output_image = image.copy()

    if fps >= 0:
        cv.putText(output_image, f"FPS: {fps:.2f}", (0, 15), cv.FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2)

    for i in range(len(points)):
        center_x = 0
        center_y = 0
        
        for r in range(points[i].shape[0]):
            point = (int(points[i][r, 0]), int(points[i][r, 1]))
            cv.circle(output_image, point, 5, (0, 255, 0), -1)
            center_x += points[i][r, 0]
            center_y += points[i][r, 1]

        center_x /= points[i].shape[0]
        center_y /= points[i].shape[0]
        
        text_size = cv.getTextSize(res[i], cv.FONT_HERSHEY_DUPLEX, 0.5, 1)[0]
        text_x = int(center_x - text_size[0] / 2)
        text_y = int(center_y + text_size[1] / 2)
        
        cv.putText(output_image, res[i], (text_x, text_y), cv.FONT_HERSHEY_DUPLEX, 0.5, text_color, 1)

    return output_image

if __name__ == '__main__':
    backend_id = backend_target_pairs[args.backend_target][0]
    target_id = backend_target_pairs[args.backend_target][1]
    
    qbar = cv.QBarAI_QBar("models/detector/qbar_det.yml", "models/sr/qbar_sr.yml")

    if args.input is not None:
        image = cv.imread(args.input)
        res, points = qbar.detectAndDecode(image)

        # Print results:
        print(res)
        print(points)

        # Draw results on the input image
        image = visualize(image, res, points)

        # Save results if save is true
        if args.save:
            print('Results saved to result.jpg\n')
            cv.imwrite('result.jpg', image)

        # Visualize results in a new window
        if args.vis:
            cv.namedWindow(args.input, cv.WINDOW_AUTOSIZE)
            cv.imshow(args.input, image)
            cv.waitKey(0)
    else:
        deviceId = 0
        cap = cv.VideoCapture(deviceId)
        
        tm = cv.TickMeter()
        while cv.waitKey(1) < 0:
            hasFrame, frame = cap.read()
            if not hasFrame:
                print('No frames grabbed!')
                break

            # Inference
            tm.start()
            res, points = qbar.detectAndDecode(frame)
            tm.stop()
            fps = tm.getFPS()

            # Draw results on the input image
            frame = visualize(frame, res, points, fps=fps)

            # Visualize results in a new window
            cv.imshow('WeChatQRCode Demo', frame)

            tm.reset()