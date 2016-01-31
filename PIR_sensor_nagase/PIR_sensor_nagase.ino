/*******************************************************************************/
/*macro definitions of PIR motion sensor pin and LED pin*/

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#define USE_SERIAL Serial

#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define LED  13//the Grove - LED is connected to D4 of Arduino
#define SEND_HTTP_COUNT  10 //HTTPリクエストを送る頻度
#define DETECT_FREQ  1000 //センサの値をとる頻度
#define GREEN_LED  16 //緑

int loopCount = 0;
int notify = 0;

void setup()
{
  USE_SERIAL.begin(115200); //set up serial port　これがないとメッセージ表示できない。
  pinsInit();
  USE_SERIAL.printf("start PIR detector\n");
  USE_SERIAL.flush();

}

void loop() 
{
    loopCount++;
  if(isPeopleDetected()){//if it detects the moving people?
    turnOnLED();
  
     notify++;
     Serial.printf("There!!![%d]\n",loopCount);
  }
  else{
    turnOffLED();
    Serial.printf("no one[%d]\n",loopCount);
  }
  
  if(loopCount >= SEND_HTTP_COUNT){ //１０回に一度検証。
    String  s1 = "abcde";
    if(notify >= 1){
      //もし、一度でもTrueがあれば存在と通知。
      s1 = "----Dummy HTTP notification. There was some person!!!!----\n";
       digitalWrite(GREEN_LED,HIGH);
    }else{
      //一度もTrueがなければ不在と通知
      s1 = "----Dummy HTTP notification. no body was there----\n";
      digitalWrite(GREEN_LED,LOW);
    }
     Serial.print(s1);
    //reset roop count
    loopCount = 0;
    notify = 0;
  }
  delay(DETECT_FREQ);
}

void pinsInit()
{
  pinMode(PIR_MOTION_SENSOR, INPUT);
  pinMode(LED,OUTPUT);
   pinMode(GREEN_LED, OUTPUT);
}
void turnOnLED()
{
 
  digitalWrite(LED,HIGH);
}
void turnOffLED()
{
  digitalWrite(LED,LOW);
}
/***************************************************************/
/*Function: Detect whether anyone moves in it's detecting range*/
/*Return:-boolean, ture is someone detected.*/
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
