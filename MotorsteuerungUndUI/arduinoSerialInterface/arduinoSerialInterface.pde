//First sketch for interfacing with CAN arduino over serial
//@Robin Holzwarth

import processing.serial.*;

Serial arduino;

byte input[] = {};

void setup(){
  String portName = Serial.list()[2];

  arduino = new Serial(this, portName, 115200);
  
  //2xGeschwindigkeit, Drehzahl, Gang/Kupplung
  arduino.buffer(4);
}

void draw(){
}

void SerialEvent(Serial arduino){
  input = arduino.readBytes(4);
}

//Maximalwerte: 280, 8000, 6+r
