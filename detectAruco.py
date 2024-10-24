# import the necessary packages
import matplotlib.pyplot as plt
import cv2
import numpy as np


ARUCO_DICT = {
	"DICT_4X4_50": cv2.aruco.DICT_4X4_50,
	"DICT_4X4_100": cv2.aruco.DICT_4X4_100,
	"DICT_4X4_250": cv2.aruco.DICT_4X4_250,
	"DICT_4X4_1000": cv2.aruco.DICT_4X4_1000,
	"DICT_5X5_50": cv2.aruco.DICT_5X5_50,
	"DICT_5X5_100": cv2.aruco.DICT_5X5_100,
	"DICT_5X5_250": cv2.aruco.DICT_5X5_250,
	"DICT_5X5_1000": cv2.aruco.DICT_5X5_1000,
	"DICT_6X6_50": cv2.aruco.DICT_6X6_50,
	"DICT_6X6_100": cv2.aruco.DICT_6X6_100,
	"DICT_6X6_250": cv2.aruco.DICT_6X6_250,
	"DICT_6X6_1000": cv2.aruco.DICT_6X6_1000,
	"DICT_7X7_50": cv2.aruco.DICT_7X7_50,
	"DICT_7X7_100": cv2.aruco.DICT_7X7_100,
	"DICT_7X7_250": cv2.aruco.DICT_7X7_250,
	"DICT_7X7_1000": cv2.aruco.DICT_7X7_1000,
	"DICT_ARUCO_ORIGINAL": cv2.aruco.DICT_ARUCO_ORIGINAL,
}
aruco_type = "DICT_7X7_100"
arucoDict = cv2.aruco.getPredefinedDictionary(ARUCO_DICT[aruco_type])
arucoParams = cv2.aruco.DetectorParameters()

# corners, ids, rejected = cv2.aruco.detectMarkers(aboba, arucoDict, parameters=arucoParams)

def detect_markers(image):
    
    aruco_type_list = []
    
    for aruco_type, dictionary_id in ARUCO_DICT.items():

        arucoDict = cv2.aruco.getPredefinedDictionary(dictionary_id)
        arucoParams = cv2.aruco.DetectorParameters()
        # detector = cv2.aruco.ArucoDetector(arucoDict, arucoParams)

        corners, ids, _ = cv2.aruco.detectMarkers(image, arucoDict, parameters=arucoParams)

        if len(corners) > 0:
            
            aruco_type_list.append(aruco_type)
            
#             print(f"Markers detected using {aruco_type} dictionary")

            for markerCorner, markerId in zip(corners, ids.flatten()):
                corners_aruco = markerCorner.reshape((4, 2))
                (topLeft, topRight, bottomRight, bottomLeft) = corners_aruco

                cv2.polylines(image, [markerCorner.astype(int)], True, (0, 255, 0), 2)

                cX = int((topLeft[0] + bottomRight[0]) / 2)
                cY = int((topLeft[1] + bottomRight[1]) / 2)

                cv2.circle(image, (cX, cY), 5, (255, 0, 0), -1)
                cv2.putText(image, str(aruco_type) + " " + str(int(markerId)),
                            (int(topLeft[0] - 5), int(topLeft[1])), cv2.FONT_HERSHEY_COMPLEX, 0.5, (255, 0, 255))

            # break  # Stop iterating once markers are detected        
        # cv2.imshow("Detected Markers", image)
            
    return aruco_type_list

def pose_estimation(frame, aruco_dict_type, matrix_coefficients, distortion_coefficients):
    
    aruco_dict = cv2.aruco.getPredefinedDictionary(aruco_dict_type)
    parameters = cv2.aruco.DetectorParameters()
    # detector = cv2.aruco.ArucoDetector(aruco_dict, parameters)

    corners, ids, rejected = cv2.aruco.detectMarkers(frame, arucoDict, parameters=arucoParams)

    if len(corners) > 0:
        for i in range(0, len(ids)):
           
            rvec, tvec, markerPoints = cv2.aruco.estimatePoseSingleMarkers(corners[i], 0.025, matrix_coefficients, distortion_coefficients)
            posss = str(tvec)[3:-3].split(' ')
            print('Position: x:{0}   y:{1} z:{2}'.format(posss[0],posss[1],posss[2]))
            cv2.aruco.drawDetectedMarkers(frame, corners) 
            print(f"tvec: {tvec[i]}")
            cv2.putText(image, f"Position: {tvec[i]}", (20, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

            cv2.drawFrameAxes(frame, matrix_coefficients, distortion_coefficients, rvec, tvec, 0.01) 
             
    return frame

if __name__ == "__main__":

    image_path = r"arucoMarkers/singlemarkersoriginal.jpg"

    intrinsic_camera = np.array(((933.15867, 0, 657.59), (0, 933.1586, 400.36993), (0, 0, 1)))
    distortion = np.array((-0.43948, 0.18514, 0, 0))

    cap = cv2.VideoCapture(1)
    # cap = picam2.capture_array()

    # Счетчик для сохранения кадров
    frame_count = 0
    max_frames_to_save = 3

    # # Читаем и сохраняем первые 3 кадра
    # while frame_count < max_frames_to_save:
    #     ret, frame = cap.read()

    #     # Проверяем, успешно ли захвачен кадр
    #     if not ret:
    #         print("Failed to grab frame.")
    #         break

    #     # # Сохраняем кадр в файл
    #     # frame_filename = f"frame_{frame_count + 1}.jpg"
    #     # cv2.imwrite(frame_filename, frame)
    #     # print(f"Saved {frame_filename}")

    #     frame_count += 1
        
    while cap.isOpened():
        ret, image = cap.read()

        for aruco_type in detect_markers(image):
             image = pose_estimation(image, ARUCO_DICT[aruco_type], intrinsic_camera, distortion)
        
        
        # cv2.imshow('Estimated Pose', image)
                
        if cv2.waitKey(50) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()