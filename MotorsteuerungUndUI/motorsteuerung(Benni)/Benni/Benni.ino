/*
    Gebrauchsanweisungen finden sie in der beiliegenden README.txt
    Projekt von Benjamin Ader & Robin Holzwarth - inf-1
*/

const byte gasInput = 200;
const byte hebelInput = 220;
const byte bremseInput = 250;

const unsigned long sendeID = 315;  //Port, auf welchem die Simulationsdaten ausgegeben werden

#include <mcp_can.h>
#include <SPI.h>
#include <AUTO.h>   //Eigenes Library zur Simulation des fahrverhaltens

volatile AUTO myAuto;
volatile byte myGas = 0;
volatile byte myBremse = 0;
volatile boolean myKupplung = true;
volatile byte myGang = 1;   //0 = Rückwärts
volatile byte myHebel = 0;  //P, R, N, D, S
volatile float myGeschwindigkeit = 0;
volatile float myDrehzahl = 0;

volatile byte ABS_Wert = 255;   //Variabler Wert der Bremse bei Schlupf (siehe bremsen)
volatile char zaehler = 0;      //Zählvariable für den Schaltprozess (siehe schalten)
volatile byte gasBuffer = 0;    //Buffer des alten Gas-Wertes während dem Schaltvorgang (siehe schalten)
byte senden = 0;

#define CAN0_INT 2
const byte SPI_CS_PIN = 10;
MCP_CAN CAN0(SPI_CS_PIN);
byte send_data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0, 0, 0};
byte empf_data[8];
byte empf_len;

void setup() {
  Serial.begin(115200);
  if (CAN0.begin(MCP_ANY, CAN_10KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!\n");
  }
  else Serial.println("Error Initializing MCP2515...\n");
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
  attachInterrupt(0, empfangen, FALLING);   //Interrupt-Routine bei Input über den CAN (siehe empfangen)

  myAuto.setGang(1);
  myAuto.setGas(0);
  myAuto.setBremse(0);
  myAuto.setKupplung(myKupplung);
  zuendung(1);    //Starten des Autos
}

void sendData() {
  byte sndStat = CAN0.sendMsgBuf(sendeID, 0, 5, send_data);
  switch (sndStat) {
    case CAN_FAILTX: Serial.println("Error: CAN_FAILTX ..."); break;
    case CAN_MSGAVAIL: Serial.println("Error: CAN_MSGAVAIL ..."); break;
    case CAN_NOMSG: Serial.println("Error: CAN_NOMSG ..."); break;
    case CAN_CTRLERROR: Serial.println("Error: CAN_CTRLERROR ..."); break;
    case CAN_GETTXBFTIMEOUT: Serial.println("Error: CAN_GETTXBFTIMEOUT ..."); break;
    //          case CAN_SENDMSGTIMEOUT: Serial.println("Error: CAN_SENDMSGTIMEOUT ..."); break;
    case CAN_FAIL: Serial.println("Error: CAN_FAIL ..."); break;
  }
}

void verpacken() {    //Verpacken der Fahrdaten für den Versand auf dem CAN (siehe README)
  unsigned int velMap = (int)(mapf(myGeschwindigkeit, 0.0, 320.0, 0.0, 65278.0));
  unsigned int rpmMap = (int)(mapf(myDrehzahl, 0.0, 8000.0, 0.0, 65278.0));
  send_data[0] = (velMap >> 8);
  send_data[1] = (velMap & 0xff);
  send_data[2] = (rpmMap >> 8);
  send_data[3] = (rpmMap & 0xff);
  byte kup = 0;
  byte cs = 0;
  if (myKupplung) kup = 1;
  if (myHebel == 4) cs = 1;
  send_data[4] = (myGang | (kup << 3) | (cs << 4));
}

void empfangen() {
  if (CAN_MSGAVAIL == CAN0.checkReceive()) {
    long unsigned int canID;
    CAN0.readMsgBuf(&canID, &empf_len, empf_data);
    switch (canID) {    //Verwerfen der nicht benötigten Daten
      case gasInput:
        gibIhm(empf_data[0]);
        break;
      case bremseInput:
        bremsen(empf_data[0]);
        break;
      case hebelInput:
        wahlhebel(empf_data[0]);
        break;
    }
  }
}

void wahlhebel(byte wert) {
  switch (wert) {
    case 0:   //P
      if (myGeschwindigkeit < 2) {    //Parken nur wenn Geschwindigkeit < 2 km/h, sonst Neutral
        myHebel = 0;
        myGang = 1;
        myBremse = 120;
      }
      else myHebel = 2;
      myKupplung = true;
      break;
    case 1:   //R
      if (myGeschwindigkeit < 5) {    //Rückwärtsgang nur wenn Geschwindigkeit < 5 km/h, sonst Neutral
        myHebel = 1;
        myGang = 0;
      }
      else myHebel = 2;
      break;
    case 2:   //N
      myHebel = 2;
      myKupplung = true;
      break;
    case 3:   //D
      if ((myHebel != 1) | (myGeschwindigkeit < 2)) {    //Drive nur wenn davor kein Rückwärtsgang oder Geschwindigkeit < 2 km/h, sonst Neutral
        myHebel = 3;
        if (myHebel < 2) {    //Nur falls davor geparkt oder Rückwärtsgang Gang = 1, sonst vorerst gleicher Gang
          myGang = 1;
        }
      }
      else myHebel = 2;
      break;
    case 4:   //S
      if ((myHebel != 1) | (myGeschwindigkeit < 2)) {    //Siehe Drive
        myHebel = 4;
        if (myHebel < 2) {    //Siehe Drive
          myGang = 1;
        }
      }
      else myHebel = 2;
      break;
  }
}

void zuendung(byte wert) {
  if (wert == 1) {    //Start
    myKupplung = true;
    myGas = 4;
    myAuto.startUp();
  }
  else {              //Stop
    if (myHebel == 0) {
      myGas = 0;
      myBremse = 0;
      myKupplung = true;
    }
  }
}

void bremsen(byte wert) {
  if (myHebel != 0) {   //Nur wenn nicht geparkt
    if (wert > 210) {   //ABS bei mehr als 80% Bremsdruck
      myBremse = ABS_Wert;
      if (ABS_Wert > 208) ABS_Wert--;
      else ABS_Wert = 255;
    }
    else myBremse = wert;
  }
}

void gibIhm(byte wert) {
  if ((myGang < 2) & (myKupplung) & (wert < 6)) myGas = 6;    //Standgas wenn ausgekuppelt
  else if ((myGang < 2) & (!myKupplung) & (wert < 6) & (myBremse < 2)) myGas = 6;   //Standgas wenn eingekuppelt aber keine Bremse
  else if ((myBremse > 1) & (!myKupplung)) myGas = 0 ;    //KEIN GAS WENN BREMSE!
  else myGas = wert;
}

void schalten() {
  if (!myKupplung) {
    if ((myHebel == 3)) {   //Comfortschaltung
      if ((myAuto.getDrehzahl() > myDrehzahl) & (myDrehzahl > 3950) & (myGang < 7)) {
        myKupplung = true;
        zaehler = 40;
        gasBuffer = myGas;
      }
      else if ((myAuto.getDrehzahl() < myDrehzahl) & (myDrehzahl < 2500) & (myGang > 1)) {
        myKupplung = true;
        zaehler = -30;
        gasBuffer = myGas;
      }
    }
    else if (myHebel == 4) {    //Sportschaltung
      if ((myAuto.getDrehzahl() > myDrehzahl) & (myDrehzahl > 6250) & (myGang < 7)) {
        myKupplung = true;
        zaehler = 36;
        gasBuffer = myGas;
      }
      else if ((myAuto.getDrehzahl() < myDrehzahl) & (myDrehzahl < 3600) & (myGang > 1)) {
        myKupplung = true;
        zaehler = -26;
        gasBuffer = myGas;
      }
    }
  }

  if (zaehler > 0) {        //Hochschaltprozess (über mehrere Simulationstakte)
    myGas = 0;
    zaehler--;
    if (zaehler == 0) {
      myGang++;
      if (myHebel != 2) myKupplung = false;   //Nur falls nicht Neutral
      myGas = gasBuffer;
    }
  }
  else if (zaehler < 0) {   //Runterschaltprozess
    myGas = 0;
    zaehler++;
    if (zaehler == 0) {
      myGang--;
      if (myHebel != 2) myKupplung = false;   //Nur falls nicht Neutral
      myGas = gasBuffer;
    }
  }
}

void standgas() {
  if ((myHebel != 0) & (myHebel != 2)) {                                            //Weder Parken noch Neutral, sonst Kupplung
    if ((myGang < 2) & (myBremse > 1) & (myDrehzahl < 950)) myKupplung = true;      //Auskuppeln weil Drehzahl zu niedrig
    if ((myGang < 2) & (myBremse < 2)) myKupplung = false;                          //Einkuppeln weil (Stand-)Gas
    if ((myGang < 2) & (myDrehzahl < 950) & (myGas < 6)) myGas = 6;                 //Standgas
  }
  else {
    myKupplung = true;
  }
}


void loop() {
  standgas();                       //Verarbeitung

  myAuto.setGas(myGas);             //Eingabe
  myAuto.setKupplung(myKupplung);
  myAuto.setBremse(myBremse);
  myAuto.setGang(myGang);

  myAuto.update(100);               //Ausführung

  schalten();
  if (myAuto.getDrehzahl() > 8000) myAuto.begrenzer();    //Begrenzer, falls die Maximaldrehzahl von 8000 Umdreh./Minute erreicht ist

  myGeschwindigkeit = myAuto.getGeschwindigkeit();
  myDrehzahl = myAuto.getDrehzahl();

  senden += 1;
  if (senden % 4 == 0) {
    verpacken();  //Ausgabe
    sendData();
  }

  delayMicroseconds(5008);   //Warten vor nächstem Update (Mittlere Arbeitszeit für eine Iteration über TIMER1 ausgemessen)

  Serial.print(myDrehzahl);
  Serial.print(" || ");
  Serial.print(myGeschwindigkeit);
  Serial.print(" || ");
  Serial.print(myGang);
  Serial.print(" || ");
  Serial.print(myKupplung);
  Serial.print(" || ");
  Serial.println(myHebel);
}

float mapf(float val, float in_min, float in_max, float out_min, float out_max) {   //Eigene Map-Funktion für Float-Werte (genauer)
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
