#!/usr/bin/env python3
"""Face tracking via OpenCV on MJPEG stream from ESP32, drives servo to follow."""

import sys
import os
import random
import subprocess
import cv2
import requests

ESP_IP = sys.argv[1] if len(sys.argv) > 1 else "192.168.1.1"
STREAM_URL = f"http://{ESP_IP}:81/stream"
SERVO_URL = f"http://{ESP_IP}/set"

current_angle = 90

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

AUDIO_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "audio")
AUDIO_FILES = [os.path.join(AUDIO_DIR, f) for f in os.listdir(AUDIO_DIR) if f.endswith(".mp3")]

audio_proc = None


def play_random_clip():
    global audio_proc
    clip = random.choice(AUDIO_FILES)
    audio_proc = subprocess.Popen(["afplay", clip], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def is_audio_playing():
    global audio_proc
    if audio_proc is None:
        return False
    if audio_proc.poll() is None:
        return True
    audio_proc = None
    return False


def stop_audio():
    global audio_proc
    if audio_proc and audio_proc.poll() is None:
        audio_proc.terminate()
        audio_proc = None


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


def track_face(x, y, w, h, frame_w):
    face_cx = x + w / 2
    normalized = face_cx / frame_w
    angle = int(normalized * 180)
    set_servo(angle)


def main():
    print(f"Connecting to stream: {STREAM_URL}")
    cap = cv2.VideoCapture(STREAM_URL)
    if not cap.isOpened():
        print(f"Failed to open stream. Is the ESP32 running at {ESP_IP}?")
        sys.exit(1)

    print("Stream connected. Press 'q' to quit.")

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
            if not is_audio_playing():
                play_random_clip()
            status = f"Face detected | Servo: {current_angle}"
        else:
            stop_audio()
            status = f"No face | Servo: {current_angle}"

        cv2.putText(frame, status, (10, 25),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
        cv2.imshow("Face Tracker", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    stop_audio()
    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
