#include <Arduino.h>
#ifndef consts_h
#define consts_h
#define sensorpin  A7
#define PIN_RELAY  12
#define LED_WATCH  2
#define LED_SHUTTER  3
#define LED_RISE  4
#define LED_STAGE1  5
#define LED_STAGE2  6
#define LED_STAGE3  7
#define VOLTAGE_PIN  A0
//const int O2_PIN = A1;

//pinout:
#define DHTTYPE11    DHT11
#define DHTPIN9 9 //D9 digital, 9th pin for DHT11
#define DHTTYPE22    DHT22
#define DHTPIN10 10 //D10 digital, 10th pin for DHT22

// for DHT errors:
#define TERR -17.7
#define HERR 99
#endif