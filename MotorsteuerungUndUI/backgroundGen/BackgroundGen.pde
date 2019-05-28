//Program for creating the UI background
//@Robin Holzwarth

void setup(){
  //default 720x480
  size(720, 480);
  background(51);

  
  drawBackground();
  saveFrame("../animatedUI/backgroundSD.png");
}

void draw(){
  //background(51);
  //drawBackground();
}

void drawBackground(){
  //circle radius
  float outer = height*0.4;
  
  //circles
  fill(255);
  stroke(51);
  strokeWeight(5);
  ellipse(width/3.5, height/2, height*0.8, height*0.8);
  ellipse(width - width/3.5, height/2, height*0.8, height*0.8);
  
  //centerpoints
  fill(51);
  stroke(51);
  strokeWeight(5);
  ellipse(width/3.5, height/2, height/20, height/20);
  ellipse(width - width/3.5, height/2, height/20, height/20);
  
  pushMatrix();
  
  //tacho (rechts)
  translate(width - width/3.5, height/2);
  
  //text
  textAlign(CENTER);
  fill(51);
  noStroke();
  textSize(16);
  text("km/h", 0, -height/5);
  
  //skala
  for(float i = 0; i < 320; i+=1){
    float inner = height*0.38;
    stroke(51);
    strokeWeight(1);
    float angle = -((3*PI/2) * (i/320) + PI/4);
    line(inner*sin(angle), inner*cos(angle), outer*sin(angle), outer*cos(angle));
  }
  
  for(float i = 0; i <= 320; i+=5){
    float inner = height*0.36;
    stroke(51);
    strokeWeight(1);
    float angle = -((3*PI/2) * (i/320) + PI/4);
    line(inner*sin(angle), inner*cos(angle), outer*sin(angle), outer*cos(angle));
  }
  
  for(float i = 0; i <= 320; i+=20){
    float inner = height*0.35;
    stroke(51);
    strokeWeight(1);
    float angle = -((3*PI/2) * (i/320) + PI/4);
    line(inner*sin(angle), inner*cos(angle), outer*sin(angle), outer*cos(angle));
    float textRadius = 0.31*height;
    textSize(18);
    textAlign(CENTER, CENTER);
    text((int)i, textRadius*sin(angle), textRadius*cos(angle));
  }
  
  float innerR = height*0.35;
  stroke(255, 0, 0);
  strokeWeight(2);
  float angle2 = -((3*PI/2) * ((float)30/320) + PI/4);
  line(innerR*sin(angle2), innerR*cos(angle2), outer*sin(angle2), outer*cos(angle2));
  float angle3 = -((3*PI/2) * ((float)50/320) + PI/4);
  line(innerR*sin(angle3), innerR*cos(angle3), outer*sin(angle3), outer*cos(angle3));
  
  popMatrix();
  pushMatrix();
  
  
  //drehzahl (links)
  translate(width/3.5, height/2);
  
  //text
  textAlign(CENTER);
  fill(51);
  noStroke();
  textSize(16);
  text("1/min x 1000", 0, -height/5);
  
  //skala  
  for(float i = 0; i < 32; i+=1){
    float inner = height*0.38;
    stroke(51);
    strokeWeight(1);
    float angle = -(PI * (i/32) + PI/4);
    line(inner*sin(angle), inner*cos(angle), outer*sin(angle), outer*cos(angle));
  }
  
  for(float i = 0; i <= 32; i+=4){
    float inner = height*0.35;
    stroke(51);
    strokeWeight(2);
    float angle = -(PI * (i/32) + PI/4);
    line(inner*sin(angle), inner*cos(angle), outer*sin(angle), outer*cos(angle));
    float textRadius = 0.31*height;
    textSize(30);
    textAlign(CENTER, CENTER);
    text((int)i/4, textRadius*sin(angle), textRadius*cos(angle));
  }
  
  popMatrix();
}
