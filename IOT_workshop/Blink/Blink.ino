/**
 * 赤と緑のLEDを点滅させます。
 * 赤のLEDはON／OFFの２段階、
 * 緑のLEDは多段階で点滅します。
 */
 #define RED_LED 15
 #define GREEN_LED  12
// the setup function runs once when you press reset or power the board
void setup() {
  //RED LED
  pinMode(15, OUTPUT);
  //GREEN LED
  pinMode(12, OUTPUT);
}

void loop() {
  Serial.println("start.");
  digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);           // wait for a second
  digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for a second

  //緑のLEDを点滅させる。
  blinkGreenLED();
}

/**
 * 緑のLEDを多段階で
 */
void blinkGreenLED(){
  Serial.println("green LED");
   //kは点滅させる回数
  for(int k=0;k<2;k++){
    for(int i=0;i<=200;i++){
      analogWrite(12,i);
      delay(3); //5ミリ秒待つ
    }
     for(int i=200;i>=0;i--){
      analogWrite(12,i);
      delay(3);
    }
  }
}

