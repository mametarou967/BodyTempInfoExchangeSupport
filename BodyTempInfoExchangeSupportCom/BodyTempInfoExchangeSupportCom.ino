#include <M5Core2.h>
#include "BodyTempInfoExchangeSupportCom.h"
#include <esp_now.h>
#include <WiFi.h>

#define DEBUG 0
// QR DISP MODE
#define QR_DISP_MODE  0
#define QR_HIDE_MODE  1
// ELAPSED TIME MODE
#define ELAPSED_TIME_MODE_CLEAR 0
#define ELAPSED_TIME_MODE_SEC_UPDATE 1

int dispMode = QR_DISP_MODE;
esp_now_peer_info_t slave;
hw_timer_t * timer = NULL;

void elapsedTimeDispUpdate(const char* text){
  M5.Lcd.fillRect(164, 32, 20 * 5, 20, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(164,32);
  M5.Lcd.print(text);
}

void latestSentValueDispUpdate(float value){
  M5.Lcd.fillRect(164, 12, 20 * 5, 20, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(164,12);
  M5.Lcd.print(value, 2);
  M5.Lcd.print("C");
  
}
void elapsedTimeUpdated(int mode){
  static int elapsedTimeSec = 0;
  static int elapsedTimeMin = 0;
  static int elapsedTimeHour = 0;

  char dispText[10] = {0};

  if(mode == ELAPSED_TIME_MODE_CLEAR){
    elapsedTimeSec = 0;
    elapsedTimeMin = 0;
    elapsedTimeHour = 0;
  }else if(mode == ELAPSED_TIME_MODE_SEC_UPDATE){
    elapsedTimeSec = elapsedTimeSec + 1;
    
    if(elapsedTimeSec > 60){
      elapsedTimeSec = 0;
      elapsedTimeMin = elapsedTimeMin + 1;
    }

    if(elapsedTimeMin > 60){
      elapsedTimeMin = 0;
      elapsedTimeHour = elapsedTimeHour + 1;
    }
  }

  sprintf(dispText,"%d:%02d:%02d",elapsedTimeHour,elapsedTimeMin,elapsedTimeSec);
  elapsedTimeDispUpdate(dispText);
}

void IRAM_ATTR onTimer() {
  elapsedTimeUpdated(ELAPSED_TIME_MODE_SEC_UPDATE);
}

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
  int intValue = (int)value;
  int keta1 = (intValue / 10) + 0x30;
  int keta2 = (intValue % 10) + 0x30;
  int keta3 = (int)((value - intValue) * 10) + 0x30;

  sprintf(sendtext,"AT$SF=%X%X%X%X",keta1,keta2,pointChar,keta3);
  if(!DEBUG){
    Serial2.println(sendtext);
  }
}

void InitShow(){
 M5.Lcd.clear(BLACK);
 M5.Lcd.setTextColor(WHITE);

  // upper text
  M5.Lcd.drawRect(2, 2, 316, 60, WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(30,20);
  M5.Lcd.print("Latest Sent Value:");
  M5.Lcd.setCursor(30,40);
  M5.Lcd.print("     Elapsed Time:");

  // A button text
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawRect(6, 218, 110, 20, WHITE);
  M5.Lcd.setCursor(20, 220);
  M5.Lcd.print("QR DISP");

  DispQrCode(QR_HIDE_MODE);
}

void EspNowInit(){
  // ESP-NOW初期化
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
  } else {
    ESP.restart();
  }
  // マルチキャスト用Slave登録
  memset(&slave, 0, sizeof(slave));
  for (int i = 0; i < 6; ++i) {
    slave.peer_addr[i] = (uint8_t)0xff;
  }
  esp_err_t addStatus = esp_now_add_peer(&slave);
  if (addStatus == ESP_OK) {
    // Pair success
    Serial.println("Pair success");
  }
}

// esp-now 受信コールバック
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  float receiveValue = (float)data[0] + ((float)data[1] /100);
  // sigfox側に送信
  SendValue(receiveValue);
  // 経過時間の初期化
  elapsedTimeUpdated(ELAPSED_TIME_MODE_CLEAR);
  // 画面に送った値の表示
  latestSentValueDispUpdate(receiveValue);
}

void setup() {
  // m5 config
  M5.begin(true, false, true);
  // sigfox config
  Serial2.begin(9600, SERIAL_8N1, 13, 14);
  // esp-now config
  EspNowInit();
  esp_now_register_recv_cb(OnDataRecv);
  // disp config
  InitShow();
  // timer config
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
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
  // do nothing
}
