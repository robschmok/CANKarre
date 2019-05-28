//flags triggering when gauges hit endpoints in testing environment
boolean gEndpoint = true;
boolean dEndpoint = true;

//functions for letting the speed gauge oscillate over whole scale
void geschwindigkeitTest(float s){
  if(gEndpoint){
    geschwindigkeit+=s;
  } else{
    geschwindigkeit-=s;
  }
  
  if(geschwindigkeit >= 65535){
    gEndpoint = !gEndpoint;
    geschwindigkeit = 65534;
  }
  
  if(geschwindigkeit <= 0){
    gEndpoint = !gEndpoint;
    geschwindigkeit = 1;
  }
}

//functions for letting the motor rotation(?) gauge oscillate over whole scale
void drehzahlTest(float s){
  if(dEndpoint){
    drehzahl+=s;
  } else{
    drehzahl-=s;
  }
  
  if(drehzahl >= 65535){
    dEndpoint = !dEndpoint;
    drehzahl = 65534;
  }
  
  if(drehzahl <= 0){
    dEndpoint = !dEndpoint;
    drehzahl = 1;
  }
}

//key events for testing clutch etc.
void keyPressed(){
  if(testEnvironment){
    if(key == 'k'){
      kupplung = true;
    }
    if(key == 'r'){
      gang = 0;
    }
    if(key == 'c'){
      sportModus = false;
    }
    if(key == 's'){
      sportModus = true;
    }
    if(key == '1'){
      gang = 1;
    }
    if(key == '2'){
      gang = 2;
    }
    if(key == '3'){
      gang = 3;
    }
    if(key == '4'){
      gang = 4;
    }
    if(key == '5'){
      gang = 5;
    }
    if(key == '6'){
      gang = 6;
    }
    if(key == '7'){
      gang = 7;
    }
    if(keyCode == LEFT){
      blinkerLinks = !blinkerLinks;
    }
    if(keyCode == RIGHT){
      blinkerRechts = !blinkerRechts;
    }
  }
}

void keyReleased(){
  if(testEnvironment){
    if(key == 'k'){
      kupplung = false;
    }
  }
}
