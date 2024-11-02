import cv2
import numpy as np
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision

# Путь к изображению
IMAGE_FILE = '/image.jpg'

# Загрузка изображения
img = cv2.imread(IMAGE_FILE)

# Параметры визуализации
MARGIN = 10  # пиксели
ROW_SIZE = 10  # пиксели
FONT_SIZE = 1
FONT_THICKNESS = 1
TEXT_COLOR = (255, 0, 0)  # красный

def visualize(image, detection_result) -> np.ndarray:
    """Отрисовка ограничительных рамок на входном изображении и его возврат.
    Args:
        image: Входное RGB изображение.
        detection_result: Список всех объектов "Detection" для визуализации.
    Returns:
        Изображение с ограничительными рамками.
    """
    for detection in detection_result.detections:
        # Рисование ограничительной рамки
        bbox = detection.bounding_box
        start_point = bbox.origin_x, bbox.origin_y
        end_point = bbox.origin_x + bbox.width, bbox.origin_y + bbox.height
        cv2.rectangle(image, start_point, end_point, TEXT_COLOR, 3)

        # Рисование метки и оценки
        category = detection.categories[0]
        category_name = category.category_name
        probability = round(category.score, 2)
        result_text = category_name + ' (' + str(probability) + ')'
        text_location = (MARGIN + bbox.origin_x,
                         MARGIN + ROW_SIZE + bbox.origin_y)
        cv2.putText(image, result_text, text_location, cv2.FONT_HERSHEY_PLAIN,
                    FONT_SIZE, TEXT_COLOR, FONT_THICKNESS)

    return image

# Создание объекта ObjectDetector
base_options = python.BaseOptions(model_asset_path="/balloons2.tflite")
options = vision.ObjectDetectorOptions(base_options=base_options,
                                       running_mode=vision.RunningMode.IMAGE,
                                       max_results=2, score_threshold=0.5,
                                       result_callback=None)
detector = vision.ObjectDetector.create_from_options(options)

# Загрузка входного изображения
image = mp.Image.create_from_file(IMAGE_FILE)

# Обнаружение объектов на входном изображении
detection_result = detector.detect(image)

# Обработка результата обнаружения. В данном случае, визуализация.
image_copy = np.copy(image.numpy_view())
annotated_image = visualize(image_copy, detection_result)
rgb_annotated_image = cv2.cvtColor(annotated_image, cv2.COLOR_BGR2RGB)
cv2.imwrite('output_image.jpg', rgb_annotated_image)
