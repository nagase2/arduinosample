
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include <ESP8266WiFi.h>
#include <SimpleTimer.h>


#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define LED  14   //the Grove - LED is connected to D4 of Arduino
#define MODE_PIN  15
//#define SEND_HTTP_COUNT  60
//HTTPリクエストを送る頻度(適正値：１０）
#define EXISTING_SENCE_CNT 1
#define SLEEP_DURATION 5 //スリープする時間（秒単位）（適正値：３０〜６０）
#define SMALL_LED  12


//#define PIR_POWER  15 //黄色
//#define DEEP_SLEEP_COUNT 3 //Deep s]leep に入るまでのカウント数(連続して検知ができなかったら） 
//一HTTPリクエスト中、誰かがいると検知する最低回数。
//#define DETECT_FREQ  1000 //センサの値をとる頻度

#define GRAPH_TRUE  10 //グラフの存在数値
#define GRAPH_FALSE  1 //グラフの不在数値


SimpleTimer timer;
int count = 0;

int loopCount = 0;
int detectedCountPIR1 = 0;
int detectedCountPIR2 = 0;
int sleepCount = 0; //ディープスリープカウント
int requestFreq = 60;

const char* ssid     = "NAGA12345";
const char* password = "nagase222";
const char* host = "api-m2x.att.com";

// Use WiFiClient class to create TCP connections
WiFiClient client;
const char* m2xKey = "7e7b47ba14a03597d6ad729c0313e4ae";


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
  pinMode(MODE_PIN, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(SMALL_LED, OUTPUT);
  //pinMode(PIR_POWER, OUTPUT);

}
void submitToM2X(int PIRNumber, int returnValue) {
  String sensorID = "";
  if (PIRNumber == 1) {
    sensorID = "PIR_sensor";
  } else {
    sensorID = "-------";
  }

  String m2xURL =  "/v2/devices/33d8824f9deee514852ee77258d74b42/streams/" + sensorID + "/value";

  //M2xにデータを送信する
  client.print("PUT " + m2xURL + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: curl/7.43.0\r\n" +
               "Accept: */*\r\n" +
               "X-M2X-KEY: " + m2xKey + "\r\n" +
               "Content-Type: application/json\r\n" +
               // "Connection: close\r\n" +
               "Content-Length: 17\r\n\r\n" +
               "{ \"value\": \"" + returnValue + "\" }\r\n");
}


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
   deepSleep実施
*/
void deepSleep() {
  //誰もいなかったら DEEP SLEEPモード突入命令
  Serial.println("DEEP SLEEP START!!");
  //LED点滅
  blinkLED(SMALL_LED, 100, 2);
  blinkLED(LED, 300, 3);

  //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
  ESP.deepSleep(SLEEP_DURATION * 1000 * 1000 , WAKE_RF_DEFAULT);
  //deepsleepモード移行までのダミー命令
  delay(1000);
}

/**
   人感センサで検知する。タイマーで指定された頻度で実施する。
*/
void sensePIR() {
  loopCount++;
  int returnValue = 0;
  if (isPeopleDetected()) { //if it detects the moving people?
    digitalWrite(LED, HIGH); //人感センサ検知したらLED点灯
    detectedCountPIR1++;
    Serial.printf("PIR1:There!!![%d]\n", loopCount);
    //returnValue = 10;
  }
  else {
    digitalWrite(LED, LOW); //人感センサ検知なければLED消灯
    Serial.printf("PIR1:no one[%d]\n", loopCount);
    //Blynk.virtualWrite(V2,GRAPH_FALSE);
    //returnValue = 1;
  }


  /*if(digitalRead(PIR_MOTION_SENSOR2)==HIGH){
    Serial.println("PIR2:detected someone");
    detectedCountPIR2++;
    }else{
    Serial.println("PIR2:could not find anyone");
    }*/

   Serial.print("detected count now is "+detectedCountPIR1);

  if (loopCount >= requestFreq) { //送信頻度を超えたら実行
    //String  s1 = "abcde";

    if (detectedCountPIR1 >= EXISTING_SENCE_CNT) {
      analogWrite(SMALL_LED, 10);
      submitToM2X(1, detectedCountPIR1); //存在
      Serial.print("----HTTP request had been sent. There was some person!!!!----\n");
      sleepCount = 0; //スリープカウントをリセット
    } 
    else {

      analogWrite(SMALL_LED, 0);
      submitToM2X(1, 0); //不在
      Serial.print("----HTTP request had been sent. no body was there----\n");
    }

    //reset roop count
    loopCount = 0;
    detectedCountPIR1 = 0;
    detectedCountPIR2 = 0;

    deepSleep(); //毎回Sleepしたい場合は、この行のコメント外す

  }
  //delay(DETECT_FREQ);
}


void startWIFI() {

  Serial.println("start wifi");
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  const int httpPort = 80;
  Serial.println("connect to client");
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed 1");
    delay(5000);
    if (!client.connect(host, httpPort)) {
      delay(5000);
      Serial.println("connection failed 2");

      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed 3 going to deep sleep mode now..");
        ESP.restart();
      }
    }

  }
}



/**
   起動時に実行。Wifiに接続する。
*/
void setup() {

  Serial.begin(115200);
  Serial.println("starting  setup");
  pinsInit();
  //接続開始を示すLED点灯
  analogWrite(SMALL_LED, 5); //一旦LED点灯。

  startWIFI();

  //接続完了したことをLED点滅で示す。
  for (int k = 0; k < 3; k++) {
    for (int i = 0; i <= 200; i++) {
      //Serial.println(i);
      analogWrite(SMALL_LED, i);
      analogWrite(LED, i);
      delay(1);
    }
    for (int i = 200; i >= 0; i--) {
      //Serial.println(i);
      analogWrite(SMALL_LED, i);
      analogWrite(LED, i);
      delay(1);
    }
  }
  Serial.println("end of setup");
}



void loop()
{
  count++;

  //スイッチの値を読み取り、更新頻度を決定する。
  if(digitalRead(MODE_PIN)==HIGH){
    requestFreq = 2;
  }else{
    requestFreq = 60;
  }
  
  sensePIR();
  delay(1000);
  startWIFI();
}




