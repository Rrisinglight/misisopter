
import cv2
import numpy as np


MARGIN = 10  # pixels
ROW_SIZE = 30  # pixels
FONT_SIZE = 1
FONT_THICKNESS = 1
TEXT_COLOR = (255, 255, 0)  # yellow


def visualize(
    image,
    detection_result
) -> np.ndarray:
    """Рисует ограничительные рамки на входном изображении и возвращает их.
    Args:
        image: Входное RGB-изображение.
        detection_result: Список всех визуализируемых сущностей "Detection".
    Returns:
        Изображение с ограничивающими рамками.
    """
    for detection in detection_result.detections:

        # Draw bounding_box
        bbox = detection.bounding_box
        start_point = bbox.origin_x, bbox.origin_y
        end_point = bbox.origin_x + bbox.width, bbox.origin_y + bbox.height
        center = (bbox.origin_x + bbox.width // 2,
                bbox.origin_y + bbox.height // 2)
        
        cv2.rectangle(image, start_point, end_point, TEXT_COLOR, 3)
        cv2.circle(image, center, 2, (0, 0, 255), -1)

        image_center = (image.shape[1] // 2, image.shape[0] // 2)
        
        # Draw line between image center and bbox center
        cv2.line(image, image_center, center, (0, 0, 130), 1)

        # Draw label and score
        category = detection.categories[0]
        category_name = category.category_name
        probability = round(category.score, 2)
        result_text = category_name + ' (' + str(probability) + ')'
        text_location = (MARGIN + bbox.origin_x,
                        MARGIN + ROW_SIZE + bbox.origin_y)
        cv2.putText(image, result_text, text_location, cv2.FONT_HERSHEY_DUPLEX,
                    FONT_SIZE, TEXT_COLOR, FONT_THICKNESS, cv2.LINE_AA)

    return image