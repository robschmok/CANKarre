#ifndef AUTO_H
#define AUTO_H

#include <Arduino.h>

class AUTO {
  private:
          float beschleunigung;
          float geschwindigkeit;
          float hubraum;
          float gaenge[8];
          byte gang;
          byte gas;
          byte bremse;
          boolean kupplung;
          float druck;
          float leistung;
          float mDrehmoment;
          float rDrehmoment;
          float kraft;
          float rDrehzahl;
          float mDrehzahl;
          short masse;
          float radius;

          void mleistung();
          void mdrehmoment();
          void rdrehmoment();
          void calKraft();
          void calBeschleunigung();
          void calGeschwindigkeit (short);
          void rdrehzahl();
          void mdrehzahl();
          void mdruck();
  public:
          AUTO();
          void update(short);
          byte getGang();
          float getDrehzahl();
          float getGeschwindigkeit();
          void setGas(byte);
          void setGang(byte);
          void setBremse(byte);
          void setKupplung(boolean);
          void startUp();
          void begrenzer();
};

#endif
