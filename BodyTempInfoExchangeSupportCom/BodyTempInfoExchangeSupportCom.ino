#include <M5Core2.h>
#include "BodyTempInfoExchangeSupportCom.h"

#define QR_DISP_MODE  0
#define QR_HIDE_MODE  1

int dispMode = QR_DISP_MODE;

void DispQrCode(int mode){
  const int qrX = 135;
  const int qrY = 70;
  const int qrSize = 160;
  
  if(mode == QR_DISP_MODE){
    M5.Lcd.fillRect(qrX, qrY, qrSize, qrSize, TFT_BLACK);
    M5.Lcd.qrcode(YOUR_WEBSITE_URI,qrX,qrY,qrSize,6);
  }else{
    M5.Lcd.fillRect(qrX, qrY, qrSize, qrSize, TFT_BLACK);
    M5.Lcd.drawRect(qrX, qrY, qrSize, qrSize, WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(qrX + 35, qrY + (qrSize / 2) - 20);
    M5.Lcd.print("QR code");
    M5.Lcd.setCursor(qrX + 45, qrY + (qrSize / 2));
    M5.Lcd.print("Hidden");
  }

  dispMode = mode;
}

void ConvertDispQrCode(){
  if(dispMode == QR_DISP_MODE){
     DispQrCode(QR_HIDE_MODE);
  }else{
    DispQrCode(QR_DISP_MODE);
  }
}

void SendValue(float value)
{
  const int pointChar = 0x2E;
  char sendtext[16] = {0};
  // M5.Lcd.println("Send Message.");
  // Serial2.println("AT$SF=33322E35");
  int intValue = (int)value;
  int keta1 = (intValue / 10) + 0x30;
  int keta2 = (intValue % 10) + 0x30;
  int keta3 = (int)((value - intValue) * 10) + 0x30;

  sprintf(sendtext,"AT$SF=%X%X%X%X",keta1,keta2,pointChar,keta3);
  Serial.print("Send Message.\n");
  Serial.print(sendtext);
  Serial.print("\n");
  // M5.Lcd.println("Send Message.");
  // Serial2.println("AT$SF=33322E35");
  
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(124,12);
  M5.Lcd.print(value, 2);
  M5.Lcd.print("C");
}

void InitShow(){
 M5.Lcd.clear(BLACK);
 M5.Lcd.setTextColor(WHITE);

  // upper text
  M5.Lcd.drawRect(2, 2, 316, 60, WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10,20);
  M5.Lcd.print("Latest Sent Value:");

  // A button text
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawRect(6, 218, 110, 20, WHITE);
  M5.Lcd.setCursor(20, 220);
  M5.Lcd.print("QR DISP");

  DispQrCode(QR_HIDE_MODE);
}

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 13, 14);
  // showTempl();
  InitShow();
}

void loop() {
  if (Serial2.available()) {
    displayResults(Serial2.readString());
  }

  TouchPoint_t pos = M5.Touch.getPressPoint();
  char btn = btnPressed(pos);
  if (btn != 0) {
    if (btn == 'A') {
      ConvertDispQrCode();
    }else if(btn == 'B') {
      int test1 = 12;
      int test2 = 34;
      float sendValue = (float)test1 + ((float)test2 * 0.01);
      SendValue(sendValue);
    }
    delay(1000);
  }
}

char btnPressed(TouchPoint_t pos) 
{
  char btn = 0;
  if (pos.y > 240) {
    if (pos.x < 109){
      btn = 'A';
    }else if(pos.x >= 109 && pos.x < 218){
      btn = 'B'; 
    }else if(pos.x > 218){
      btn = 'C'; 
    }
 } 
 return btn; 
} 

void displayResults(String ack)
{
 M5.Lcd.println(ack);
 int i = ack.indexOf("RX=");
 if (i >= 0)
 {
 ack.replace(" ", "");
 String bs = ack.substring(i + 6, i + 11);
 String rs = ack.substring(i + 15);
 signed int rssi = (int16_t)(strtol(rs.c_str(), NULL, 16));
 M5.Lcd.print("BSID: ");
 M5.Lcd.println(bs);
 M5.Lcd.print("RSSI: ");
 M5.Lcd.println(rssi);
 }
}
