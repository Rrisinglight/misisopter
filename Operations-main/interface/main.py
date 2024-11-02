from gpiozero import OutputDevice
from flask import Flask, render_template, request, jsonify
import time
import random

app = Flask(__name__)

PIN_LEFT = 22   # Move left
PIN_RIGHT = 15  # Move right
PIN_SCAN = 13   # Initialize auto-scanning

left = OutputDevice(PIN_LEFT)
right = OutputDevice(PIN_RIGHT)
scan = OutputDevice(PIN_SCAN)

def read_compass():
    # For testing, return a random value
    return random.uniform(0, 360)

def read_gps():
    # For testing, return random coordinates within a specific range
    latitude = random.uniform(55.7, 55.8)    # Approximate latitude range for Moscow
    longitude = random.uniform(37.6, 37.7)   # Approximate longitude range for Moscow
    return latitude, longitude


def read_antenna_direction():
    # For testing, return a random value
    return random.uniform(0, 360)

def activate_pin(device, duration=1):
    device.on()
    time.sleep(duration)
    device.off()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def data():
    latitude, longitude = read_gps()
    heading = read_compass()
    antenna_direction = read_antenna_direction()
    return jsonify({
        'latitude': latitude,
        'longitude': longitude,
        'heading': heading,
        'antenna_direction': antenna_direction
    })

@app.route('/left', methods=['POST'])
def move_left():
    activate_pin(left)
    return jsonify({'status': 'success'})

@app.route('/right', methods=['POST'])
def move_right():
    activate_pin(right)
    return jsonify({'status': 'success'})

@app.route('/scan', methods=['POST'])
def initialize_scan():
    activate_pin(scan)
    return jsonify({'status': 'success'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
