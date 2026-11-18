//كود رئيسي للكاميرا
#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

// WiFi & Telegram
const char* ssid = "YOUR_WIFI_NAME";

const char* password = "YOUR_WIFI_PASSWORD";

// Telegram

String botToken = "YOUR_BOT_TOKEN";

String chat_id  = "YOUR_CHAT_ID";

// UART للاتصال بـ ESP32-WROOM
#define RX_PIN 3
#define TX_PIN 1

HardwareSerial SerialComm(0);  // UART0 (GPIO3 RX, GPIO1 TX)

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  esp_camera_init(&config);
}

void sendPhotoToTelegram() {
  Serial.println("Capturing image...");
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect("api.telegram.org", 443)) {
    String head = "--boundary\r\nContent-Disposition: form-data; name=\"chat_id\"\r\n\r\n" + chat_id;
    head += "\r\n--boundary\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"photo.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--boundary--\r\n";

    String request = String("POST /bot") + botToken + "/sendPhoto HTTP/1.1\r\n";
    request += "Host: api.telegram.org\r\n";
    request += "Content-Type: multipart/form-data; boundary=boundary\r\n";
    request += "Content-Length: " + String(head.length() + fb->len + tail.length()) + "\r\n\r\n";

    client.print(request);
    client.print(head);
    client.write(fb->buf, fb->len);
    client.print(tail);

    Serial.println("Photo sent to Telegram!");
  }

  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200); // فقط للمراقبة
  SerialComm.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // الاتصال بـ ESP32-WROOM

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  startCamera();
}

void loop() {
  if (SerialComm.available()) {
    String cmd = SerialComm.readStringUntil('\n');
    cmd.trim();
    if (cmd == "snap") {
      Serial.println("Snap command received.");
      sendPhotoToTelegram();
    }
  }
}
