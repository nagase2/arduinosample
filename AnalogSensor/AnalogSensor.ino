/**
 * 人感センサの情報を取得し、人を検知したらLEDを一秒点灯させる。
 */
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}


void setup() {
 Serial.begin(115200); //シリアル通信も通常のArduinoとは違う
 pinMode(13, OUTPUT);
 pinMode(15, INPUT);
}
 
//TOUTピンからの入力値を取得
int getToutValue(){
 int res = system_adc_read(); //ここでTOUTの値を取得
 return res; 
}
 
void loop() {
 int val= getToutValue();
 Serial.print("value is:");
 Serial.println(val);
 delay(500);
 analogWrite(15,val/8);
/* if(val &gt; 580){
 digitalWrite(13, HIGH);
 delay(400); 
 }
 else{
 digitalWrite(13, LOW); 
 delay(400); 
 }*/
}

