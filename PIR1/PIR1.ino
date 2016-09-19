/**
 * 人感センサの情報を取得し、人を検知したらLEDを一秒点灯させる。
 */
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <ESP8266WiFi.h>


#define PIR_MOTION_SENSOR 4  //人感センサを示す番号
#define GREEN_LED  15  //緑のLEDを示す番号（１５番ピンに接続）

int loopCount = 0; //現在のループ数
int detectedCount = 0; //人感センサの検知回数

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
  sensePIR();//人感センサの状態チェック
  delay(1000);//一秒待機
}
