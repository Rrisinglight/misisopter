import cv2
import numpy as np

fallback_video_path = './video/testvideo_1.mp4'

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    cap = cv2.VideoCapture(fallback_video_path)


frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = int(cap.get(cv2.CAP_PROP_FPS))


out = cv2.VideoWriter('output_video.avi', cv2.VideoWriter_fourcc(*'XVID'), fps, (frame_width, frame_height), False)

while cap.isOpened():
    ret, frame = cap.read()

    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    edges = cv2.Canny(gray, 70, 170)

    out.write(edges)


cap.release()
out.release()
cv2.destroyAllWindows()

print("Видео обработано и сохранено как 'output_video.avi'")
