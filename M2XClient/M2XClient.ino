
#include <ESP8266WiFi.h>

#define DHTPIN 12     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT22   // DHT 11
//DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "NAGA12345";
const char* password = "nagase222";

const char* host = "api-m2x.att.com";
String tempurl =  "/v2/devices/b4f55b41c6a7269fe0a2612651474aed/streams/pir/value";
//const char* host = "192.168.1.80";
//String tempurl = "/rest";


const char* m2xKey = "67e01b6454c2af670ff6bea10ad1893e";


void setup() {
  Serial.begin(115200);
  delay(1000);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //Serial.println("DHT22 test!");
  //dht.begin();
}

void loop() {
  delay(2000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  //const int httpPort = 7776;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(tempurl);

  // This will send the request to the server
  client.print("PUT " + tempurl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: curl/7.43.0\r\n" +
               "Accept: */*\r\n" +
               "X-M2X-KEY: " + m2xKey + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Connection: close\r\n" + 
               "Content-Length: 17\r\n\r\n" +
               "{ \"value\": \"21\" }\r\n");

  delay(1000);

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

 

  //1:μ秒での復帰までのタイマー時間設定  2:復帰するきっかけの設定（モード設定）
  //ESP.deepSleep(300 * 1000 * 1000 , WAKE_RF_DEFAULT);

  //deepsleepモード移行までのダミー命令
  delay(1000);

}
