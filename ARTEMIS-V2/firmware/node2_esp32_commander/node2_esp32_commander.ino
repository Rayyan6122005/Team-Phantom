/*
 * A.R.T.E.M.I.S. v2.0 — ESP32 Radar (SYNCED TO ARDUINO + OFF-SCREEN CANVAS)
 * 1. Reads real ultrasonic data from Arduino via TX(Pin1) -> ESP32 RX(GPIO16)
 * 2. Uses off-screen Canvas to eliminate all tearing, flickering, and artifacts
 *
 * TFT Pins: MOSI=13, SCLK=14, CS=15, DC=2, RST=4
 * Touch: GPIO27
 */
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_SCLK  14
#define TFT_MOSI  13
#define TFT_CS    15
#define TFT_DC     2
#define TFT_RST    4
#define TOUCH_PIN  T7

SPIClass hspi(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&hspi, TFT_CS, TFT_DC, TFT_RST);

// 🔥 The Ultimate Polish: 16-bit Canvas buffer (40KB RAM)
// This perfectly renders the entire screen in memory first, then blasts it to the TFT in one go.
// Zero flickering, perfect alignment, completely stable.
GFXcanvas16 canvas(160, 128);

#define W    160
#define H    128
#define CX    63   
#define CY   124   
#define MXR  108   
#define SBX  126   // Sidebar X perfectly aligned

// Colours
uint16_t cGD,cGM,cSWH,cSAFE,cWRN,cCRT,cCYN,cAMB,cRED,cSBBG,cSBLN,cBK;

// Telemetry from Arduino
float sA = 0;
float sDs = 999;
bool  hwLocked = false;
float hwLockTemp = 24.5;

// Touch state
bool tAlr = false;
unsigned long taEnd = 0;

// Trail & Blips
#define TLN 8
float trA[TLN]; bool trOk[TLN];
struct Blip{float a,d;uint8_t age,thr;};
#define MB 16
Blip bl[MB]; uint8_t bH=0;

// Helper Math
void p2xy(float deg,float r,int&x,int&y){
  float rad=deg*DEG_TO_RAD;
  x=CX-(int)(r*cosf(rad)); y=CY-(int)(r*sinf(rad));
}
float d2r(float d){if(d>=990)return 0;return min((d/200.f)*MXR,(float)MXR);}

// ------------------------------------------------------------------
// ARDUINO SYNC (Reads Serial2)
// ------------------------------------------------------------------
void parseSerial() {
  static String buf = "";
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      buf.trim();
      if (buf.startsWith("<") && buf.endsWith(">")) {
        buf = buf.substring(1, buf.length() - 1);
        int colon = buf.indexOf(':');
        if (colon > 0) {
          String type = buf.substring(0, colon);
          String data = buf.substring(colon + 1);
          int comma = data.indexOf(',');
          if (comma > 0) {
            float ang = data.substring(0, comma).toFloat();
            float dst = data.substring(comma + 1).toFloat();
            
            sA = ang;
            sDs = dst;
            
            if (type == "LOCK") {
              hwLocked = true;
              hwLockTemp = 38.0 + random(0, 40) / 10.f; // Fake temp spike on real lock
              if (millis() % 200 < 50) {
                 bl[bH%MB]={sA,sDs,0,2}; bH++;
              }
              
              // 🔥 TRIGGER ESP32-CAM (User's original code needs this!)
              // Throttled to 1 pic per 3s to prevent SD card crashing
              static unsigned long lastCamTrigger = 0;
              if (millis() - lastCamTrigger > 3000) {
                lastCamTrigger = millis();
                Serial.println("<LOG:THREAT_DETECTED>");
              }
            } else if (type == "SCAN") {
              hwLocked = false;
              for(int i=TLN-1;i>0;i--){trA[i]=trA[i-1];trOk[i]=trOk[i-1];}
              trA[0]=sA;trOk[0]=true;
              
              uint8_t thr = (sDs<20)?2:(sDs<40)?1:0;
              bl[bH%MB]={sA,sDs,0,thr}; bH++;
            }
          }
        }
      }
      buf = "";
    } else {
      buf += c;
    }
  }
}

// ------------------------------------------------------------------
// TOUCH DEBOUNCE
// ------------------------------------------------------------------
void chkTouch() {
  static unsigned long lastT = 0;
  if (millis() - lastT > 50) { // Debounce timer
    lastT = millis();
    int tVal = touchRead(TOUCH_PIN);
    // Values < 40 and > 0 indicate a firm touch
    if (tVal < 40 && tVal > 0) { 
      tAlr = true;
      taEnd = millis() + 3000;
    }
  }
  if (tAlr && millis() > taEnd) {
    tAlr = false;
  }
}

// ------------------------------------------------------------------
// FLAWLESS CANVAS RENDER
// ------------------------------------------------------------------
void drawFrame() {
  canvas.fillScreen(cBK); // Clear memory buffer
  
  // 1. Grid
  const int rR[]={27,54,81,MXR};
  for(int ri=0;ri<4;ri++){
    uint16_t c=(ri==3)?cGM:cGD;
    for(int a=0;a<=178;a+=4){
      int x1,y1,x2,y2; p2xy(a,rR[ri],x1,y1); p2xy(a+4,rR[ri],x2,y2);
      canvas.drawLine(x1,y1,x2,y2,c);
    }
  }
  for(int a=0;a<=180;a+=30){int ex,ey;p2xy(a,MXR,ex,ey);canvas.drawLine(CX,CY,ex,ey,cGD);}
  canvas.drawFastHLine(CX-MXR,CY,MXR*2,cGM);
  canvas.fillCircle(CX,CY,2,tft.color565(0,180,70));

  // 2. Trail
  uint16_t tc[TLN]={
    tft.color565(45,210,55),tft.color565(25,155,35),tft.color565(12,95,18),
    tft.color565(6,58,10),  tft.color565(3,34,5),   tft.color565(2,18,3),
    tft.color565(1,10,2),   tft.color565(0,6,1)
  };
  for(int i=TLN-1;i>=0;i--){
    if(!trOk[i])continue;
    int ex,ey;p2xy(trA[i],MXR,ex,ey);
    canvas.drawLine(CX,CY,ex,ey,tc[i]);
  }
  int hx,hy;p2xy(sA,MXR,hx,hy);
  canvas.drawLine(CX,CY,hx,hy,cSWH);
  canvas.drawLine(CX+1,CY,hx+1,hy,tft.color565(20,160,30));

  // 3. Blips
  for(int i=0;i<MB;i++){
    Blip&b=bl[i];if(b.age>180||b.d>=990)continue;
    float r=d2r(b.d);if(r<2)continue;
    int bx,by;p2xy(b.a,r,bx,by);
    uint16_t col=(b.age>120)?tft.color565(0,28,10):(b.thr==2)?cCRT:(b.thr==1)?cWRN:cSAFE;
    canvas.fillCircle(bx,by,(b.thr==2)?3:2,col);
    b.age++;
  }

  // 4. Target crosshair (hardware lock or alert)
  bool al = hwLocked || tAlr;
  float td = sDs;
  if(td<990){
    float tr=d2r(td);if(tr>=2){
      int tx,ty;p2xy(sA,tr,tx,ty);
      bool bk=(millis()/300)%2;
      if(al){
        if(bk){canvas.drawLine(tx-8,ty,tx+8,ty,cCRT);canvas.drawLine(tx,ty-8,tx,ty+8,cCRT);canvas.drawCircle(tx,ty,6,cCRT);canvas.drawCircle(tx,ty,3,cAMB);}
        canvas.drawLine(tx,ty-7,tx+7,ty,cCRT);canvas.drawLine(tx+7,ty,tx,ty+7,cCRT);
        canvas.drawLine(tx,ty+7,tx-7,ty,cCRT);canvas.drawLine(tx-7,ty,tx,ty-7,cCRT);
      }else if(td<40){canvas.fillCircle(tx,ty,3,cWRN);if(bk)canvas.drawCircle(tx,ty,6,cWRN);}
      else canvas.fillCircle(tx,ty,2,cSAFE);
    }
  }

  // 5. Sidebar layout (Perfectly aligned text)
  canvas.fillRect(SBX,0,W-SBX,H,cSBBG);
  canvas.drawFastVLine(SBX,0,H,cSBLN);
  int sx = SBX + 3; // Sidebar inner margin
  
  canvas.setTextSize(1);
  if(al&&(millis()/280)%2){
    canvas.fillRect(SBX+1,0,33,11,cRED);
    canvas.setTextColor(ST77XX_WHITE); canvas.setCursor(sx+2,2); canvas.print("ALRT");
  }else{
    canvas.fillRect(SBX+1,0,33,11,cSBBG);
    canvas.setTextColor(tft.color565(0,170,60)); canvas.setCursor(sx+2,2); canvas.print(hwLocked?"LOCK":"SCAN");
  }
  canvas.drawFastHLine(SBX,12,34,cSBLN);
  
  uint16_t dc=(sDs<20)?cCRT:(sDs<40)?cWRN:tft.color565(0,190,90);
  canvas.setTextColor(tft.color565(90,90,90)); canvas.setCursor(sx,15); canvas.print("DIST");
  canvas.setTextColor(dc);
  if(sDs>=990){
    canvas.setCursor(sx+2,26); canvas.print("---");
  } else {
    canvas.setTextSize(2); canvas.setCursor(sx-1,24); canvas.print((int)sDs); canvas.setTextSize(1);
  }
  canvas.setTextColor(tft.color565(60,60,60)); canvas.setCursor(sx+16,42); canvas.print("cm");
  canvas.drawFastHLine(SBX,51,34,cGD);
  
  canvas.setTextColor(tft.color565(90,90,90)); canvas.setCursor(sx,54); canvas.print("ANG");
  canvas.setTextSize(2); canvas.setTextColor(cCYN); canvas.setCursor(sx-1,63); canvas.print((int)sA); canvas.setTextSize(1);
  canvas.setTextColor(tft.color565(60,60,60)); canvas.setCursor(sx+12,81); canvas.print("deg");
  canvas.drawFastHLine(SBX,90,34,cGD);
  
  float dispTemp = hwLocked ? hwLockTemp : (tAlr ? (43.0+random(0,20)/10.f) : 24.5);
  uint16_t tcol=(dispTemp>37)?cCRT:(dispTemp>29)?cWRN:tft.color565(0,180,220);
  canvas.setTextColor(tft.color565(90,90,90)); canvas.setCursor(sx,93); canvas.print("TEMP");
  canvas.setTextColor(tcol); canvas.setCursor(sx-1,103); canvas.print(dispTemp,1);
  canvas.setTextColor(tft.color565(60,60,60)); canvas.setCursor(sx+20,113); canvas.print("C");
  canvas.drawFastHLine(SBX,121,34,cGD);
  
  uint16_t zc=al?cCRT:(sDs<40)?cWRN:tft.color565(0,180,60);
  canvas.fillRect(SBX+1,122,33,6,cSBBG);
  canvas.setTextColor(zc); canvas.setCursor(sx,122); canvas.print(al?"CRIT":(sDs<40)?"WARN":"SAFE");

  // 6. Threat Banner over Radar (Only flashes)
  if(al && (millis()/220)%2){
    canvas.fillRect(0,0,SBX,13,cRED);
    canvas.setTextColor(ST77XX_WHITE);
    canvas.setCursor(tAlr?4:2,3);
    canvas.print(tAlr?"! TOUCH ALERT !":"!! TARGET LOCKED !!");
  }

  // 💥 BLAST CANVAS TO TFT SCREEN
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), W, H);
}

// ------------------------------------------------------------------
void setup(){
  // Debug Serial
  Serial.begin(115200);
  
  // ARDUINO SYNC SERIAL (RX = GPIO 16, TX = GPIO 17)
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  
  hspi.begin(TFT_SCLK,-1,TFT_MOSI,TFT_CS);
  tft.initR(INITR_BLACKTAB);
  delay(100);
  tft.setRotation(1);

  cBK=0x0000;
  cGD=tft.color565(0,28,8);   cGM=tft.color565(0,55,18);
  cSWH=tft.color565(60,255,80);cSAFE=tft.color565(0,255,120);
  cWRN=tft.color565(255,200,0);cCRT=tft.color565(255,40,0);
  cCYN=tft.color565(0,210,255);cAMB=tft.color565(255,160,0);
  cRED=tft.color565(220,0,0);  cSBBG=tft.color565(0,6,2);
  cSBLN=tft.color565(0,70,30);

  for(int i=0;i<TLN;i++)trOk[i]=false;
  for(int i=0;i<MB;i++)bl[i].age=255;
  
  // Splash Screen via Canvas
  canvas.fillScreen(cBK);
  canvas.setTextSize(2); canvas.setTextColor(cSAFE); canvas.setCursor(8,16); canvas.print("A.R.T.E.M.I.S");
  canvas.setTextSize(1); canvas.setTextColor(tft.color565(0,140,60)); canvas.setCursor(18,36); canvas.print("SENTRY SYSTEM");
  canvas.setTextColor(tft.color565(60,60,60)); canvas.setCursor(35,48); canvas.print("SYNCING...");
  tft.drawRGBBitmap(0,0,canvas.getBuffer(),W,H);
  delay(1000);
}

void loop(){
  parseSerial();
  chkTouch();
  drawFrame();
}
