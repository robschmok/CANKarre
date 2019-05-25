//arduino sketch for interface between CAN bus and processing sketch

#include <mcp_can.h>
#include <SPI.h>

//status variables
volatile byte msbGeschwindigkeit = 0;
volatile byte lsbGeschwindigkeit = 0;
volatile byte msbDrehzahl = 0;
volatile byte lsbDrehzahl = 0;
volatile byte rest = 0;
volatile bool blinker = false;

//CAN variables
#define CAN0_INT 2
const byte SPI_CS_PIN = 10;
MCP_CAN CAN0(SPI_CS_PIN);     // Set CS to pin 10
byte input[8];
byte empf_len;

void setup() {
  //start CAN and serial bus
  Serial.begin(115200);
  CAN0.begin(MCP_ANY, CAN_10KBPS, MCP_16MHZ);

  //init CAN and CAN interrupt
  CAN0.setMode(MCP_NORMAL);  // Change to normal mode to allow messages to be transmitted
  pinMode(CAN0_INT, INPUT);  // RX: Configuring pin for /INT input
  attachInterrupt(digitalPinToInterrupt(CAN0_INT), empfangen, FALLING);

  //init blinker timer
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  OCR1A = 3906;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

//ISR for blinker flag
ISR(TIMER1_COMPA_vect){
  blinker = !blinker;
}

//receive data over CAN
void empfangen() {
  if (CAN_MSGAVAIL == CAN0.checkReceive()) {
    long unsigned int canID;
    CAN0.readMsgBuf(&canID, &empf_len, input);
    //Ader (Motor, Getriebe)
    if(canID == 315){
      msbGeschwindigkeit = input[0];
      lsbGeschwindigkeit = input[1];
      msbDrehzahl = input[2];
      lsbDrehzahl = input[3];
      rest = input[4]&0x1F;
      if(msbGeschwindigkeit == 0xFF) msbGeschwindigkeit = 0xFE;
      if(lsbGeschwindigkeit == 0xFF) lsbGeschwindigkeit = 0xFE;
      if(msbDrehzahl == 0xFF) msbDrehzahl = 0xFE;
      if(lsbDrehzahl == 0xFF) lsbDrehzahl = 0xFE;
    //Nadine (Blinker)
    } else if(canID == 300){
      volatile bool blinkerRechts = (input[0]&0x01) == 0x01;
      volatile bool blinkerLinks = (input[0]&0x02) == 0x02;
      if(blinkerRechts){
        rest |= 0x20;
      } else{
        rest &= 0x5F;
      }

      if(blinkerLinks){
        rest |= 0x40;
      } else{
        rest &= 0x3F;
      }
    }
  }
}

void loop(){
  senden();
}

//send values to processing over serial
void senden(){
  //make turning lights blink via timer interrupt 
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

  //send data over serial (0xFF as start flag)
  Serial.write(0xFF);
  Serial.write(msbGeschwindigkeit);
  Serial.write(lsbGeschwindigkeit);
  Serial.write(msbDrehzahl);
  Serial.write(lsbDrehzahl);
  Serial.write(restNeu);
}


