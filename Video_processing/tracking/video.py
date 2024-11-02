import cv2
import numpy as np
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision

# Путь к видеофайлу
VIDEO_FILE = '/test_video.mp4'
OUTPUT_VIDEO_FILE = 'output_video.mp4'  # Путь к выходному видеофайлу

MARGIN = 10  # пиксели
ROW_SIZE = 10  # пиксели
FONT_SIZE = 1
FONT_THICKNESS = 1
TEXT_COLOR = (255, 0, 0)  # красный

def visualize(image, detection_result) -> np.ndarray:
    """Наносит ограничивающие рамки на изображение и возвращает его."""
    for detection in detection_result.detections:
        bbox = detection.bounding_box
        start_point = int(bbox.origin_x), int(bbox.origin_y)
        end_point = int(bbox.origin_x + bbox.width), int(bbox.origin_y + bbox.height)
        cv2.rectangle(image, start_point, end_point, TEXT_COLOR, 3)

        category = detection.categories[0]
        category_name = category.category_name
        probability = round(category.score, 2)
        result_text = f"{category_name} ({probability})"
        text_location = (MARGIN + int(bbox.origin_x), MARGIN + ROW_SIZE + int(bbox.origin_y))
        cv2.putText(image, result_text, text_location, cv2.FONT_HERSHEY_PLAIN, FONT_SIZE, TEXT_COLOR, FONT_THICKNESS)

    return image

# Создание объекта ObjectDetector
base_options = python.BaseOptions(model_asset_path="/balloons2.tflite")
options = vision.ObjectDetectorOptions(base_options=base_options, running_mode=vision.RunningMode.IMAGE,
                                       max_results=2, score_threshold=0.15, result_callback=None)
detector = vision.ObjectDetector.create_from_options(options)

cap = cv2.VideoCapture(VIDEO_FILE)

# Получение информации о видео (ширина, высота, кадры в секунду)
width = int(cap.get(3))
height = int(cap.get(4))
fps = int(cap.get(cv2.CAP_PROP_FPS))  # Загрузка частоты кадров с использованием CAP_PROP_FPS

# Определение кодека
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter(OUTPUT_VIDEO_FILE, fourcc, fps, (width, height))

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Преобразование кадра в объект изображения MediaPipe
    mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))

    # Обнаружение объектов на кадре
    detection_result = detector.detect(mp_image)

    # Визуализация результата обнаружения
    image_copy = np.copy(frame)
    annotated_image = visualize(image_copy, detection_result)

    # Запись в выходное видео
    out.write(annotated_image)

    # Отображение результата
    # cv2.imshow('Обнаружение объектов', annotated_image)
    
cap.release()
out.release()
cv2.destroyAllWindows()