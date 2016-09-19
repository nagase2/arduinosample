#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include <ESP8266WiFi.h>

#define PIR_MOTION_SENSOR 4  //人感センサを示す番号
#define GREEN_LED  15  //緑のLEDを示す番号（１５番ピンに接続）

int loopCount = 0; //現在のループ数
int detectedCount = 0; //人感センサの検知回数


 //------ここから↓----------
// Use WiFiClient class to create TCP connections
WiFiClient client;
WiFiClient m2xClient;

int requestFreq = 10; //サーバへの更新頻度
const char* ssid     = "NAGA12345"; //★接続するWifiのSSID
const char* password = "nagase222"; //★接続するWifiのパスワード
const char* m2xHost = "api-m2x.att.com"; //接続するホストのアドレス
String m2xKey = "7e7b47ba14a03597d6ad729c0313e4ae";//★M2Xに接続する為のKey
String m2xURL =  "/v2/devices/33d8824f9deee514852ee77258d74b42/streams/PIR_sensor/value";//★デバイスを示すURL
const int httpPort = 80;//ポート番号

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
    delay(2000);
    if (failedcount >= 5) {
      Serial.println("Connection Failed! Rebooting...");
      ESP.restart();
    }
  }
  Serial.println("M2xに接続します");
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
//-----↑ここまで-----------

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
/**
 * 人感センサの情報をチェックし、現在の検知状態を調べる。
 */
boolean isPeopleDetected(){
  int sensorValue = digitalRead(PIR_MOTION_SENSOR);
  if (sensorValue == HIGH){ //検知状態
    return true;  //yes,return ture
  }
  else if(sensorValue == LOW){  //未検知状態
    return false;  //no,return false
  }
}

/*
 * 人感センサをチェックし、結果をコンソールに出力
 */
void sensePIR() {
  loopCount++;
  int returnValue = 0;

  //センサの状態を調べて、周りに人がいるかをチェック
  if (isPeopleDetected()==true) { //人がいたら 
    digitalWrite(GREEN_LED, HIGH); //人感センサ検知したらLED点灯
    detectedCount++;
    Serial.printf("PIR1:find someone! loopCount=%d detectedCount=%d\n",loopCount,detectedCount);
  }
  else {        //人がいなかったら
    digitalWrite(GREEN_LED, LOW); //人感センサ検知なければLED消灯
    Serial.printf("PIR1:no one can find..loopCount=%d \n", loopCount);
  }
}

/**
 * 準備開始（最初に呼ばれる関数）
 */
void setup(){
  Serial.begin(115200);//シリアルポートを通信速度115200で開始
  Serial.println("start setup");

  pinMode(PIR_MOTION_SENSOR, INPUT); //ピンを入力状態にセット
  pinMode(GREEN_LED, OUTPUT); //ピンを出力状態にセット

  Serial.println("end of setup");
}

/**
 * セットアップの後に連続して呼ばれる
 */
void loop(){
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

