#include "esp_camera.h"
#include <WiFi.h>
#include "FS.h"
#include "SD_MMC.h"
#include "time.h"

// ===========================
// Select camera model
// ===========================
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h" 

// ===========================
// WiFi & Time Credentials
// ===========================
const char *ssid = "Realme C35";
const char *password = "MOHAMMEDMAAZ";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // IST (GMT+5:30)
const int   daylightOffset_sec = 0;

void startCameraServer();
void setupLedFlash();

void setup() {
  // Listen to the ESP-32 Tactical Brain on the default hardware RX pin
  Serial.begin(115200);
  Serial.setDebugOutput(false); 
  Serial.println("\n🚀 INIT: V.A.L.K.Y.R.I.E. VISION & BLACK BOX...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; 
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // 1. INIT CAMERA
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed with error 0x%x\n", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_VGA); // Stream resolution

  // 2. INIT SD CARD
  if (!SD_MMC.begin()) {
    Serial.println("❌ SD Card Mount Failed! Black Box Offline.");
  } else {
    Serial.println("✅ SD Card Mounted. Black Box Online.");
  }

  // 3. WIFI CONNECT
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("📡 WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // 4. SYNC WORLD TIME
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("✅ Tactical Clock Synced");

  startCameraServer();
  Serial.print("🟢 Camera Ready! Access stream at: http://");
  Serial.println(WiFi.localIP());
}

String getTimeString() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "UNKNOWN_TIME";
  }
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y%m%d_%H%M%S", &timeinfo);
  return String(timeStringBuff);
}

void loop() {
  // LISTEN FOR TACTICAL BRAIN INTERRUPTS
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    
    // If the ESP-32 Standard triggers a threat logging event
    if (cmd.indexOf("<LOG:THREAT_DETECTED>") >= 0) {
      
      String timestamp = getTimeString();
      String path = "/THREAT_" + timestamp + ".jpg";
      
      // GRAB HIGH-RES FRAME
      camera_fb_t * fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("❌ Black Box Capture Failed");
        return;
      }

      // SAVE TO SD CARD
      fs::FS &fs = SD_MMC;
      File file = fs.open(path.c_str(), FILE_WRITE);
      if(!file){
        Serial.println("❌ Failed to open SD file for writing");
      } else {
        file.write(fb->buf, fb->len);
        Serial.println("📸 THREAT SAVED TO BLACK BOX: " + path);
      }
      
      file.close();
      esp_camera_fb_return(fb); 
      
      // Turn off the Flash LED (SD_MMC uses the Flash LED pin to write data, 
      // so it will flash when taking a picture. This turns it back off.)
      pinMode(4, OUTPUT);
      digitalWrite(4, LOW);
    }
  }
}
