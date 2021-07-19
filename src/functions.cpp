#include <Arduino.h>
#include "consts.h"
void stp()
{
    // put your setup code here, to run once:
    // Serial.begin(9600);
    delay(1000);
    //pin conf:
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(LED_WATCH, OUTPUT);//WATCH
    pinMode(LED_SHUTTER, OUTPUT);//SHUTTER
    pinMode(LED_RISE, OUTPUT);//RISE
    pinMode(LED_STAGE1, OUTPUT);//stage 1
    pinMode(LED_STAGE2, OUTPUT);//stage 2
    pinMode(LED_STAGE3, OUTPUT);//stage 3
    // sensor_t sensor;
    digitalWrite(LED_STAGE1, HIGH);
    digitalWrite(LED_STAGE2, HIGH);
    digitalWrite(LED_STAGE3, HIGH);
    digitalWrite(LED_WATCH, HIGH);
    digitalWrite(LED_SHUTTER, HIGH);
    digitalWrite(LED_RISE, HIGH);
    //analog A0:
    pinMode(A0, INPUT);  
    Serial.println(F("REM:===STARTED===:REM:!"));
    delay(1000);
    Serial.println(F("REM:V.0.4.AW:REM:!"));
    delay(3000);
    digitalWrite(LED_STAGE1, LOW);
}

void end()
{
    digitalWrite(LED_STAGE1, LOW);
    digitalWrite(LED_STAGE2, LOW);
    digitalWrite(LED_STAGE3, LOW);
    digitalWrite(LED_WATCH, LOW);
    digitalWrite(LED_SHUTTER, LOW);
    digitalWrite(LED_RISE, LOW);
}

void open_gass_shutter(bool& CLOSED)
{
  if (CLOSED){
    digitalWrite(PIN_RELAY, HIGH);
    //digitalWrite(23, HIGH);
    CLOSED = false;
    digitalWrite(LED_SHUTTER, HIGH);
  }
}
void close_gass_shutter(bool& CLOSED)
{
  if(!CLOSED){
    digitalWrite(PIN_RELAY, LOW);
    //digitalWrite(23, LOW);
    CLOSED = true;
    digitalWrite(LED_SHUTTER, LOW);
  }
}

void  open_shutter(bool& CLOSED) {
    /* code */
    if(CLOSED)
    {
        open_gass_shutter(CLOSED);
        Serial.println(F("REM:OPENED:REM:!"));
    }
    else
    {
        close_gass_shutter(CLOSED);
        Serial.println(F("REM:CLOSED:REM:!"));
    }
}

void check_shutter(const bool& CLOSED)
{
  //just blinks LEDs:
  if (!CLOSED)
  {
    digitalWrite(LED_SHUTTER, HIGH);
  }
  else
  {
    digitalWrite(LED_SHUTTER, LOW);
  }
}

void watch()
{
    digitalWrite(LED_WATCH, HIGH);
    digitalWrite(LED_RISE, LOW);
    digitalWrite(LED_STAGE1, LOW);
    digitalWrite(LED_STAGE2, LOW);
    digitalWrite(LED_STAGE3, LOW);
}

void low()
{
    digitalWrite(LED_WATCH, LOW);
    digitalWrite(LED_RISE, LOW);
    digitalWrite(LED_STAGE1, LOW);
    digitalWrite(LED_STAGE2, LOW);
    digitalWrite(LED_STAGE3, LOW);
}

void problems()
{
  //inform about problems:
  low();
  digitalWrite(LED_SHUTTER, LOW);
  digitalWrite(LED_WATCH, HIGH);
  digitalWrite(LED_STAGE3, HIGH);
  digitalWrite(LED_RISE, HIGH);
  delay(200);
  digitalWrite(LED_SHUTTER, HIGH);
  digitalWrite(LED_WATCH, LOW);
  digitalWrite(LED_STAGE3, LOW);
  digitalWrite(LED_RISE, LOW);
}

void overflow()
{
    digitalWrite(LED_STAGE2, HIGH);
    digitalWrite(LED_STAGE3, HIGH);
}

void Serialprintln(const int& a)
{
    Serial.println("REM: "+(String)a+":REM:!");
}
void Serialprintln(const unsigned int& a)
{
    Serial.println("REM: "+(String)a+":REM:!");
}
void Serialprintln(const String& a)
{
    Serial.println("REM:"+a+":REM:!");
}
void Serialprintln(const float& a)
{
    Serial.println("REM: "+(String)a+":REM:!");
}

void Serialprintln(const char* a)
{
  Serial.println("REM:"+(String)a+":REM:!");
}
void Serialprintln(const unsigned long& a)
{
    Serial.println("REM: "+(String)a+":REM:!");
}

void Serialprintln(const char& a)
{
  Serial.println("REM:"+(String)a+":REM:!");
}
