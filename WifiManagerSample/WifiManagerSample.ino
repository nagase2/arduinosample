/**
 * このプログラムを使うことで、ユーザ自身にWifiのセットアップをさせることができる。
 * （プロキシの設定は？）
 * 
 * */
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager



#define PIR_MOTION_SENSOR 4 //Use pin 8 to receive the signal from the module 
#define LED  14   //the Grove - LED is connected to D4 of Arduino
#define MODE_PIN  16
#define SEND_HTTP_COUNT  10
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
//#define ESP8266_REG(addr) ((volatile uint32_t )(0x60000000+(addr)))


int count = 0;

int loopCount = 0;
int detectedCountPIR1 = 0;
int detectedCountPIR2 = 0;
int sleepCount = 0; //ディープスリープカウント
int requestFreq = 99; //サーバへの更新頻度
boolean sleepflag = false;
const char* ssid     = "NAGA12345";
const char* password = "nagase222";
const char* host = "api-m2x.att.com";
const int httpPort = 80;

//const char* host = "192.168.1.80";
//const char* host = "ec2-52-196-9-244.ap-northeast-1.compute.amazonaws.com";
//const int httpPort = 7776;

// Use WiFiClient class to create TCP connections
WiFiClient client;
const char* m2xKey = "7e7b47ba14a03597d6ad729c0313e4ae";


void pinsInit()
{
  pinMode(PIR_MOTION_SENSOR, INPUT);
  
  pinMode(LED, OUTPUT);
  pinMode(SMALL_LED, OUTPUT);
  //pinMode(PIR_POWER, OUTPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15,HIGH);
  
  pinMode(MODE_PIN, INPUT);
}




char proxyServer[40];

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
               "Connection: close\r\n" +
               "Content-Length: 17\r\n\r\n" +
               "{ \"value\": \"" + returnValue + "\" }\r\n");
               
  Serial.printf("sent %d to the server\n",returnValue);
  String response = client.readString();
  Serial.println(response);
}



WiFiManager wifiManager;
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
    WiFiManagerParameter custom_proxy_server("proxy", "proxy server", "", 20);
   

/**
   起動時に実行。Wifiに接続する。
*/
void setup() {

  Serial.begin(115200);
  Serial.println("starting  setup");
  pinsInit();

 

  //Pinの状態を調べる
  if(digitalRead(MODE_PIN)==LOW){
    //wifiManager.addParameter(&custom_mqtt_server);
    //wifiManager.addParameter(&custom_proxy_server);
    //設定モードにはいる
    Serial.println("set up mode will be began");
    wifiManager.startConfigPortal("OnDemandAP");
  }else{
    //前回設定で自動起動
     Serial.println("auto configration will be used.");
    wifiManager.autoConnect("RoomMoniter1");
  }
   //入力された値を取得
   //strcpy(proxyServer, custom_proxy_server.getValue());
  
  //接続開始を示すLED点灯
  analogWrite(SMALL_LED, 5); //一旦LED点灯。

  
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
  //設定された値を出力
  Serial.println(proxyServer);
  Serial.println("end of setup");
}



void loop()
{
  count++;

  //スイッチの値を読み取り、更新頻度を決定する。
  if(digitalRead(MODE_PIN)==LOW){
    requestFreq = 10;
    Serial.println("MODE is LOW");
  }else if(digitalRead(MODE_PIN)==HIGH){
    Serial.println("MODE is HIGH");
  }
 
  submitToM2X(1, 7);
  
  delay(2000);
 
}




