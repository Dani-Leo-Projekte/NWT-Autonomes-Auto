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
long lastActionTime = 0;

int geschwindigkeit = 0;
int aktuelleHelligkeit;

const int pause_geschwindigkeit = 5000;
const int pause_helligkeit = 5000;

const char* RFID_ID = "99 45 A1 47";

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

int getLDR(){
  aktuelleHelligkeit = analogRead(eingang);
  return aktuelleHelligkeit;
}

byte Geschwindigkeit(){
  if (millis() >= (lastActionTime + pause_geschwindigkeit)) {
    lastActionTime = millis();
    if (getTemperature() <= FrostgefahrTemperatur){ //wenn die Temperatur unter 4 Grad fällt, wird die Geschwindigkeit auf die Hälfte verringert
        geschwindigkeit = 127;
    }
    else {
      geschwindigkeit = 255;
    }
  return geschwindigkeit;
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