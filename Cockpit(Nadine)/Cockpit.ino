/*
  brems und gaswert mit Joystick an analoge Pins 0 und 1
  zuendung mit Taster an Pin 6
  blinker an drei Tastern an Pins 3(links),4(Warnblincker) und 5(rechts) und Restlichter: Poti
  schalthebel an an Poti (P,R,N,D,S)
*/

#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
const byte SPI_CS_PIN = 10;
MCP_CAN CAN0(SPI_CS_PIN);                  // Set CS to pin 10

byte send_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte empf_data[8];
byte empf_len;
int gasID = 200;
int zuendID = 210;
int schaltID = 220;
int bremsID = 250;
int blinkID = 300;
int lichtID = 320;
byte gaswert;
byte bremswert;
byte lblink = 0;
byte wblink = 0;
byte rblink = 0;
byte lichter;
byte schalt;
byte gneu = 0;
byte bneu = 0;
byte lneu = 0;
byte wneu = 0;
byte rneu = 0;
byte lichtneu = 0;
byte sneu = 0;
byte zuendung;
byte i = 0;
byte j = 0;
byte stopp = 1;
byte x;                         //Für Lichter
byte s;                         //Für den Schalter (Hebel)
byte n = 0;                     //Zählvariable für den linken Blinker
byte m = 0;                     //Zählvariable für den rechten Blinker
byte b = 0;                     //Zählvariable für den Warnblinker

void setup() {
  Serial.begin(115200);
  if (CAN0.begin(MCP_ANY, CAN_10KBPS, MCP_16MHZ) == CAN_OK) {
     Serial.println("MCP2515 Initialized Successfully!\n");
  } else Serial.println("Error Initializing MCP2515...\n");
  
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
  pinMode(3, INPUT_PULLUP);                  //Taster für links Blinker
  pinMode(4, INPUT_PULLUP);                  //Taster für Warnblinker
  pinMode(5, INPUT_PULLUP);                  //Taster für rechter Blinker
  pinMode(6, INPUT_PULLUP);                  //Taster für Zuendung

}

void sendData(unsigned long id, byte mode, byte len) {
  byte sndStat = CAN0.sendMsgBuf(id, mode, len, send_data);
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

void zuendungAn() {
  if (j == 0) {                             //j damit nur bei Zuendungs-Änderung Daten gesendet werden
    send_data[0] = 1;                       //1 weil zuendung an
    sendData(zuendID, 0, 1);
    j = !j;
  }
}

void alteWerte() {                          //Werte werden zugeordnet, neue Werte werden zu alten Werten
  gaswert = gneu ;
  bremswert = bneu ;
  lichter = lichtneu;
  lblink = lneu;
  wblink = wneu;
  rblink = rneu;
}

void neueWerte() {                          //Werte werden neu ausgelesen
  gneu = map(analogRead(A0), 0, 1023, 0, 255);
  bneu = map(analogRead(A1), 0, 1023, 0, 255);
  lichtneu = map(analogRead(A2), 0, 1023, 0, 255);
  lneu = digitalRead(3);
  wneu = digitalRead(4);
  rneu = digitalRead(5);
}

void gasDaten() {
  if (gaswert != gneu) {                    //bei Gaswertänderung neuen Gaswert senden
    send_data[0] = gneu;
    sendData(gasID, 0, 1);
  }
}

void bremsDaten() {
  if (bremswert != bneu) {                  //bei Bremswertänderung neuen Bremswert senden
    send_data[0] = bneu;
    sendData(bremsID, 0, 1);
  }
}


void linksblinkerDaten() {
  if (lblink != lneu) {                       //bei Blinkeränderung
    m = 0;
    b = 0;
    if (n == 0) {                          //an
      //  Serial.println("0b00000010");
      send_data[0] = 0b00000010;
      sendData(blinkID, 0, 1);
    }
    else {
      if (n % 4 == 2) {                     //aus
        //    Serial.println("0b00000000");
        send_data[0] = 0b00000000;
        sendData(blinkID, 0, 1);
      }
    }
    n++;
    if (n == 4) {                               //zählt immer von 0 bis 3
      n = 0;
    }
  }
}

void warnblinkerDaten() {
  if (wblink != wneu) {
    n = 0;
    m = 0;
    if (b == 0) {
      //  Serial.println("0b00000011");
      send_data[0] = 0b00000011;
      sendData(blinkID, 0, 1);
    }
    else {
      if (b % 4 == 2) {
        //   Serial.println("0b00000000");
        send_data[0] = 0b00000000;
        sendData(blinkID, 0, 1);
      }
    }
    b++;
    if (b == 4) {
      b = 0;
    }
  }
}

void rechtsblinkerDaten() {
  if (rblink != rneu) {
    n = 0;
    b = 0;
    if (m == 0) {
      // Serial.println("0b00000001");
      send_data[0] = 0b00000001;
      sendData(blinkID, 0, 1);
    }
    else {
      if (m % 4 == 2) {
        // Serial.println("0b00000000");
        send_data[0] = 0b00000000;
        sendData(blinkID, 0, 1);
      }
    }
    m++;
    if (m == 4) {
      m = 0;
    }
  }
}

void lichtDaten() {
  if (lichter != lichtneu) {                  //bei Lichtschalterstellungsänderung neu Stellung senden
    x = map(lichtneu, 0, 255, 0, 5);        //Lichtschalter mit 6 Positionen (mit Nebelschlussleuchte und Nebelscheinwerfer als Drehposition)
    send_data[0] = x;
    sendData(lichtID, 0, 1);
  }
}

void schaltDaten() {
  if (schalt != sneu) {                  //bei Lichtschalterstellungsänderung neu Stellung senden
    s = map(sneu, 0, 255, 0, 4);
    send_data[0] = s;
    sendData(schaltID, 0, 1);
  }
}

void datenFlut() {                          //Daten werden bei Änderung gesendet (Blinker, Gaswert, Bremswert, Hebelstellung, ...)
  alteWerte();
  delay(10);
  neueWerte();
  schaltDaten();
  if (s != 0) {
    if (s != 2) {                           //Gasdaten nicht versenden, wenn FZ parkt(P) oder Leergang(N) drin ist
      gasDaten();
      bremsDaten();
    }
  }
  lichtDaten();
  if (i == 19) {                            // fünf mal pro sek (für Blinker)
    i = 0;
    linksblinkerDaten();
    warnblinkerDaten();
    rechtsblinkerDaten();
  }
  i++;
}

bool zuendungStop() {
  if (!digitalRead(6)) {                     //Zuendung aus
    stopp = !stopp;                        //keine Daten mehr schicken = stopp = 1
    j = 0;                                 //
    i = 0; // reset
    return false;
  }
  return true;
}

void zuendungAus() {
  if (j == 0) {                              //damit nur bei Änderung Daten gesendet werden
    send_data[0] = 0;                        //Zündung ist aus
    sendData(zuendID, 0, 1);
    j = !j;
  }
}

void zuendungBereit() {                      //Zündung an, Daten los
  if (digitalRead(6)) {                      //Datenaussendung wird mit Zuendung (Taste) gestartet
    stopp = !stopp;                          //stopp wird aufgehoben
    j = 0;
  }
}

void loop() {
  while (!stopp) {                           //solange wie Zündung an ist
    zuendungAn();                            //ein mal schicken, dass Zündung an
    datenFlut();                             //Daten schicken
    if (!zuendungStop()) break;             //schauen ob Zündung ausgeschaltet ist, wenn ja keine Daten mehr schicken = raus aus der Schleife
  }
  zuendungAus();                             //ein mal schicken, dass Zündung aus
  zuendungBereit();                          //Zündung wartet auf Neustart
}
