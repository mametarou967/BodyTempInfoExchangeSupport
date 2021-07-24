#include <M5Core2.h>

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 13, 14);
  showTempl();
}

void loop() {
  if (Serial2.available()) {
    displayResults(Serial2.readString());
  }

  TouchPoint_t pos = M5.Touch.getPressPoint();
  char btn = btnPressed(pos);
  if (btn != 0) {
    showTempl();
    if (btn == 'A') {
      M5.Lcd.println("Send Message.");
      Serial2.println("AT$SF=33322E35");
    } else if (btn == 'B') {
      M5.Lcd.println("Send Message with Ack.");
      Serial2.println("AT$SF=C0FFEE,1");        
    } else if (btn == 'C') {
      M5.Lcd.println("Device ID&PAC: ");
      Serial2.println("AT$I=10");
      Serial2.println("AT$I=11");    
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

void showTempl() 
{
 M5.Lcd.clear(BLACK);
 M5.Lcd.setTextColor(WHITE);
 M5.Lcd.setTextSize(2);
 M5.Lcd.setCursor(10, 10);
 M5.Lcd.println("M5Stack Core2 COM.Sigfox");
 M5.Lcd.setCursor(30, 220);
 M5.Lcd.print("UP");
 M5.Lcd.setCursor(129, 220);
 M5.Lcd.print("DOWN");
 M5.Lcd.setCursor(238, 220);
 M5.Lcd.print("ID/PAC");
 M5.Lcd.setTextColor(BLUE);
 M5.Lcd.setCursor(0, 35);
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
