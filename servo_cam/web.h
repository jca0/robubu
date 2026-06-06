#pragma once
#include <WebServer.h>
#include "servo.h"
#include "audio_data.h"

WebServer server(80);

const char* html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Servo Cam</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 20px; background: #111; color: #fff; }
    img { width: 100%; max-width: 640px; border-radius: 8px; }
    input[type=range] { width: 80%; max-width: 400px; margin-top: 16px; }
    #angle { font-size: 1.5em; margin-top: 8px; }
    #voiceBtn { font-size: 1.2em; padding: 12px 32px; margin-top: 16px; border: none;
      border-radius: 8px; background: #e91e63; color: #fff; cursor: pointer; }
    #voiceBtn:active { background: #c2185b; }
  </style>
</head>
<body>
  <h1>Servo Cam</h1>
  <img id="stream">
  <p id="angle">90&deg;</p>
  <input type="range" min="0" max="180" value="90"
    oninput="document.getElementById('angle').innerHTML=this.value+'&deg;';
             fetch('/set?angle='+this.value)">
  <br>
  <button id="voiceBtn" onclick="playVoice()">Play Voice</button>
  <script>
    document.getElementById('stream').src = 'http://' + location.hostname + ':81/stream';
    function playVoice() {
      fetch('/voice').then(r => r.blob()).then(b => {
        var url = URL.createObjectURL(b);
        var a = new Audio(url);
        a.play();
        a.onended = function() { URL.revokeObjectURL(url); };
      });
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    setServoAngle(angle);
  }
  server.send(200, "text/plain", "ok");
}

void handleVoice() {
  int idx = random(0, audio_clip_count);
  const AudioClip& clip = audio_clips[idx];
  server.send_P(200, "audio/mpeg", (const char*)clip.data, clip.len);
}

void initServer() {
  randomSeed(analogRead(0));
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/voice", handleVoice);
  server.begin();
  Serial.println("Server started on port 80");
}
