
import cv2
import argparse
import time

import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision

from flask import Flask, render_template, Response

from utils import visualize


app = Flask(__name__)

@app.route('/')
def index():
    """Video streaming home page."""
    return render_template('index.html')


# Глобальные переменные для расчета FPS
COUNTER, FPS = 0, 0
START_TIME = time.time()


def run(model: str, max_results: int, score_threshold: float,
        camera_id: int, width: int, height: int) -> None:
    """Непрерывное выполнение вывода на изображениях, полученных с камеры.

    Args:
      model: Имя модели обнаружения объектов TFLite.
      max_results: Максимальное количество результатов обнаружения.
      score_threshold: Порог оценки результатов обнаружения.
      camera_id: Идентификатор камеры, передаваемый в OpenCV.
      width: Ширина кадра, захваченного с камеры.
      height: Высота кадра, захваченного с камеры.
    """

    # Запуск захвата видеосигнала с камеры
    cap = cv2.VideoCapture(camera_id)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    # Параметры визуализации
    row_size = 50  # пиксели
    left_margin = 24  # пиксели
    text_color = (0, 0, 0)  # черный
    font_size = 1
    font_thickness = 1
    fps_avg_frame_count = 10

    detection_result_list = []

    def save_result(result: vision.ObjectDetectorResult, unused_output_image: mp.Image, timestamp_ms: int):
        global FPS, COUNTER, START_TIME

        # Вычисление FPS
        if COUNTER % fps_avg_frame_count == 0:
            FPS = fps_avg_frame_count / (time.time() - START_TIME)
            START_TIME = time.time()

        detection_result_list.append(result)
        COUNTER += 1

    # Инициализация модели обнаружения объектов
    base_options = python.BaseOptions(model_asset_path=model)
    options = vision.ObjectDetectorOptions(base_options=base_options,
                                           running_mode=vision.RunningMode.LIVE_STREAM,
                                           max_results=max_results, score_threshold=score_threshold,
                                           result_callback=save_result)
    detector = vision.ObjectDetector.create_from_options(options)

    image_center = (width // 2, height // 2)

    # Непрерывный захват изображений с камеры и выполнение вывода
    while cap.isOpened():
        success, image = cap.read()
        if not success:
            break

        image = cv2.flip(image, 1)

        # Преобразование изображения из BGR в RGB, как требуется моделью TFLite.
        rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb_image)

        # Запуск обнаружения объектов с использованием модели.
        detector.detect_async(mp_image, time.time_ns() // 1_000_000)

        # Показать FPS
        fps_text = 'FPS = {:.1f}'.format(FPS)
        text_location = (left_margin, row_size)
        current_frame = image

        cv2.circle(image, image_center, 3, (0, 0, 255), -1)

        cv2.putText(current_frame, fps_text, text_location, cv2.FONT_HERSHEY_DUPLEX,
                    font_size, text_color, font_thickness, cv2.LINE_AA)

        if detection_result_list:
            current_frame = visualize(current_frame, detection_result_list[0])
            detection_result_list.clear()

        ret, jpeg = cv2.imencode('.jpg', image)
        frame = jpeg.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

    detector.close()
    cap.release()


@app.route('/video_feed')
def video_feed():
    return Response(run(args.model, int(args.maxResults),
                        args.scoreThreshold, int(args.cameraId), args.frameWidth, args.frameHeight),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '--model',
        help='Путь к модели обнаружения объектов.',
        required=False,
        default='/balloons2.tflite')
    parser.add_argument(
        '--maxResults',
        help='Максимальное количество результатов обнаружения.',
        required=False,
        default=3)
    parser.add_argument(
        '--scoreThreshold',
        help='Порог оценки результатов обнаружения.',
        required=False,
        type=float,
        default=0.25)
    parser.add_argument(
        '--cameraId', help='Идентификатор камеры.', required=False, type=int, default=0)
    parser.add_argument(
        '--frameWidth',
        help='Ширина кадра для захвата с камеры.',
        required=False,
        type=int,
        default=1280)
    parser.add_argument(
        '--frameHeight',
        help='Высота кадра для захвата с камеры.',
        required=False,
        type=int,
        default=720)

    args = parser.parse_args()

    app.run(host='0.0.0.0', threaded=True, debug=False)
