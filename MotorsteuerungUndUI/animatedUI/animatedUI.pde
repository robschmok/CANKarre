//Animated UI with serial arduino interface for the Karre
//@Robin Holzwarth

float drehzahl = 100;
float maxDrehzahl = 255;
float geschwindigkeit = 0;
float maxGeschwindigkeit = 255;


boolean gTestFlag = true;
boolean dTestFlag = true;

PImage bckgrnd;

import processing.serial.*;

Serial arduino;

//2xGeschwindigkeit, Drehzahl, Gang/Kupplung; je die drei letzten werte
byte[][] input = new byte[4][3];

byte index = 0;

void setup(){
  bckgrnd = loadImage("background.png");
  //default 720x480
  size(720, 480);
  background(bckgrnd);
  
  String portName = Serial.list()[2];

  arduino = new Serial(this, portName, 115200);
}



void draw(){
  background(bckgrnd);
  drawGauges();
  getInput();
  
}

void drawGauges(){
  float gaugeLength = height*0.35;
  
  pushMatrix();
  
  //tacho (rechts)
  translate(width - width/3.5, height/2);
  
  //zeiger
  float alpha = -((3*PI/2) * (geschwindigkeit/maxGeschwindigkeit) + PI/4);
  strokeWeight(5);
  stroke(255, 165, 0);
  line(0, 0, sin(alpha)*gaugeLength, cos(alpha)*gaugeLength);

  popMatrix();
  pushMatrix();
  
  //drehzahl (links)
  translate(width/3.5, height/2);
  
  //zeiger
  float beta = -(PI * (drehzahl/maxDrehzahl) + PI/4);
  strokeWeight(5);
  stroke(255, 165, 0);
  line(0, 0, sin(beta)*gaugeLength, cos(beta)*gaugeLength);

  popMatrix();
}

void geschwindigkeitTest(float s){
  if(gTestFlag){
    geschwindigkeit+=s;
  } else{
    geschwindigkeit-=s;
  }
  
  if(geschwindigkeit >= 255){
    gTestFlag = !gTestFlag;
    geschwindigkeit = 254;
  }
  
  if(geschwindigkeit <= 0){
    gTestFlag = !gTestFlag;
    geschwindigkeit = 1;
  }
}

void drehzahlTest(float s){
  if(dTestFlag){
    drehzahl+=s;
  } else{
    drehzahl-=s;
  }
  
  if(drehzahl >= 255){
    dTestFlag = !dTestFlag;
    drehzahl = 254;
  }
  
  if(drehzahl <= 0){
    dTestFlag = !dTestFlag;
    drehzahl = 1;
  }
}

void getInput(){
  //Maximalwerte: 280, 8000, 6+r
  if(arduino.available() >= 4){
    input[0][index] = (byte)arduino.read();
    input[1][index] = (byte)arduino.read();
    input[2][index] = (byte)arduino.read();
    input[3][index] = (byte)arduino.read();
  }
  
  index++;
  
  if(index > 2) index = 0;
}

void computeValues(){
  
}
