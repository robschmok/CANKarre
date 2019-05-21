#include <Servo.h>

//statusvariablen
byte msbGeschwindigkeit = 0;
byte lsbGeschwindigkeit = 0;
byte msbDrehzahl = 0;
byte lsbDrehzahl = 0;
byte rest = 0;

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

void empfangen() {
  if(Serial.available() >= 5){
    msbGeschwindigkeit = Serial.read();
    lsbGeschwindigkeit = Serial.read();
    msbDrehzahl = Serial.read();
    lsbDrehzahl = Serial.read();
    rest = Serial.read();
  }
}

void loop(){
  empfangen();
  servos();
  leds();
}

void servos(){
  //tacho
  unsigned short tempMsbGeschwindigkeit = msbGeschwindigkeit;
  unsigned short geschwindigkeit = (tempMsbGeschwindigkeit<<8) + lsbGeschwindigkeit;
  byte tachoOut = map(geschwindigkeit, 0, 65535, 0, 180);
  Tacho.write(tachoOut);

  //drehzahl
  unsigned short tempMsbDrehzahl = msbDrehzahl;
  unsigned short drehzahl = (tempMsbDrehzahl<<8) + lsbDrehzahl;
  byte drehzahlOut = map(drehzahl, 0, 65535, 0, 180);
  Drehzahl.write(drehzahlOut);
}

void leds(){
  byte gang = rest&0x07;
  bool kupplung = (rest&0x08) == 0x08;
  bool sportMode = (rest&0x10) == 0x10;
  bool blinkerRechts = (rest&0x20) == 0x20;
  bool blinkerLinks = (rest&0x40) == 0x40;

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

  if(blinkerRechts){
    digitalWrite(18, HIGH);
  } else{
    digitalWrite(0, LOW);
  }

  if(blinkerLinks){
    digitalWrite(19, HIGH);
  } else{
    digitalWrite(19, LOW);
  }
}
