/*

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

*/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN  9
#define SS_PIN  10

String key;
String accepted = "FAE1AB0";

int rs = A0;
int en = A1;
int d0 = A2;
int d1 = A3;
int d2 = A4;
int d3 = A5;

int trig = 2;
int echo = 3;

int blue = 4 ;
int purple = 5;
int yellow = 6;
int orange = 7;

int buzzer = 8;

int car_space_left = 5;
int car_distance = 10;

int dt = 1000;
int car_dt = 2000;
int buzzer_dt = 200;
int stepper_dt = 15;
int access_denied_dt = 500;
int ultrasonic_dt = 10;

int distance;
int travel_time;

LiquidCrystal lcd (rs, en, d0, d1, d2, d3);
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(blue, OUTPUT);
  pinMode(purple, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(orange, OUTPUT);

  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);

  Serial.begin(9600);

  for (int i = 4; i >= 7; i++) {

    digitalWrite(i, LOW);

  }

  for (int i = 0; i <= 23; i++) {

    lcd.setCursor(1, 0);
    lcd.scrollDisplayLeft();
    lcd.print("Automatic Car Management System");
    delay(dt);
    lcd.noAutoscroll();

  }

  lcd.clear();

}

void getKey(byte *buffer, byte bufferSize) {

  String raw_key;

  for (byte i = 0; i < bufferSize; i++) {

    raw_key += String(buffer[i], HEX);

  }

  raw_key.toUpperCase();
  key = raw_key;

}

void openBarrier() {

  for (int i = 0; i <= 120; i++) {

  digitalWrite(orange, HIGH);
  digitalWrite(purple, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, LOW);

  delay(stepper_dt);

  digitalWrite(orange, LOW);
  digitalWrite(purple, HIGH);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, LOW);

  delay(stepper_dt);

    digitalWrite(orange, LOW);
  digitalWrite(purple, LOW);
  digitalWrite(yellow, HIGH);
  digitalWrite(blue, LOW);

  delay(stepper_dt);
  
  digitalWrite(orange, LOW);
  digitalWrite(purple, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, HIGH);

  delay(stepper_dt);
  }

}

void closeBarrier() {

  for (int i = 120; i >= 0; i--) {

  digitalWrite(orange, LOW);
  digitalWrite(purple, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, HIGH);

  delay(stepper_dt);


  digitalWrite(orange, LOW);
  digitalWrite(purple, LOW);
  digitalWrite(yellow, HIGH);
  digitalWrite(blue, LOW);

  delay(stepper_dt);

  digitalWrite(orange, LOW);
  digitalWrite(purple, HIGH);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, LOW);

  delay(stepper_dt);


  digitalWrite(orange, HIGH);
  digitalWrite(purple, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, LOW);


  delay(stepper_dt);
  }
}

void loop() {

  digitalWrite(trig, LOW);
  delayMicroseconds(ultrasonic_dt);

  digitalWrite(trig, HIGH);
  delayMicroseconds(ultrasonic_dt);
  digitalWrite(trig, LOW);

  travel_time = pulseIn(echo, HIGH);
  distance = 0.0343 * (travel_time / 2);

  if (distance <= car_distance) {

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Security Alert");

    for (int i = 0; i <= 5; i++) {

      digitalWrite(buzzer, HIGH);
      delay(buzzer_dt);
      digitalWrite(buzzer, LOW);
      delay(buzzer_dt);
    }

  }

  if (car_space_left <= 0) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car Park Is Full");

  }

  else {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car Slot Left:");

    lcd.setCursor(14, 0);
    lcd.print(car_space_left);

    lcd.setCursor(0, 1);
    lcd.print("Place your ID");

  }


  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  getKey(mfrc522.uid.uidByte, mfrc522.uid.size);

  if ((key == accepted) && (car_space_left > 0)) {

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access granted");

    delay (dt);

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Barrier is");
    lcd.setCursor(5, 1);
    lcd.print("Opening");

    openBarrier();

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Barrier is");
    lcd.setCursor(6, 1);
    lcd.print("Open");


    for (int i = 0; i<=2000; i++) {

      digitalWrite(trig, LOW);
      delayMicroseconds(ultrasonic_dt);

      digitalWrite(trig, HIGH);
      delayMicroseconds(ultrasonic_dt);
      digitalWrite(trig, LOW);

      travel_time = pulseIn(echo, HIGH);
      distance = 0.0343 * (travel_time / 2);

      if (distance <= car_distance){

        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Car Slot");
        lcd.setCursor(2, 1);
        lcd.print("Reduced By 1");
    
        delay(car_dt);
    
        car_space_left -= 1;

        break;
        
      }
    }

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Barrier Is");
    lcd.setCursor(5, 1);
    lcd.print("Closing");

    delay(dt);

    closeBarrier();

  }
  else if (car_space_left <= 0) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car Park Is Full");

  }

  else {

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access denied");

    delay(access_denied_dt);
  }

}