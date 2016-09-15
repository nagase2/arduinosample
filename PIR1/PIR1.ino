/**
 * 人感センサの情報を取得し、人を検知したらLEDを一秒点灯させる。
 */
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include <ESP8266WiFi.h>

#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define GREEN_LED  15   //the Grove - LED is connected to D4 of Arduino
#define MODE_PIN  13
#define SEND_HTTP_COUNT  10
//HTTPリクエストを送る頻度(適正値：１０）
//#define EXISTING_SENCE_CNT 1
//#define SLEEP_DURATION 5 //スリープする時間（秒単位）（適正値：３０〜６０）
//#define SMALL_LED  12

#define GRAPH_TRUE  10 //グラフの存在数値
#define GRAPH_FALSE  1 //グラフの不在数値

int count = 0;

int loopCount = 0;
int detectedCount = 0;
int detectedCountPIR2 = 0;
int sleepCount = 0; //ディープスリープカウント
int requestFreq = 10; //サーバへの更新頻度
boolean sleepflag = false;


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
  pinMode(GREEN_LED, OUTPUT);
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

void setup(){
  Serial.begin(115200);
  Serial.println("starting  setup");
  pinsInit();
  Serial.println("end of setup");
}

void loop(){
  count++;
  requestFreq = SEND_HTTP_COUNT;
  sensePIR();

  delay(1000);
}
