import time
import cv2
import numpy as np
import cv2.aruco as aruco
from dronekit import connect, VehicleMode
from pymavlink import mavutil
import time

# Параметры подключения к Pixhawk
connection_string = '/dev/ttyS0'  # Замените на ваш порт
baud_rate = 57600

# Подключение к коптеру
print("Подключение к коптеру на: %s" % connection_string)
vehicle = connect(connection_string, baud=baud_rate, wait_ready=True)

def arm_and_disarm():
    print("Arming motors...")
    
    # Проверяем, не армированы ли уже двигатели
    while not vehicle.is_armable:
        print("Waiting for vehicle to become armable...")
        time.sleep(1)

    # Устанавливаем режим в GUIDED и армируем дрон
    vehicle.mode = VehicleMode("GUIDED")
    vehicle.armed = True

    # Ждем, пока дрон будет армирован
    while not vehicle.armed:
        print("Waiting for arming...")
        time.sleep(1)
    
    print("Vehicle is armed!")

    # Держим дрон армированным 2 секунды
    time.sleep(2)

    # Разармируем
    print("Disarming motors...")
    vehicle.armed = False

    # Ждем, пока дрон будет разармирован
    while vehicle.armed:
        print("Waiting for disarming...")
        time.sleep(1)

    print("Vehicle is disarmed!")

# Выполняем армирование и разармирование
arm_and_disarm()

# Закрываем соединение
vehicle.close()