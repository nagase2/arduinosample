
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <SimpleTimer.h>


#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define LED  13   //the Grove - LED is connected to D4 of Arduino
#define SEND_HTTP_COUNT  20 //HTTPリクエストを送る頻度
#define EXISTING_SENCE_CNT 5 //一HTTPリクエスト中、誰かがいると検知する最低回数。
#define DETECT_FREQ  1000 //センサの値をとる頻度
#define GREEN_LED  14 //緑
#define YELLOW_LED  2 //黄色
#define DEEP_SLEEP_COUNT 3 //Deep sleep に入るまでのカウント数(連続して検知ができなかったら）


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
  pinMode(GREEN_LED, OUTPUT);
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
 * 人感センサで検知する。タイマーで指定された頻度で実施する。
 */
void sensePIR(){
     loopCount++;
  if(isPeopleDetected()){//if it detects the moving people?
    digitalWrite(LED,HIGH);
     notify++;
     Serial.printf("There!!![%d]\n",loopCount);
     Blynk.virtualWrite(V2,GRAPH_TRUE);
  }
  else{
    digitalWrite(LED,LOW);
    Serial.printf("no one[%d]\n",loopCount);
    Blynk.virtualWrite(V2,GRAPH_FALSE);
  }
  if(loopCount % 2 == 0){
    //検知カウントをクライアントに送信
    Blynk.virtualWrite(V1,notify);
  }
  if(loopCount >= SEND_HTTP_COUNT){ //20回に一度検証。
    String  s1 = "abcde";
    if(notify >= EXISTING_SENCE_CNT){
      //もし、一度でもTrueがあれば存在と通知。
      s1 = "----Dummy HTTP notification. There was some person!!!!----\n";
       digitalWrite(GREEN_LED,HIGH);
       Blynk.virtualWrite(V3,GRAPH_TRUE); //存在グラフ
       sleepCount=0; //スリープカウントをリセット
    }else{
      //一度もTrueがなければ不在と通知
      s1 = "----Dummy HTTP notification. no body was there----\n";
      digitalWrite(GREEN_LED,LOW);
      Blynk.virtualWrite(V3,GRAPH_FALSE); //存在グラフ

     
      if(sleepCount >= DEEP_SLEEP_COUNT){ //２回連続不在だったら
        //誰もいなかったら DEEP SLEEPモード突入命令
        Serial.println("DEEP SLEEP START!!");
        //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
        ESP.deepSleep(30 * 1000 * 1000 , WAKE_RF_DEFAULT);
        //deepsleepモード移行までのダミー命令
        delay(1000);
      }else{
         sleepCount++;
        Serial.println("Sleep count up");
      }
    }
     Serial.print(s1);
    //reset roop count
    loopCount = 0;
    notify = 0;
  }
  //delay(DETECT_FREQ);
}


void setup()
{
  Serial.begin(115200);
  Serial.println("starting  setup");
  pinsInit();

  Blynk.begin(auth, "NAGA12345", "nagase222");
    while (Blynk.connect() == false) {
       // Serial.println("waiting...");
       //  Blynk.begin(auth, "iPhone", "nagase222");
    }
 //  Blynk.begin(auth, "iPhone", "nagase222");
   
  //何もしていないのでコメントアウト
  timer.setInterval(1000, sendUptime);
  timer.setInterval(1000, sensePIR); 
  
  Serial.println("end of setup");
  digitalWrite(YELLOW_LED,LOW);
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



