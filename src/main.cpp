#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <MFRC522.h>

#define echo 9
#define trigger 8
#define Taster 7
#define Motor_Links 10
#define Motor_Rechts 11
#define Piezo 3
#define eingang A0
#define LED 5
#define SS_PIN 10
#define RST_PIN 9

#define ldrLevel5 50
#define ldrLevel4 110
#define ldrLevel3 300
#define ldrLevel2 430
#define ldrLevel1 700


bool Motor_State = false;
long Abstand;
long lastActionTime = 0;
const int pause_geschwindigkeit = 5000;
int aktuelleHelligkeit;
const int pause_helligkeit = 20000;

Adafruit_BMP280 bmp;
MFRC522 mfrc522(SS_PIN, RST_PIN);

bool Authorisieren(){
  mfrc522.PICC_IsNewCardPresent();
  mfrc522.PICC_ReadCardSerial();
  String RFID;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     RFID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     RFID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  RFID.toUpperCase();
  if (RFID.substring(1) == "99 45 A1 47")
  {
    return true;
  }
  else{
    return false;
    
  }
}

int getTemperature(){
  int temperatur = bmp.readTemperature();
  Serial.println(temperatur);
  return temperatur;
}

int getLDR(){
  aktuelleHelligkeit = analogRead(eingang);
  Serial.println(aktuelleHelligkeit);
  return aktuelleHelligkeit;
}

byte Geschwindigkeit(){
  if (millis() >= (lastActionTime + pause_geschwindigkeit)) {
      lastActionTime = millis();
    if (getTemperature() <= 4){ //wenn die Temperatur unter 4 Grad fällt, wird die Geschwindigkeit auf die Hälfte verringert
        return 127;
    }
    else {
      return 255;
    }
  }
}
 
int getAbstand(){
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger,HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  long duration = pulseIn(echo, HIGH);
  long entfernung = (duration / 2) * 0.03432;
  return entfernung;
}

void LichtNachHelligkeit(){
  if ((millis() >= (lastActionTime + pause_helligkeit))) {
    lastActionTime = millis();
   if(getLDR() <= ldrLevel5){
    analogWrite(LED, 255);
  }
  if(getLDR() <= ldrLevel4){
    analogWrite(LED, 170);
  }
  if(getLDR() <= ldrLevel3){
    analogWrite(LED, 125);
  }
 
  if(getLDR() <= ldrLevel2){
 
    analogWrite(LED, 100);
  }
  if(getLDR() <= ldrLevel1){
    analogWrite(LED, 50);
  }
  if(getLDR() >= ldrLevel1){
    analogWrite(LED, 0);
  }
  }
}
 
void setup() {
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(Motor_Links, OUTPUT);
  pinMode(Motor_Rechts, OUTPUT);
  pinMode(Taster, INPUT);
  pinMode(Piezo, OUTPUT);
  pinMode (LED, OUTPUT);
  Serial.begin(9600);
  bmp.begin(0x77);
  SPI.begin();
  mfrc522.PCD_Init();
  while(!Authorisieren()){
    Serial.print(".");
    delay(100);
  }
  if(Authorisieren()){
    Serial.println("Fahrzeug entriegelt");
  }
}
 
void loop() {
  LichtNachHelligkeit();
  if(digitalRead(Taster)){
    Motor_State = !Motor_State;
    while(digitalRead(Taster));    
  }
  Abstand = getAbstand();
  if(Abstand >= 8 && Motor_State){
    analogWrite(Motor_Rechts, Geschwindigkeit());
    analogWrite(Motor_Links, Geschwindigkeit());
  }
  else{
    digitalWrite(Motor_Rechts, LOW);
    digitalWrite(Motor_Links, LOW);
  }
  if(Abstand <= 20 && Motor_State){
    int Tonehohe = map(Abstand, 5, 20, 1000,100);
    tone(Piezo,Tonehohe);
  }
  else{
    noTone(Piezo);
  }
}