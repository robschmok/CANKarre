#include "AUTO.h"

AUTO::AUTO() {
  beschleunigung = 0.0;
  geschwindigkeit = 0.0;
  hubraum = 2.981;    //Liter
  gaenge[0] = 12.7445;
  gaenge[1] = 14.0369;
  gaenge[2] = 8.221;
  gaenge[3] = 5.6722;
  gaenge[4] = 4.3262;
  gaenge[5] = 3.3746;
  gaenge[6] = 2.8361;
  gaenge[7] = 2.2258;
  kupplung = false;
  gas = 0;
  bremse = 0;
  mDrehzahl = 0.0;
  rDrehzahl = 0.0;
  mDrehmoment = 0.0;
  rDrehmoment = 0.0;
  druck = 1200000.0;
  masse = 1510;   //Kilogramm
  radius = 0.254;   //Meter
}


void AUTO::update(short refresh) {
  if (!kupplung) {    //Falls keine Kupplung
    mleistung();
    mdrehmoment();
    rdrehmoment();
  }
  else {              //Falls Kupplung
    mleistung();
    mdrehmoment();
    mDrehmoment -= 0.004485 * mDrehzahl;
    mDrehzahl += mDrehmoment / (0.05024 * refresh);
    if (mDrehzahl > 8000) mDrehzahl = 8000;
    rDrehmoment = 0;
  }
  calKraft();
  calBeschleunigung();
  calGeschwindigkeit(refresh);
  rdrehzahl();
  if (!kupplung) mdrehzahl();
  mdruck();
  if (geschwindigkeit < 0) {    //Wenn zum Stillstand abgebremst wurde
    geschwindigkeit = 0;
    bremse = 0;
  }
}

float AUTO::getDrehzahl() {
  return mDrehzahl;
}

float AUTO::getGeschwindigkeit() {
  return geschwindigkeit * 3.6;
}

byte AUTO::getGang() {
  return gang;
}

void AUTO::setGas(byte wert) {
  gas = wert;
}

void AUTO::setBremse(byte wert) {
  bremse = wert;
}

void AUTO::setGang(byte wert) {
  gang = wert;
}

void AUTO::setKupplung(boolean wert) {
  kupplung = wert;
}

void AUTO::startUp() {
  mDrehzahl = 1000;
}

void AUTO::begrenzer() {
  mDrehzahl = 8000;
}



void AUTO::mdruck () {
  if (mDrehzahl > 5999) druck = -0.15 * ((mDrehzahl - 6000) * (mDrehzahl - 6000)) + 1200000.0;
  else druck = 1200000.0;
}

void AUTO::mleistung () {
  leistung = ((float)gas / 255) * 0.6 * hubraum * druck * mDrehzahl / 72000;
}

void AUTO::mdrehmoment () {
  mDrehmoment = leistung / (6.28 * (mDrehzahl / 60));
}

void AUTO::rdrehmoment () {
  rDrehmoment = mDrehmoment * gaenge[gang];
}

void AUTO::calKraft () {
  kraft = (3.937 * rDrehmoment) - (0.348 * geschwindigkeit * geschwindigkeit) - (((float)bremse/255) * masse * 11) - (0.14715 * masse);  //Luft-, Brems- und Rollwiderstand
}

void AUTO::calBeschleunigung () {
   beschleunigung = kraft / masse;
}

void AUTO::calGeschwindigkeit (short refresh) {
   geschwindigkeit += beschleunigung / refresh;
}

void AUTO::rdrehzahl () {
  rDrehzahl = geschwindigkeit * 37.59566;
}

void AUTO::mdrehzahl () {
  mDrehzahl = rDrehzahl * gaenge[gang];
}
