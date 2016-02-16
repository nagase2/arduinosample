/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
#define GREEN_LED  16 //Green

ESP8266WiFiMulti WiFiMulti;

int red = 13;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

    USE_SERIAL.begin(115200); //setup serial port
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
 digitalWrite(GREEN_LED, HIGH);   
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("NAGA12345", "nagase222");

}

void loop() {
  digitalWrite(GREEN_LED, LOW); 
  digitalWrite(red, HIGH);   // turn the LED on (HIGH is the voltage level)
  /*
    // set the brightness of pin 9:
 // analogWrite(led, brightness);
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount ;
  }
*/
  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("192.168.1.12", 443, "/test.html", true, "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("api-m2x.att.com/v2/devi94a8a3d8b62ecd0/streams/kkkk/value", 80, "/http/"); //HTTP
        http.addHeader("X-M2X-KEY","67e01b6454c2af670ff6bea10ad1893e");
        http.addHeader("Content-Type","application/json");
        

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        if(httpCode) {
            // HTTP header has been send and Server response header has been handlxed
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == 200) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.print("[HTTP] GET... failed, no connection or no HTTP server\n");
        }
    }
  digitalWrite(red, LOW);    // turn the LED off by making the voltage LOW
    delay(2000);
}


