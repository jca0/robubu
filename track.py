#!/usr/bin/env python3
"""Face tracking via OpenCV on MJPEG stream from ESP32, drives servo to follow."""

import sys
import time
import cv2
import requests

ESP_IP = sys.argv[1] if len(sys.argv) > 1 else "192.168.1.1"
STREAM_URL = f"http://{ESP_IP}:81/stream"
SERVO_URL = f"http://{ESP_IP}/set"
VOICE_URL = f"http://{ESP_IP}/voice"

current_angle = 90
last_voice_time = 0
VOICE_INTERVAL = 2.0
DEADZONE = 0.05
GAIN = 0.3

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")


def set_servo(angle):
    global current_angle
    angle = max(0, min(180, int(angle)))
    if angle == current_angle:
        return
    current_angle = angle
    try:
        requests.get(f"{SERVO_URL}?angle={angle}", timeout=0.1)
    except requests.RequestException:
        pass


def play_voice():
    global last_voice_time
    now = time.time()
    if now - last_voice_time < VOICE_INTERVAL:
        return
    last_voice_time = now
    try:
        requests.get(VOICE_URL, timeout=0.1)
    except requests.RequestException:
        pass


def track_face(x, y, w, h, frame_w):
    global current_angle
    face_cx = x + w / 2
    frame_cx = frame_w / 2
    error = (face_cx - frame_cx) / frame_w

    if abs(error) < DEADZONE:
        return

    new_angle = current_angle - error * GAIN * 180
    set_servo(new_angle)


def main():
    print(f"Connecting to stream: {STREAM_URL}")
    cap = cv2.VideoCapture(STREAM_URL)
    if not cap.isOpened():
        print(f"Failed to open stream. Is the ESP32 running at {ESP_IP}?")
        sys.exit(1)

    print("Stream connected. Press Ctrl+C to quit.")

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                continue

            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

            for (x, y, w, h) in faces:
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

            if len(faces) > 0:
                biggest = max(faces, key=lambda f: f[2] * f[3])
                x, y, w, h = biggest
                track_face(x, y, w, h, frame.shape[1])
                play_voice()
                status = f"Face detected | Servo: {current_angle}"
            else:
                status = f"No face | Servo: {current_angle}"

            cv2.putText(frame, status, (10, 25),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
            cv2.imshow("Face Tracker", frame)
            cv2.waitKey(1)
    except KeyboardInterrupt:
        pass

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
