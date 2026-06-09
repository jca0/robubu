#pragma once
#include <WebServer.h>
#include "servo.h"
#include "audio.h"

extern bool manualMode;

WebServer server(80);

const char* html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Labubu</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 20px; background: #111; color: #fff; }
    input[type=range] { width: 80%; max-width: 400px; margin-top: 16px; }
    #angle { font-size: 1.5em; margin-top: 8px; }
    .btn { font-size: 1.1em; padding: 10px 24px; margin: 8px; border: none;
      border-radius: 8px; color: #fff; cursor: pointer; }
    #voiceBtn { background: #e91e63; }
    #voiceBtn:active { background: #c2185b; }
    #modeBtn { background: #4caf50; }
    #modeBtn.manual { background: #ff9800; }
  </style>
</head>
<body>
  <h1>Labubu</h1>
  <p id="angle">90&deg;</p>
  <input type="range" min="0" max="180" value="90" id="slider"
    oninput="document.getElementById('angle').innerHTML=this.value+'&deg;';
             fetch('/set?angle='+this.value)">
  <br>
  <button class="btn" id="modeBtn" onclick="toggleMode()">Mode: Auto</button>
  <button class="btn" id="voiceBtn" onclick="fetch('/voice')">Play Voice</button>
  <script>
    var manual = false;
    function toggleMode() {
      manual = !manual;
      fetch('/mode?manual=' + (manual ? '1' : '0'));
      var btn = document.getElementById('modeBtn');
      btn.textContent = 'Mode: ' + (manual ? 'Manual' : 'Auto');
      btn.className = 'btn' + (manual ? ' manual' : '');
    }
    setInterval(function() {
      fetch('/status').then(r => r.json()).then(d => {
        document.getElementById('slider').value = d.angle;
        document.getElementById('angle').innerHTML = d.angle + '&deg;';
        manual = d.manual;
        var btn = document.getElementById('modeBtn');
        btn.textContent = 'Mode: ' + (manual ? 'Manual' : 'Auto');
        btn.className = 'btn' + (manual ? ' manual' : '');
      });
    }, 500);
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
  playRandomClip();
  server.send(200, "text/plain", "ok");
}

void handleMode() {
  if (server.hasArg("manual")) {
    manualMode = server.arg("manual").toInt();
  }
  server.send(200, "text/plain", "ok");
}

void handleStatus() {
  char json[48];
  snprintf(json, sizeof(json), "{\"angle\":%d,\"manual\":%s}", getServoAngle(), manualMode ? "true" : "false");
  server.send(200, "application/json", json);
}

void initServer() {
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/voice", handleVoice);
  server.on("/mode", handleMode);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("Server started on port 80");
}
