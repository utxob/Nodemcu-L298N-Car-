#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// মোটর পিন ডেফাইন
#define ENA D5  // PWM স্পিড কন্ট্রোল (বাম মোটর)
#define IN1 D1  // বাম মোটর ডাইরেকশন ১
#define IN2 D2  // বাম মোটর ডাইরেকশন ২
#define ENB D6  // PWM স্পিড কন্ট্রোল (ডান মোটর)
#define IN3 D3  // ডান মোটর ডাইরেকশন ১
#define IN4 D4  // ডান মোটর ডাইরেকশন ২

// Wi-Fi সেটিংস
const char* ssid = "SmartCar_4WD";
const char* password = "12345678";

ESP8266WebServer server(80);
int carSpeed = 800; // ডিফল্ট স্পিড (0-1023)

void setup() {
  Serial.begin(115200);
  
  // মোটর পিন সেটআপ
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopCar();

  // Wi-Fi হটস্পট
  WiFi.softAP(ssid, password);
  Serial.println("\nWi-Fi Hotspot Ready!");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  // ওয়েব সার্ভার
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/speed", handleSpeed);
  
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
}

// মোটর কন্ট্রোল ফাংশন
void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // বাম মোটর সামনে
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // ডান মোটর সামনে
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  Serial.println("Forward | Speed: " + String(carSpeed));
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);  // বাম মোটর পিছনে
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);  // ডান মোটর পিছনে
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  Serial.println("Backward | Speed: " + String(carSpeed));
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);  // বাম মোটর পিছনে
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // ডান মোটর সামনে
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  Serial.println("Left Turn | Speed: " + String(carSpeed));
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // বাম মোটর সামনে
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);  // ডান মোটর পিছনে
  analogWrite(ENA, carSpeed); analogWrite(ENB, carSpeed);
  Serial.println("Right Turn | Speed: " + String(carSpeed));
}

void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  Serial.println("Car Stopped");
}

// স্পিড কন্ট্রোল
void handleSpeed() {
  if (server.hasArg("value")) {
    carSpeed = server.arg("value").toInt();
    server.send(200, "text/plain", "Speed: " + String(carSpeed));
    Serial.println("New Speed: " + String(carSpeed));
  }
}

// ওয়েব ইন্টারফেস
void handleRoot() {
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>৪-মোটর স্মার্ট কার</title>
  <style>
    body {font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; background: #f0f0f0;}
    .control-panel {background: white; border-radius: 15px; padding: 20px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); max-width: 400px; margin: 0 auto;}
    h1 {color: #2c3e50; margin-bottom: 20px;}
    .btn {background: #3498db; border: none; color: white; padding: 12px 20px; margin: 8px; border-radius: 8px; font-size: 16px; cursor: pointer; width: 100px; transition: all 0.3s;}
    .btn:active {transform: scale(0.95);}
    .btn-stop {background: #e74c3c;}
    .speed-control {margin: 20px 0;}
    .speed-slider {width: 100%; margin: 10px 0;}
    .speed-value {font-size: 18px; font-weight: bold; color: #2c3e50;}
  </style>
</head>
<body>
  <div class="control-panel">
    <h1>স্মার্ট কার কন্ট্রোল</h1>
    
    <div class="speed-control">
      <h3>গতি কন্ট্রোল</h3>
      <input type="range" min="300" max="1023" value="800" class="speed-slider" id="speedSlider" oninput="updateSpeed(this.value)">
      <div class="speed-value">গতি: <span id="speedValue">800</span></div>
    </div>
    
    <div>
      <button class="btn" ontouchstart="moveForward()" ontouchend="stop()" onmousedown="moveForward()" onmouseup="stop()">সামনে</button>
    </div>
    <div>
      <button class="btn" ontouchstart="turnLeft()" ontouchend="stop()" onmousedown="turnLeft()" onmouseup="stop()">বামে</button>
      <button class="btn btn-stop" onclick="stop()">থামুন</button>
      <button class="btn" ontouchstart="turnRight()" ontouchend="stop()" onmousedown="turnRight()" onmouseup="stop()">ডানে</button>
    </div>
    <div>
      <button class="btn" ontouchstart="moveBackward()" ontouchend="stop()" onmousedown="moveBackward()" onmouseup="stop()">পিছনে</button>
    </div>
  </div>

  <script>
    function moveForward() { fetch("/forward"); }
    function moveBackward() { fetch("/backward"); }
    function turnLeft() { fetch("/left"); }
    function turnRight() { fetch("/right"); }
    function stop() { fetch("/stop"); }
    function updateSpeed(val) {
      document.getElementById("speedValue").innerText = val;
      fetch("/speed?value=" + val);
    }
  </script>
</body>
</html>
)=====";
  server.send(200, "text/html", html);
}

// অন্যান্য হ্যান্ডলার
void handleForward() { moveForward(); server.send(200, "text/plain", "Forward"); }
void handleBackward() { moveBackward(); server.send(200, "text/plain", "Backward"); }
void handleLeft() { turnLeft(); server.send(200, "text/plain", "Left"); }
void handleRight() { turnRight(); server.send(200, "text/plain", "Right"); }
void handleStop() { stopCar(); server.send(200, "text/plain", "Stop"); }
