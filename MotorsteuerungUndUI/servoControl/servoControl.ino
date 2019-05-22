// Programm zur Steuerung der physischen UI
// @Gruppenmitglied1, @Gruppenmitglied2...
#include <Servo.h>

//Inputvariablen
byte msbGeschwindigkeit = 0;
byte lsbGeschwindigkeit = 0;
byte msbDrehzahl = 0;
byte lsbDrehzahl = 0;
byte rest = 0;

//Statusvariablen
unsigned short geschwindigkeit = 0;
unsigned short drehzahl = 0;
byte gang = 0;
bool kupplung = false;
bool sportMode = false;
bool blinkerRechts = false;
bool blinkerLinks = false;

//servopins
#define tachoPin 5
#define drehzahlPin 6

//servo-objekt initialisierung
Servo Tacho;
Servo Drehzahl;

void setup() {
  Serial.begin(115200);
  
  //servo attachment
  Tacho.attach(tachoPin);
  Drehzahl.attach(drehzahlPin);

  //7=Comfort, 8=Sport, 9-15=vorwärts, 16=rückwärts, 17=kupplung, 18=blinker rechts, 19=blinker links
  for(int i = 7; i <= 19; i++){
    pinMode(i, OUTPUT);
  }
}

void loop(){
  getInput();
  computeValues();
  servos();
  leds();
}

//Inputvariablen von der seriellen Schnittstelle beziehen
void getInput() {
  if(Serial.available() >= 12){
    //warten auf startbyte
    while(Serial.read() != 0xFF){

    }
    msbGeschwindigkeit = Serial.read();
    lsbGeschwindigkeit = Serial.read();
    msbDrehzahl = Serial.read();
    lsbDrehzahl = Serial.read();
    rest = Serial.read();
  }
}

//Statusvariablen aus Inputvariablen berechnen
void computeValues(){
  unsigned short tempMsbGeschwindigkeit = msbGeschwindigkeit;
  geschwindigkeit = (tempMsbGeschwindigkeit<<8) + lsbGeschwindigkeit;

  unsigned short tempMsbDrehzahl = msbDrehzahl;
  drehzahl = (tempMsbDrehzahl<<8) + lsbDrehzahl;

  gang = rest&0x07;
  kupplung = (rest&0x08) == 0x08;
  sportMode = (rest&0x10) == 0x10;
  blinkerRechts = (rest&0x20) == 0x20;
  blinkerLinks = (rest&0x40) == 0x40;
}

//Servosteuerung
void servos(){
  //tacho
  byte tachoOut = map(geschwindigkeit, 0, 65535, 0, 180);
  Tacho.write(tachoOut);

  //drehzahl
  byte drehzahlOut = map(drehzahl, 0, 65535, 0, 180);
  Drehzahl.write(drehzahlOut);
}

//LED-Steuerung
void leds(){
  //sport modus leds
  if(sportMode){
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
  } else{
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }

  //gang leds
  if(gang == 0){
    for(byte i = 9; i <= 15; i++){
      digitalWrite(i, LOW);
    }
    digitalWrite(16, HIGH);
  } else if(1<=gang&&gang<=7){
    for(byte i = 1; i <= gang; i++){
      digitalWrite(i+8, HIGH);
    }

    for(byte i = 7; i>gang; i--){
      digitalWrite(i+8, LOW);
    }
  }
  
  //kupplung leds
  if(kupplung){
    digitalWrite(17, HIGH);
  } else{
    digitalWrite(17, LOW);
  }

  //blinker leds
  if(blinkerRechts){
    digitalWrite(18, HIGH);
  } else{
    digitalWrite(18, LOW);
  }

  if(blinkerLinks){
    digitalWrite(19, HIGH);
  } else{
    digitalWrite(19, LOW);
  }
}
