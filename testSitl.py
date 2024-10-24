# from dronekit import connect, VehicleMode, LocationGlobalRelative
# from pymavlink import mavutil
# import time
# import argparse  

# parser = argparse.ArgumentParser()
# parser.add_argument('--connect', default='127.0.0.1:14550')
# args = parser.parse_args()

# # Connect to the Vehicle
# print('Connecting to vehicle on: %s' % args.connect)
# vehicle = connect(args.connect, baud=921600, wait_ready=True)
# # 921600 is the baudrate that you have set in the mission planner or QGC

# # Function to arm and then takeoff to a user-specified altitude
# def arm_and_takeoff(aTargetAltitude):
#     print("Basic pre-arm checks")
#     # Don't let the user try to arm until autopilot is ready
#     while not vehicle.is_armable:
#         print(" Waiting for vehicle to initialise...")
#         time.sleep(1)
        
#     print("Arming motors")
#     # Copter should arm in GUIDED mode
#     vehicle.mode = VehicleMode("GUIDED")
#     vehicle.armed = True

#     while not vehicle.armed:
#         print(" Waiting for arming...")
#         time.sleep(1)

#     print("Taking off!")
#     vehicle.simple_takeoff(aTargetAltitude)  # Take off to target altitude

#     # Check that vehicle has reached takeoff altitude
#     while True:
#         print(" Altitude: ", vehicle.location.global_relative_frame.alt)
#         # Break and return from function just below target altitude.        
#         if vehicle.location.global_relative_frame.alt >= aTargetAltitude * 0.95: 
#             print("Reached target altitude")
#             break
#         time.sleep(1)

# # Initialize the takeoff sequence to 15m
# arm_and_takeoff(15)

# print("Take off complete")

# # Hover for 10 seconds
# time.sleep(15)

# print("Now let's land")
# vehicle.mode = VehicleMode("LAND")

# # Close vehicle object
# vehicle.close()


import time
import cv2
import numpy as np
import cv2.aruco as aruco
from dronekit import connect, VehicleMode
from pymavlink import mavutil

# Параметры подключения к Pixhawk
connection_string = '/dev/ttyS0'  # Замените на ваш порт
baud_rate = 57600

# Подключение к коптеру
print("Подключение к коптеру на: %s" % connection_string)
vehicle = connect(connection_string, baud=baud_rate, wait_ready=True)

# Функция для армирования и взлёта
def arm_and_takeoff(aTargetAltitude):
    print("Проверка готовности системы...")
    while not vehicle.is_armable:
        print(" Ожидание готовности...")
        time.sleep(1)

    print("Армирование моторов")
    vehicle.mode = VehicleMode("GUIDED")
    vehicle.armed = True

    while not vehicle.armed:
        print(" Ожидание арминга...")
        time.sleep(1)

    print("Взлёт")
    vehicle.simple_takeoff(aTargetAltitude)

    while True:
        altitude = vehicle.location.global_relative_frame.alt
        print(" Высота: %.2f м" % altitude)
        if altitude >= aTargetAltitude * 0.95:
            print("Достигнута целевая высота")
            break
        time.sleep(1)

# Функция для отправки NED-скоростей
def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
    msg = vehicle.message_factory.set_position_target_local_ned_encode(
        0,       # время от начала (не используется)
        0, 0,    # системный ID, ID компонента
        mavutil.mavlink.MAV_FRAME_BODY_NED,  # Система координат
        0b0000111111000111,  # Типы данных для контроля
        0, 0, 0,             # координаты (не используются)
        velocity_x, velocity_y, velocity_z,  # скорости
        0, 0, 0,             # ускорения (не используются)
        0, 0)
    vehicle.send_mavlink(msg)
    vehicle.flush()

# Настройка камеры и Aruco
cap = cv2.VideoCapture(0)
aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_50)
parameters = aruco.DetectorParameters_create()

# Калибровка камеры (замените на свои значения)
camera_matrix = np.array([[640, 0, 320],
                          [0, 480, 240],
                          [0, 0, 1]], dtype=np.float32)
dist_coeffs = np.zeros((5, 1))

# Основной цикл
try:
    arm_and_takeoff(7)

    start_time = time.time()
    while time.time() - start_time < 30:
        ret, frame = cap.read()
        if not ret:
            continue

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        corners, ids, _ = aruco.detectMarkers(
            gray, aruco_dict, parameters=parameters)

        if np.all(ids is not None):
            # Предполагаемый размер маркера (метры)
            marker_size = 0.1

            # Получение позы маркера
            rvecs, tvecs, _ = aruco.estimatePoseSingleMarkers(
                corners, marker_size, camera_matrix, dist_coeffs)

            # Отображение на экране (опционально)
            for i in range(len(ids)):
                aruco.drawAxis(frame, camera_matrix,
                               dist_coeffs, rvecs[i], tvecs[i], 0.1)
                aruco.drawDetectedMarkers(frame, corners)

            # Получение координат
            x = tvecs[0][0][0]
            y = tvecs[0][0][1]
            z = tvecs[0][0][2]

            # Преобразование в скорости
            k = 0.5  # коэффициент пропорциональности
            velocity_x = -y * k
            velocity_y = x * k
            velocity_z = 0  # Держим высоту постоянной

            send_ned_velocity(velocity_x, velocity_y, velocity_z, 1)
        else:
            # Если маркер не найден, удерживаем позицию
            send_ned_velocity(0, 0, 0, 1)

        cv2.imshow('Frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    print("Удержание позиции завершено. Начинаем посадку.")
    vehicle.mode = VehicleMode("LAND")

    while vehicle.armed:
        print(" Высота: %.2f м" % vehicle.location.global_relative_frame.alt)
        time.sleep(1)

    print("Коптер приземлился и разармлен.")

except Exception as e:
    print("Ошибка: %s" % e)

finally:
    cap.release()
    cv2.destroyAllWindows()
    vehicle.close()
