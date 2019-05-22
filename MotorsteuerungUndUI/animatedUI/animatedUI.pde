//Animated UI with serial arduino interface for the Karre
//@Robin Holzwarth

//global status variables
int drehzahl = 0;
int maxDrehzahl = 65535;
int geschwindigkeit = 0;
int maxGeschwindigkeit = 65535;
//0=rückwarts; 1-7=forwärts
byte gang = 0;
boolean kupplung = false;
boolean sportModus = false;
boolean blinkerLinks = false;
boolean blinkerRechts = false;

//boolean to toggle test animations and button functions
boolean testEnvironment = false;

//background image variable
PImage bckgrnd;

//font for display values
PFont sevenSegmentFont;

void setup(){
  bckgrnd = loadImage("background.png");
  //default 720x480
  size(720, 480);
  background(bckgrnd);
  
  sevenSegmentFont = createFont("sevenSegment.ttf", 50);
  
  initPortAndSerial();
  //initSPortAndSerial
}

void draw(){
  background(bckgrnd);
  getInput();
  computeValues();
  if(blinkerRechts) drawBlinkerRechts();
  if(blinkerLinks) drawBlinkerLinks();
  drawGauges();
  drawDisplays();
  //sendSerial();
}

//function for drawing both gauges
void drawGauges(){
  float gaugeLength = height*0.35;
  
  pushMatrix();
  
  //tacho (rechts)
  translate(width - width/3.5, height/2);
  
  //zeiger
  float alpha = -((3*PI/2) * ((float)geschwindigkeit/maxGeschwindigkeit) + PI/4);
  strokeWeight(5);
  stroke(255, 165, 0);
  line(0, 0, sin(alpha)*gaugeLength, cos(alpha)*gaugeLength);

  popMatrix();
  pushMatrix();
  
  //drehzahl (links)
  translate(width/3.5, height/2);
  
  //zeiger
  float beta = -(PI * ((float)drehzahl/maxDrehzahl) + PI/4);
  strokeWeight(5);
  stroke(255, 165, 0);
  line(0, 0, sin(beta)*gaugeLength, cos(beta)*gaugeLength);

  popMatrix();
}

//function for drawing clutch and gear displays etc.
void drawDisplays(){
  int posx = width/2;
  int posy = 4*height/5;
  
  textAlign(CENTER, CENTER);
  textFont(sevenSegmentFont);
  if(1 <= gang && gang <= 7){
    float frac = (float)(gang-1)/6;
    float g = -pow(frac, 2)+1;
    float r = -pow(frac - 1, 4)+1;
    fill(r*255, g*255, 0);
    text((int)gang, posx, posy);
  } else if(gang == 0){
    fill(255, 0, 255);
    text("R", posx, posy);
  }
  posy = 37*height/40;
  
  noFill();
  stroke(255);
  strokeWeight(2);
  if(kupplung){
    ellipse(posx - 30, posy, 10, 10);
    ellipse(posx - 30, posy, 40, 40);
    ellipse(posx + 40, posy, 10, 10);
    ellipse(posx + 40, posy, 60, 60);
  } else{
    ellipse(posx - 20, posy, 10, 10);
    ellipse(posx - 20, posy, 40, 40);
    ellipse(posx + 30, posy, 10, 10);
    ellipse(posx + 30, posy, 60, 60);
  }
  
  posx = 2*width/7;
  posy = 2*height/3;
  
  textAlign(CENTER, CENTER);
  if(sportModus){
    fill(255, 0, 0);
    text("S", posx, posy);
  } else{
    fill(0, 0, 255);
    text("C", posx, posy);
  }
}

void drawBlinkerRechts(){
  float xoffset = width/15;
  pushMatrix();
  translate(width - width/3.5 + xoffset, height/2 - 10);
  fill(0, 255, 0);
  beginShape();
  vertex(0, 0);
  vertex(30, 0);
  vertex(30, -10);
  vertex(50, 10);
  vertex(30, 30);
  vertex(30, 20);
  vertex(0, 20);
  endShape();
  popMatrix();
}

void drawBlinkerLinks(){
  float xoffset = width/15;
  pushMatrix();
  translate(width - width/3.5 - xoffset, height/2 - 10);
  fill(0, 255, 0);
  beginShape();
  vertex(0, 0);
  vertex(-30, 0);
  vertex(-30, -10);
  vertex(-50, 10);
  vertex(-30, 30);
  vertex(-30, 20);
  vertex(0, 20);
  endShape();
  popMatrix();
}
