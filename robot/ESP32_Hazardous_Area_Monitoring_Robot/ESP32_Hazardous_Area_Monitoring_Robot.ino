#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <DHT.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
String botToken = "YOUR_BOT_TOKEN";
String chat_id  = "YOUR_CHAT_ID";

TinyGPSPlus gps;
HardwareSerial SerialGPS(2);
HardwareSerial SerialCam(1);
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int mq2Pin = 36;
const int trigPin = 33;
const int echoPin = 32;
const int buzzerPin = 23;
const int in1 = 5;
const int in2 = 18;
const int in3 = 19;
const int in4 = 22;
const int motorSpeed = 100;

struct SensorData {
  float gas;
  float temp;
  float humidity;
  float distance;
};

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  SerialCam.begin(9600, SERIAL_8N1, -1, 14);
  delay(1000);
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  stopMotors();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  goForward();
}

void loop() {
  while (SerialGPS.available()) gps.encode(SerialGPS.read());
  SensorData data = readSensors();
  printSensors(data);
  if (dangerDetected(data)) {
    handleDanger();
  } else {
    goForward();
  }
  delay(200);
}

SensorData readSensors() {
  SensorData d;
  d.gas = analogRead(mq2Pin);
  d.temp = dht.readTemperature();
  d.humidity = dht.readHumidity();
  d.distance = getUltrasonic();
  return d;
}

void printSensors(const SensorData& d) {
  Serial.print("Gas: "); Serial.print(d.gas);
  Serial.print(" | Temp: "); Serial.print(d.temp);
  Serial.print(" | Humidity: "); Serial.print(d.humidity);
  Serial.print(" | Distance: "); Serial.println(d.distance);
  if (gps.location.isValid()) {
    Serial.print("Latitude: "); Serial.print(gps.location.lat(), 6);
    Serial.print(" | Longitude: "); Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Waiting for GPS fix...");
  }
}

bool dangerDetected(const SensorData& d) {
  return (d.gas > 1500 || d.distance < 15 || d.temp > 40);
}

void handleDanger() {
  stopMotors();
  Serial.println("⚠️ Danger detected! Stopping + Sending location and photo");
  float lat = gps.location.isValid() ? gps.location.lat() : 0.0;
  float lon = gps.location.isValid() ? gps.location.lng() : 0.0;
  sendLocationToTelegram(lat, lon);
  SerialCam.println("snap");
  digitalWrite(buzzerPin, HIGH);
  delay(500);
  digitalWrite(buzzerPin, LOW);
  goBackward();
  delay(1000);
  stopMotors();
  float rightDist = scanDirection(turnRight, 500);
  float leftDist = scanDirection(turnLeft, 1000);
  turnCenter();
  decideDirection(rightDist, leftDist);
}

float scanDirection(void (*turnFunc)(), int turnDelay) {
  turnFunc();
  delay(turnDelay);
  float dist = getUltrasonic();
  stopMotors();
  delay(200);
  return dist;
}

void decideDirection(float rightDist, float leftDist) {
  if (rightDist > 15 && rightDist >= leftDist) {
    turnRight(); delay(500); goForward();
  } else if (leftDist > 15 && leftDist > rightDist) {
    turnLeft(); delay(1000); goForward();
  } else {
    stopMotors();
    Serial.println("No safe path. Stopping.");
    while (1) { stopMotors(); delay(1000); }
  }
}

void turnCenter() {
  // Return to center from left (assuming left scan is always after right)
  turnRight(); delay(500);
  stopMotors(); delay(200);
}

float getUltrasonic() {
  stopMotors(); delay(100);
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void sendLocationToTelegram(float lat, float lon) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot"  + botToken +
      "/sendLocation?chat_id=" + chat_id +
      "&latitude=" + String(lat, 6) +
      "&longitude=" + String(lon, 6);
    http.begin(url);
    http.GET();
    http.end();
  }
}

void goForward() {
  analogWrite(in1, 0);
  analogWrite(in2, motorSpeed);
  analogWrite(in3, 0);
  analogWrite(in4, motorSpeed);
}
void goBackward() {
  analogWrite(in1, motorSpeed);
  analogWrite(in2, 0);
  analogWrite(in3, motorSpeed);
  analogWrite(in4, 0);
}
void stopMotors() {
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);
}
void turnRight() {
  analogWrite(in1, 0);
  analogWrite(in2, motorSpeed);
  analogWrite(in3, motorSpeed);
  analogWrite(in4, 0);
}
void turnLeft() {
  analogWrite(in1, motorSpeed);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, motorSpeed);
}هاد في
