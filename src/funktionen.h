//Bibliotheken
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <MFRC522.h>
#include <config.h>

//Variablen
bool Motor_State = false;

long Abstand;
long lastActionTime_Geschwindigkeit = 0;
long lastActionTime_Helligkeit = 0;

int geschwindigkeit = 255;
int aktuelleHelligkeit;

int SensorLinksData = 0;
int SensorRechtsData = 0;
int FaktorRechts = 0;
int FaktorLinks = 0;

const int pause_geschwindigkeit = 5000;
const int pause_helligkeit = 5000;

const char* RFID_ID = "E5 56 D7 10";

//Objekte
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
  if (RFID.substring(1) == RFID_ID)
  {
    return true;
  }
  else{
    return false;
    
  }
}

int getTemperature(){
  int temperatur = bmp.readTemperature();
  return temperatur;
}

byte Geschwindigkeit(){
  if (millis() >= (lastActionTime_Geschwindigkeit + pause_geschwindigkeit)) {
    lastActionTime_Geschwindigkeit = millis();
    if (getTemperature() <= FrostgefahrTemperatur){ //wenn die Temperatur unter 4 Grad fällt, wird die Geschwindigkeit auf die Hälfte verringert
        geschwindigkeit = 127;
    }
    else {
      geschwindigkeit = 255;
    }
  }
  return geschwindigkeit;
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
  if ((millis() >= (lastActionTime_Helligkeit + pause_helligkeit))) {
    aktuelleHelligkeit = analogRead(eingang);
    if(aktuelleHelligkeit < 500){
      analogWrite(LED, map(aktuelleHelligkeit, 0, 500, 255, 50));
    }
    else{
      digitalWrite(LED, LOW);
    }
    lastActionTime_Helligkeit = millis();
  }
}

void Entriegelung(){
  for(int i = 0; i < 255; i++){
    analogWrite(LED, i);
    toneAC2(Piezo_Pin1, Piezo_Pin2, map(i, 0, 255, 500, 700));
    delay(3);
  }
  for(int i = 255; i > 0; i--){
    analogWrite(LED, i);
    toneAC2(Piezo_Pin1, Piezo_Pin2, map(i, 0, 255, 500, 700));
    delay(3);
  }
  analogWrite(LED, LOW);
  toneAC2(Piezo_Pin1, Piezo_Pin2, 800);
  delay(50);
  noToneAC2();
  delay(50);
  toneAC2(Piezo_Pin1, Piezo_Pin2, 800);
  delay(50);
  noToneAC2();  
}

float Linie_folgen(bool i){
  SensorLinksData = analogRead(SensorLinks);
  SensorRechtsData = analogRead(SensorRechts);
  if(SensorRechtsData > 600){
    FaktorRechts = 0.5;
    FaktorLinks = 1;
  }
  else{
    Serial.println(SensorLinksData);
    if(SensorLinksData < 250){
      FaktorRechts  = 1;
      FaktorLinks = 0.5;
    }
    else{
      FaktorLinks = 1;
      FaktorRechts = 1;
    }
  }
  if(i){
    return FaktorRechts;
  }
  else {
    return FaktorLinks;
  }
}