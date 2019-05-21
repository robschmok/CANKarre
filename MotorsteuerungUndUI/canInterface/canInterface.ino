#include <mcp_can.h>
#include <SPI.h>

volatile byte msbGeschwindigkeit = 0;
volatile byte lsbGeschwindigkeit = 0;
volatile byte msbDrehzahl = 0;
volatile byte lsbDrehzahl = 0;
volatile byte rest = 0;
volatile bool blinkerRechts = false;
volatile bool blinkerLinks = false;

#define CAN0_INT 2
const byte SPI_CS_PIN = 10;
MCP_CAN CAN0(SPI_CS_PIN);     // Set CS to pin 10
byte input[8];
byte empf_len;

void setup() {
  Serial.begin(115200);
  if (CAN0.begin(MCP_ANY, CAN_10KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!\n");
  } else{
    Serial.println("Error Initializing MCP2515...\n");
  }

  CAN0.setMode(MCP_NORMAL);  // Change to normal mode to allow messages to be transmitted
  pinMode(CAN0_INT, INPUT);  // RX: Configuring pin for /INT input
  attachInterrupt(digitalPinToInterrupt(2), empfangen, FALLING);

  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){
  blinkerLinks = !blinkerLinks;
  blinkerRechts = !blinkerRechts;
}

void empfangen() {
  if (CAN_MSGAVAIL == CAN0.checkReceive()) {
    long unsigned int canID;
    CAN0.readMsgBuf(&canID, &empf_len, input);
    if(canID == 315){
      msbGeschwindigkeit = input[0];
      lsbGeschwindigkeit = input[1];
      msbDrehzahl = input[2];
      lsbDrehzahl = input[3];
      rest = input[4];
    }
  }
}

void loop(){
  empfangen();

  byte bRechtsOut = 0;
  byte bLinksOut = 0;
  if((rest&0x02) == 0x02 && blinkerRechts){
    byte bRechtsOut = 0x02;
  }

  if((rest&0x04) == 0x04 && blinkerLinks){
    byte bLinksOut = 0x04;
  }

  byte restNeu = rest&0x4F;
  restNeu |= bRechtsOut | bLinksOut;
  
  Serial.write(msbGeschwindigkeit);
  Serial.write(lsbGeschwindigkeit);
  Serial.write(msbDrehzahl);
  Serial.write(lsbDrehzahl);
  Serial.write(restNeu);
}
