
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <SimpleTimer.h>


#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define LED  13   //the Grove - LED is connected to D4 of Arduino
#define SEND_HTTP_COUNT  10 //HTTPリクエストを送る頻度
#define EXISTING_SENCE_CNT 4 //一HTTPリクエスト中、誰かがいると検知する最低回数。
#define DETECT_FREQ  1000 //センサの値をとる頻度
#define SMALL_LED  12 
#define YELLOW_LED  2 //黄色
#define DEEP_SLEEP_COUNT 0 //Deep sleep に入るまでのカウント数(連続して検知ができなかったら）
#define SLEEP_DURATION 30 //スリープする時間（秒単位）


#define GRAPH_TRUE  10 //グラフの存在数値
#define GRAPH_FALSE  1 //グラフの不在数値

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "c78e67c6a1f8453186da5a610416b670";

SimpleTimer timer;
SimpleTimer timer2;
int count = 0;

int loopCount = 0;
int notify = 0;
int sleepCount = 0; //ディープスリープカウント


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
  pinMode(LED,OUTPUT);
  pinMode(SMALL_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
 void sendUptime()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  /*if(count%2 == 1){
   Blynk.virtualWrite(V5, LOW);
  }else{
    Blynk.virtualWrite(V5, HIGH);
  }*/
  //Blynk.virtualWrite(V1,count);
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
  if(isPeopleDetected()){//if it detects the moving people?
    digitalWrite(LED,HIGH); //人感センサ検知したらLED点灯
     notify++;
     Serial.printf("There!!![%d]\n",loopCount);
     Blynk.virtualWrite(V2,GRAPH_TRUE);
  }
  else{
    digitalWrite(LED,LOW); //人感センサ検知なければLED消灯
    Serial.printf("no one[%d]\n",loopCount);
    Blynk.virtualWrite(V2,GRAPH_FALSE); 
  }
  if(loopCount % 2 == 0){
    //検知カウントをクライアントに送信
    Blynk.virtualWrite(V1,notify);
  }
  if(loopCount >= SEND_HTTP_COUNT){
    String  s1 = "abcde";
    if(notify >= EXISTING_SENCE_CNT){
      
       
       analogWrite(SMALL_LED,10);
       Blynk.virtualWrite(V3,GRAPH_TRUE); //存在グラフ
       //もし、一度でもTrueがあれば存在と通知。
       Serial.print("----HTTP request had been sent. There was some person!!!!----\n");
       sleepCount=0; //スリープカウントをリセット
  
       deepSleep(); //毎回Sleep
    }else{
      
      analogWrite(SMALL_LED,0);
      Blynk.virtualWrite(V3,GRAPH_FALSE); //存在グラフ
      //一度もTrueがなければ不在と通知
      Serial.print("----HTTP request had been sent. no body was there----\n");
     
      if(sleepCount >= DEEP_SLEEP_COUNT){ //２回連続不在だったら
        deepSleep();
      }else{
         sleepCount++;
        Serial.println("Sleep count up");
      }
    }
    //reset roop count
    loopCount = 0;
    notify = 0;
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

  //接続開始を示すLED点灯
  analogWrite(SMALL_LED,5); //一旦LED点灯。

  Blynk.begin(auth, "NAGA12345", "nagase222");

  int connectFailedCount = 0;
  while (Blynk.connect() == false) {
       if(connectFailedCount > 5){
         Serial.println("I'm going to deepsleep mode, because there is no wifi to connect.");
         deepSleep();
       }else{
         Serial.println("I'm retrying to connect...");
         connectFailedCount++;
       }
       delay(1000);
  }
  //接続完了したことをLED点滅で示す。
  for(int k=0;k<3;k++){
    for(int i=0;i<=200;i++){
      //Serial.println(i);
      analogWrite(SMALL_LED,i);
      delay(1);
    }
     for(int i=200;i>=0;i--){
      //Serial.println(i);
      analogWrite(SMALL_LED,i);
      delay(1);
    }
  }
  Serial.println("end of setup");

  //タイマーを開始
  timer2.setInterval(1000, sendUptime); //何もしていないのでコメントアウト
  timer.setInterval(1000, sensePIR); 

}

void loop()
{
  count++;
  Blynk.run();
  timer.run(); // Initiates SimpleTimer
  timer2.run(); // Initiates SimpleTimer
  //Serial.println(count);
}

// This function will be called every time
// when App writes value to Virtual Pin 1
BLYNK_WRITE(V10)
{
  BLYNK_LOG("Got a value: %s", param.asStr());
  // You can also use: 
  // int i = param.asInt() or 
  // double d = param.asDouble()
  analogWrite(YELLOW_LED,param.asInt()); //アナログでLED点灯

}



