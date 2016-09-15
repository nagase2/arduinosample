#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include <ESP8266WiFi.h>

#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define GREEN_LED  15   //the Grove - LED is connected to D4 of Arduino
#define MODE_PIN  13
#define SEND_HTTP_COUNT  10
//HTTPリクエストを送る頻度(適正値：１０）
#define EXISTING_SENCE_CNT 1
#define SLEEP_DURATION 5 //スリープする時間（秒単位）（適正値：３０〜６０）
#define SMALL_LED  12

#define GRAPH_TRUE  10 //グラフの存在数値
#define GRAPH_FALSE  1 //グラフの不在数値

int count = 0;

int loopCount = 0;
int detectedCount = 0;
//int detectedCountPIR2 = 0;
//int sleepCount = 0; //ディープスリープカウント



//--------------------
// Use WiFiClient class to create TCP connections
WiFiClient client;
WiFiClient m2xClient;

int requestFreq = 10; //サーバへの更新頻度
const char* ssid     = "NAGA12345";
const char* password = "nagase222";
const char* m2xHost = "api-m2x.att.com";
String m2xKey = "7e7b47ba14a03597d6ad729c0313e4ae";
String m2xURL =  "/v2/devices/33d8824f9deee514852ee77258d74b42/streams/PIR_sensor/value";
const int httpPort = 80;
//----------------------------

/**
   LEDを点滅させる
*/
void blinkLED(int pinNo, int waitTime, int repertCount) {
  for (int i = 0; i <= repertCount; i++) {
    digitalWrite(pinNo, HIGH);
    delay(waitTime);
    digitalWrite(pinNo, LOW);
    delay(waitTime);
  }
}

boolean isPeopleDetected()
{
  int sensorValue = digitalRead(PIR_MOTION_SENSOR);
  if (sensorValue == HIGH) //if the sensor value is HIGH?
  {
    return true;  //yes,return ture
  }
  else
  {
    return false;  //no,return false
  }
}

void pinsInit()
{
  pinMode(PIR_MOTION_SENSOR, INPUT);
  //pinMode(MODE_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  //pinMode(SMALL_LED, OUTPUT);
  //pinMode(PIR_POWER, OUTPUT);

  //pinMode(15, OUTPUT);
  //digitalWrite(15, HIGH);

}

/**
   人感センサで検知する。タイマーで指定された頻度で実施する。
*/
void sensePIR() {
  loopCount++;
  int returnValue = 0;
  if (isPeopleDetected()) { //if it detects the moving people?
    digitalWrite(GREEN_LED, HIGH); //人感センサ検知したらLED点灯
    detectedCount++;
    Serial.printf("PIR1: someone! loopCount=%d detectedCount=%d\n",loopCount,detectedCount);

  }
  else {
    digitalWrite(GREEN_LED, LOW); //人感センサ検知なければLED消灯
    Serial.printf("PIR1:no one can find..loopCount=%d \n", loopCount);
  }
}

//----------------
/*
 * Wifiに接続する
 */
void startWIFI() {

  Serial.println("starting wifi");
  WiFi.begin(ssid, password);

  int failedcount = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    failedcount++;
    Serial.println("Connection Failed! retriying.");
    //WiFi.forceSleepWake();
    delay(2000);
    if (failedcount >= 5) {
      Serial.println("Connection Failed! Rebooting...");
      ESP.restart();
    }
  }

  //Serial.println("connect to client");
  //M2Xサーバに接続する
  if (!client.connect(m2xHost, httpPort)) {
    Serial.println("接続に失敗しました。");
  }
}

/**
   M2xにデータを送信する
 * */
void submitToM2X(int PIRNumber, int returnValue) {

  if (returnValue >= 1) {  //検出数が０でない場合は送信対象とする
    if (!m2xClient.connect(m2xHost, 80)) {
      Serial.println("connection failed 1");
      delay(1000);
   
    }
    //M2xにデータを送信する
    m2xClient.print("PUT " + m2xURL + " HTTP/1.1\r\n" +
                    "Host: " + m2xHost + "\r\n" +
                    "User-Agent: curl/7.43.0\r\n" +
                    "Accept: */*\r\n" +
                    "X-M2X-KEY: " + m2xKey + "\r\n" +
                    "Content-Type: application/json\r\n" +
                    // "Connection: close\r\n" +
                    "Content-Length: 17\r\n\r\n" +
                    "{ \"value\": \"" + returnValue + "\" }\r\n");

    Serial.printf("sent %d to the m2x server\n", returnValue);
    String response = m2xClient.readString();
    Serial.println(response);
  } else {
    Serial.println("値が0なのでM2xには送信しません");

  }
}
//----------------

void setup(){
  Serial.begin(115200);
  Serial.println("starting  setup");
  pinsInit();
  Serial.println("end of setup");
}

void loop(){
  count++;
  //requestFreq = SEND_HTTP_COUNT;
  sensePIR();
  delay(1000);

   if (loopCount >= requestFreq) { //送信頻度を超えたら実行

    startWIFI();
    delay(1000);
    submitToM2X(1, detectedCount);

    loopCount = 0;
    detectedCount = 0;

  }
}

