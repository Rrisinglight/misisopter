import cv2
import numpy as np
import argparse
from tqdm import tqdm
import time

# Пороговые значения для детектора границ Canny
canny_threshold1 = 70
canny_threshold2 = 170

fallback_video_path = './video/testvideo_1.mp4'

def process_frame(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray, canny_threshold1, canny_threshold2)
    return edges

def main():
    parser = argparse.ArgumentParser(description='Скрипт для обнаружения границ в видео')
    parser.add_argument('--video', help='Путь к видеофайлу (если не указан, используется камера)', default=None)
    parser.add_argument('--output', help='Путь для сохранения обработанного видео', default='output_video.avi')
    args = parser.parse_args()

    cap = cv2.VideoCapture(0 if args.video is None else args.video)
    if not cap.isOpened():
        if args.video is None:
            cap = cv2.VideoCapture(fallback_video_path)
            if not cap.isOpened():
                print(f"Не удалось открыть камеру или файл {fallback_video_path}")
                exit()
        else:
            print(f"Не удалось открыть видео файл {args.video}")
            exit()

    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    if fps == 0.0 or fps is None:
        fps = 25.0

    out = cv2.VideoWriter(
        args.output,
        cv2.VideoWriter_fourcc(*'XVID'),
        fps,
        (frame_width, frame_height),
        isColor=False
    )

    print("Начало обработки видео...")
    start_time = time.time()

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    if total_frames <= 0:
        total_frames = None

    pbar = tqdm(total=total_frames, desc='Обработка кадров', unit='кадр', ncols=80)

    frame_count = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        frame_start_time = time.time()

        edges = process_frame(frame)
        out.write(edges)

        frame_end_time = time.time()
        frame_processing_time = frame_end_time - frame_start_time

        frame_count += 1
        pbar.update(1)
        pbar.set_postfix({
            'Кадр': frame_count,
            'Время на кадр (с)': f'{frame_processing_time:.4f}'
        })

    pbar.close()
    cap.release()
    out.release()
    cv2.destroyAllWindows()
    total_time = time.time() - start_time

    print(f"Видео обработано и сохранено как '{args.output}'")
    print(f"Общее время обработки: {total_time:.2f} секунд")

if __name__ == '__main__':
    main()