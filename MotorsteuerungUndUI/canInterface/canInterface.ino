#include <mcp_can.h>
#include <SPI.h>

volatile byte msbGeschwindigkeit = 0;
volatile byte lsbGeschwindigkeit = 0;
volatile byte msbDrehzahl = 0;
volatile byte lsbDrehzahl = 0;
volatile byte rest = 0;

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
  Serial.write(msbGeschwindigkeit);
  Serial.write(lsbGeschwindigkeit);
  Serial.write(msbDrehzahl);
  Serial.write(lsbDrehzahl);
  Serial.write(rest);
}
