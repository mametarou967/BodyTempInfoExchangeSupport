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

void InitShow(){
 M5.Lcd.clear(BLACK);
 M5.Lcd.setTextColor(WHITE);
 M5.Lcd.setTextSize(2);
 M5.Lcd.drawRect(6, 218, 110, 20, WHITE);
 M5.Lcd.setCursor(20, 220);
 M5.Lcd.print("QR DISP");
}

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 13, 14);
  // showTempl();
  InitShow();
  DispQrCode(QR_DISP_MODE);
}

void loop() {
  if (Serial2.available()) {
    displayResults(Serial2.readString());
  }

  TouchPoint_t pos = M5.Touch.getPressPoint();
  char btn = btnPressed(pos);
  if (btn != 0) {
    // showTempl();
    if (btn == 'A') {
      ConvertDispQrCode();
      // M5.Lcd.println("Send Message.");
      // Serial2.println("AT$SF=33322E35");
    }
    // } else if (btn == 'B') {
      // M5.Lcd.println("Send Message with Ack.");
      // Serial2.println("AT$SF=C0FFEE,1");        
    //} else if (btn == 'C') {
      // Serial.print("C pushed");
      // M5.Lcd.println("Device ID&PAC: ");
      // Serial2.println("AT$I=10");
      // Serial2.println("AT$I=11");    
    // }
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

/*
void showTempl() 
{
 M5.Lcd.clear(BLACK);
 M5.Lcd.setTextColor(WHITE);
 M5.Lcd.setTextSize(2);
 M5.Lcd.setCursor(10, 10);
 M5.Lcd.println("M5Stack Core2 COM.Sigfox");
 M5.Lcd.setTextColor(BLUE);
 M5.Lcd.setCursor(0, 35);
 }
 */

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
