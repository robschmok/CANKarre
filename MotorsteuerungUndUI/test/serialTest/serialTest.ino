volatile byte msbGeschwindigkeit = 0xBE;
volatile byte lsbGeschwindigkeit = 0x20;
volatile byte msbDrehzahl = 0xBF;
volatile byte lsbDrehzahl = 0x40;
volatile byte rest = 0x62;
volatile bool blinker = false;

void setup() {
  Serial.begin(115200);

  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 7812;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

ISR(TIMER1_COMPA_vect){
  blinker = !blinker;
}

void loop(){
  senden();
}

void senden(){
  byte bRechtsOut = 0;
  byte bLinksOut = 0;
  if(((rest&0x20) == 0x20) && blinker){
    bRechtsOut = 0x20;
  }

  if(((rest&0x40) == 0x40) && blinker){
    bLinksOut = 0x40;
  }

  byte restNeu = rest&0x9F;
  restNeu |= bRechtsOut | bLinksOut;

  Serial.write(0xFF);
  Serial.write(msbGeschwindigkeit);
  Serial.write(lsbGeschwindigkeit);
  Serial.write(msbDrehzahl);
  Serial.write(lsbDrehzahl);
  Serial.write(restNeu);
  delay(10);

  msbGeschwindigkeit++;
  msbDrehzahl++;

  if(msbGeschwindigkeit >= 0xFE) msbGeschwindigkeit = 0x00;
  if(msbDrehzahl >= 0xFE) msbDrehzahl = 0x00;
}


