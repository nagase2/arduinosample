
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <SimpleTimer.h>


#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define LED  13   //the Grove - LED is connected to D4 of Arduino
#define PIR_MOTION_SENSOR2  14   
#define SEND_HTTP_COUNT  30 //HTTPリクエストを送る頻度(適正値：１０）
#define EXISTING_SENCE_CNT 1 //一HTTPリクエスト中、誰かがいると検知する最低回数。
#define DETECT_FREQ  1000 //センサの値をとる頻度
#define SMALL_LED  12 
#define PIR_POWER  15 //黄色
#define DEEP_SLEEP_COUNT 0 //Deep s]leep に入るまでのカウント数(連続して検知ができなかったら） 
#define SLEEP_DURATION 5 //スリープする時間（秒単位）（適正値：３０〜６０）


#define GRAPH_TRUE  10 //グラフの存在数値
#define GRAPH_FALSE  1 //グラフの不在数値

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "c78e67c6a1f8453186da5a610416b670";

SimpleTimer timer;
SimpleTimer timer2;
int count = 0;

int loopCount = 0;
int detectedCountPIR1 = 0;
int detectedCountPIR2 = 0;
int sleepCount = 0; //ディープスリープカウント

const char* ssid     = "NAGA123456";
const char* password = "nagase222";
const char* host = "api-m2x.att.com";

// Use WiFiClient class to create TCP connections
WiFiClient client;
const char* m2xKey = "67e01b6454c2af670ff6bea10ad1893e";


boolean isPeopleDetected()
{
  int sensorValue = digitalRead(PIR_MOTION_SENSOR);
  if(sensorValue == HIGH)//if the sensor value is HIGH?
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
  pinMode(PIR_MOTION_SENSOR2, INPUT);
  pinMode(LED,OUTPUT);
  pinMode(SMALL_LED, OUTPUT);
  pinMode(PIR_POWER, OUTPUT);
  
}
void submitToM2X(int PIRNumber, int returnValue){
  String sensorID="";
  if(PIRNumber==1){
    sensorID = "pir";
  }else{
    sensorID = "PIR2";
  }
  
  String m2xURL =  "/v2/devices/b4f55b41c6a7269fe0a2612651474aed/streams/"+sensorID+"/value";
  
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
 * LEDを点滅させる
 */
void blinkLED(int pinNo, int waitTime, int repertCount){
     for(int i=0;i<=repertCount;i++){
      digitalWrite(pinNo,HIGH);
      delay(waitTime);
      digitalWrite(pinNo,LOW);
      delay(waitTime);
    }
}
/**
 * deepSleep実施
 */
void deepSleep(){
    //誰もいなかったら DEEP SLEEPモード突入命令
    Serial.println("DEEP SLEEP START!!");
    //LED点滅
    blinkLED(SMALL_LED,100,2);
    blinkLED(LED,300,3);
  
    //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
    ESP.deepSleep(SLEEP_DURATION * 1000 * 1000 , WAKE_RF_DEFAULT);
    //deepsleepモード移行までのダミー命令
    delay(1000);
}

/**
 * 人感センサで検知する。タイマーで指定された頻度で実施する。
 */
void sensePIR(){
     loopCount++;
  int returnValue = 0;
  if(isPeopleDetected()){//if it detects the moving people?
    digitalWrite(LED,HIGH); //人感センサ検知したらLED点灯
     detectedCountPIR1++;
     Serial.printf("PIR1:There!!![%d]\n",loopCount);
    // Blynk.virtualWrite(V2,GRAPH_TRUE);
     returnValue = 10;
  }
  else{
    digitalWrite(LED,LOW); //人感センサ検知なければLED消灯
    Serial.printf("PIR1:no one[%d]\n",loopCount);
    //Blynk.virtualWrite(V2,GRAPH_FALSE);
    returnValue = 1;
  }
  
  if(digitalRead(PIR_MOTION_SENSOR2)==HIGH){
    Serial.println("PIR2:detected someone");
    detectedCountPIR2++;
  }else{
    Serial.println("PIR2:could not find anyone");
  }

  
  if(loopCount % 2 == 0){
    //検知カウントをクライアントに送信
   // Blynk.virtualWrite(V1,detectedCountPIR1);
  }
  
             
  if(loopCount >= SEND_HTTP_COUNT){ //１０回程度に一度実行 
    String  s1 = "abcde";
    if(detectedCountPIR1 >= EXISTING_SENCE_CNT){
       analogWrite(SMALL_LED,10);
       //Blynk.virtualWrite(V3,GRAPH_TRUE); //存在グラフ
       submitToM2X(1,10); //存在
       
       //もし、一度でもTrueがあれば存在と通知。
       Serial.print("----HTTP request had been sent. There was some person!!!!----\n");
       sleepCount=0; //スリープカウントをリセット
    }else{
      analogWrite(SMALL_LED,0);
      //Blynk.virtualWrite(V3,GRAPH_FALSE); //存在グラフ
      submitToM2X(1,1); //不在
      
      //一度もTrueがなければ不在と通知
      Serial.print("----HTTP request had been sent. no body was there----\n");
     
    /*  if(sleepCount >= DEEP_SLEEP_COUNT){ //２回連続不在だったら
     *  Serial.println("Sleep because sleep count has been reached to DEEP_SLEEP_COUNT");
        deepSleep();
      }else{
         sleepCount++;
        Serial.println("Sleep count up");
      }*/
    }
    delay(1000);
    //2つめのセンサの情報をM2Xにアップ
     if(detectedCountPIR2 >= EXISTING_SENCE_CNT){
      submitToM2X(2,10); //存在
      Serial.println("PIR2 10 has been submitted");
     }else{
      submitToM2X(2,0); //存在
      Serial.println("PIR2 1 has been submitted");
     }
     //delay(5000); //データがアップされるのを待機
    
    deepSleep(); //毎回Sleepしたい場合は、この行のコメント外す
    
    //reset roop count
    loopCount = 0;
    detectedCountPIR1 = 0;
    detectedCountPIR2 = 0;
  }
  //delay(DETECT_FREQ);
}



/**
 * 起動時に実行。Wifiに接続する。
 */
void setup() {
  
  Serial.begin(115200);
  Serial.println("starting  setup");
  pinsInit();
  //人感センサの電源をON
  digitalWrite(PIR_POWER,HIGH);
  //接続開始を示すLED点灯
  analogWrite(SMALL_LED,5); //一旦LED点灯。
  //Blynkの環境セットアップ
 // Blynk.begin(auth, ssid, password);

/////////////////////
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.print(".");
  }
  delay(2000); //PIRセットアップのため少し待たせる？

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed 1");
    delay(5000);
    if (!client.connect(host, httpPort)) { 
      delay(5000); 
      Serial.println("connection failed 2");
      
      if (!client.connect(host, httpPort)) {
          Serial.println("connection failed 3 going to deep sleep mode now..");
          deepSleep();//コネクション失敗したら再起動
          return;
      }
    }
    
  }
  ////////////////////////////

  int connectFailedCount = 0;
 /* while (Blynk.connect() == false) {
       if(connectFailedCount > 5){
         Serial.println("I'm going to deepsleep mode, because there is no wifi to connect.");
         deepSleep();
       }else{
         Serial.println("I'm retrying to connect...");
         connectFailedCount++;
       }
       delay(1000);
  }*/
  //接続完了したことをLED点滅で示す。
  for(int k=0;k<3;k++){
    for(int i=0;i<=200;i++){
      //Serial.println(i);
      analogWrite(SMALL_LED,i);
      analogWrite(LED,i);
      delay(1);
    }
     for(int i=200;i>=0;i--){
      //Serial.println(i);
      analogWrite(SMALL_LED,i);
      analogWrite(LED,i);
      delay(1);
    }
  }
  Serial.println("end of setup");
  

  //タイマーを開始
  //timer2.setInterval(1000, sendUptime); //何もしていないのでコメントアウト
  timer.setInterval(1000, sensePIR); 

}


void loop()
{
  count++;
  //Blynk.run();
  timer.run(); // Initiates SimpleTimer
  timer2.run(); // Initiates SimpleTimer
  //Serial.println(count);
}




