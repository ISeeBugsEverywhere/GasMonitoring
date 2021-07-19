#include <Arduino.h>
#include <Adafruit_Sensor.h>
//drėgmės ir temperatūros sensorius
// #include "Barometer.h"
#include <DHT.h>
#include <DHT_U.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include "consts.h"
//=== pins:
#ifndef MAIN_H
#define MAIN_H


//voids:
double getPressure();
void dht_report();
//=== VOIDS:
//void send_T_value();
void get_cmd();
void send_sensor_value();
void gass_over_limit();
void control_gass_flow(int& SENSOR);
void control_o2_flow(float& );
void get_voltage();
void run_init_gas();
void open_gas_time_mode();
//===END OF VOIDS

//===VARIABLES


DHT_Unified dht(DHTPIN9, DHTTYPE11);
DHT_Unified dht1(DHTPIN10, DHTTYPE22);

//pressure object:
SFE_BMP180 pressure;
char cmd_recorded [20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // cmd for control of Arduino
int ii = 0;
bool data_received  = false;
int rise_value = 0;
int quick_rise = 0;
int gass_keep = 0;
int gass_threshold_p1 = 0;
float p1 = 0;
float p2 = 0;
int gass_threshold_p2 = 0;
unsigned int delay_s1 = 0;
unsigned int delay_s2 = 0;
unsigned int open_s1 = 0;
unsigned int open_s2 = 0;
//For algorythm C:
unsigned long init_t = 0ul;
unsigned int open_t = 0;
unsigned int close_t = 0;
bool USE_C = false;
bool INIT_GAS = true;
unsigned long gas_t = 0;
int SENSOR = 0;
float O2_SENSOR = 0.0;
bool RISE = false;
bool WATCH = false;
bool CLOSED = true;
bool WAIT_1 = false;
bool WAIT_2 = false;
unsigned long t_report = 0;//millis
unsigned long t_report1 = 0;//millis
unsigned long t_report_TH = 0; //mills
unsigned long t_watch = 0;
unsigned long t_info = 2048;// was 1024;
unsigned long t_info1 = 2048; //was 1536;
char algoritmas;
bool dht_reported = false;

//O2 sensing:
bool USE_O2 = false;
float keep_o2 = 0.0;
unsigned long t_o2_watch = 200; //milliseconds, hardcoded, need to control over a PC!
unsigned long t_o2_wait = 1500;
float o2_threshold = 0.99;
float o2_th = 0.0;
bool RECALIBRATE = false;


//===END OF VARIABLES

#endif
