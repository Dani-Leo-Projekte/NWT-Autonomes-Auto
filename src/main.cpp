//Bibliotheken
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <MFRC522.h>
#include <toneAC2.h>

//externe Datein
#include <config.h>
#include <funktionen.h>

void setup() {
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(Motor_Links, OUTPUT);
  pinMode(Motor_Rechts, OUTPUT);
  pinMode(Taster, INPUT);
  pinMode (LED, OUTPUT);
  noToneAC2();
  bmp.begin();
  SPI.begin();
  mfrc522.PCD_Init();

  while(!Authorisieren()){
    delay(100);
  }
  Entriegelung();
  
}
 
void loop() {
  if(digitalRead(Taster)){
    Motor_State = !Motor_State;
    while(digitalRead(Taster));    
  }

  LichtNachHelligkeit();
  Abstand = getAbstand();

  if(Abstand >= 8 && Motor_State){
    analogWrite(Motor_Rechts, Geschwindigkeit()*Linie_folgen(1));
    analogWrite(Motor_Links, Geschwindigkeit()*Linie_folgen(0));
  }
  else{
    digitalWrite(Motor_Rechts, LOW);
    digitalWrite(Motor_Links, LOW);
  }

  if(Abstand <= 20 && Motor_State){
    int Tonehohe = map(Abstand, 5, 20, 1000,100);
    toneAC2(Piezo_Pin1, Piezo_Pin2,Tonehohe);
  }  
  else{
    noToneAC2();
  }
}