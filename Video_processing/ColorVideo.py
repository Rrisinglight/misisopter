import cv2
import numpy as np
import argparse
from tqdm import tqdm
import time

#print(cv2.getBuildInformation())

# (Denoising)
bilateral_d = 8
bilateral_sigmaColor = 25
bilateral_sigmaSpace = 40

# (AWB)
awb_enable = True
awb_algorithm = 'gray_world'

# Авто контраст и яркость
auto_brightness_contrast = True
contrast_method = 'clahe'

# Параметры для CLAHE
clahe_clipLimit = 0.7
clahe_tileGridSize = (8, 8)

# Управление насыщенностью
saturation_scale = 1.8

# Unsharp Masking
unsharp_kernel_size = (7, 7)
unsharp_sigma = 1.0
unsharp_amount = 1.4
unsharp_threshold = 0.1

# Коррекция гаммы
gamma_value = 1


def denoise(img):
    return cv2.bilateralFilter(img, d=bilateral_d, sigmaColor=bilateral_sigmaColor, sigmaSpace=bilateral_sigmaSpace)


def apply_awb(img):
    if not awb_enable:
        return img
    result = img.astype(np.float32)
    avg_b, avg_g, avg_r = cv2.mean(result)[:3]
    avg_gray = (avg_b + avg_g + avg_r) / 3

    result[:, :, 0] *= avg_gray / (avg_b + 1e-6)
    result[:, :, 1] *= avg_gray / (avg_g + 1e-6)
    result[:, :, 2] *= avg_gray / (avg_r + 1e-6)

    np.clip(result, 0, 255, out=result)
    return result.astype(np.uint8)


def apply_auto_contrast(img):
    if not auto_brightness_contrast:
        return img
    if contrast_method == 'clahe':
        lab = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
        l_channel, a_channel, b_channel = cv2.split(lab)

        clahe = cv2.createCLAHE(clipLimit=clahe_clipLimit, tileGridSize=clahe_tileGridSize)
        l_channel = clahe.apply(l_channel)

        lab = cv2.merge((l_channel, a_channel, b_channel))
        result = cv2.cvtColor(lab, cv2.COLOR_LAB2BGR)
        return result
    elif contrast_method == 'hist_equalization':
        img_yuv = cv2.cvtColor(img, cv2.COLOR_BGR2YUV)
        img_yuv[:, :, 0] = cv2.equalizeHist(img_yuv[:, :, 0])
        result = cv2.cvtColor(img_yuv, cv2.COLOR_YUV2BGR)
        return result
    return img


def adjust_saturation(img):
    img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV).astype(np.float32)
    img_hsv[:, :, 1] *= saturation_scale
    np.clip(img_hsv[:, :, 1], 0, 255, out=img_hsv[:, :, 1])
    img_hsv = img_hsv.astype(np.uint8)
    return cv2.cvtColor(img_hsv, cv2.COLOR_HSV2BGR)


def unsharp_mask(img):
    blurred = cv2.GaussianBlur(img, unsharp_kernel_size, unsharp_sigma)
    sharpened = cv2.addWeighted(img, unsharp_amount + 1, blurred, -unsharp_amount, 0)
    np.clip(sharpened, 0, 255, out=sharpened)
    return sharpened.astype(np.uint8)


def adjust_gamma(img):
    if gamma_value == 1.0:
        return img
    inv_gamma = 1.0 / gamma_value
    table = (np.arange(256) / 255.0) ** inv_gamma * 255
    return cv2.LUT(img, table.astype(np.uint8))


def main():
    parser = argparse.ArgumentParser(description='Video Enhancement Script')
    parser.add_argument('--video', help='Путь к видеофайлу (если не указан, используется камера)', default=None)
    parser.add_argument('--output', help='Путь для сохранения обработанного видео', default='output.avi')
    args = parser.parse_args()

    cap = cv2.VideoCapture(0 if args.video is None else args.video)
    if not cap.isOpened():
        print("Не удалось открыть источник видео")
        exit()

    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    if fps == 0.0 or fps is None:
        fps = 25.0

    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(args.output, fourcc, fps, (frame_width, frame_height))

    print("Начало обработки видео...")
    start_time = time.time()

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    if total_frames <= 0:
        total_frames = None

    if total_frames is not None and total_frames > 0:
        pbar = tqdm(total=total_frames, desc='Обработка кадров')
    else:
        pbar = tqdm(desc='Обработка кадров')

    frame_count = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        frame_count += 1
        frame_start_time = time.time()

        frame = apply_awb(frame)
        frame = apply_auto_contrast(frame)
        frame = denoise(frame)
        frame = adjust_saturation(frame)
        frame = adjust_gamma(frame)
        frame = unsharp_mask(frame)

        out.write(frame)

        frame_end_time = time.time()
        frame_processing_time = frame_end_time - frame_start_time

        pbar.update(1)
        pbar.set_postfix({'Кадр': frame_count, 'Время на кадр (с)': f'{frame_processing_time:.4f}'})


    pbar.close()
    cap.release()
    out.release()
    total_time = time.time() - start_time

    print(f"Обработанное видео сохранено в {args.output}")
    print(f"Общее время обработки: {total_time:.2f} секунд")


if __name__ == '__main__':
    main()
