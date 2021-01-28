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
  //Serial.begin(9600);
  if(!bmp.begin()){
    //Serial.println("Temperatursensor nicht gefunden");
  }
  SPI.begin();
  mfrc522.PCD_Init();
  //Serial.print("Warte auf Authorisierung");
  while(!Authorisieren()){
    //Serial.print(".");
    delay(100);
  }
  if(Authorisieren()){
    //Serial.println();
    //Serial.println("Fahrzeug entriegelt");
    toneAC2(Piezo_Pin1, Piezo_Pin2, 800);
    delay(50);
    noToneAC2();
    delay(50);
    toneAC2(Piezo_Pin1, Piezo_Pin2, 800);
    delay(50);
    noToneAC2();  
    }
  
}
 
void loop() {
  if(digitalRead(Taster)){
    Motor_State = !Motor_State;
    if(Motor_State){
    //Serial.println("Motor gestartet!");
    }
    else{
      //Serial.println("Motor gestoppt!");
    }
    while(digitalRead(Taster));    
  }

  LichtNachHelligkeit();
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
    toneAC2(Piezo_Pin1, Piezo_Pin2,Tonehohe);
  }
  
  else{
    noToneAC2();
  }
}