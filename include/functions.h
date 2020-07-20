#include <Arduino.h>
#ifndef functions_h
#define functions_h
void stp();
void end();
void watch();
void low();
void overflow();
void check_shutter(const bool& CLOSED);
void open_shutter(bool& CLOSED);
void close_gass_shutter(bool& CLOSED);
void open_gass_shutter(bool& CLOSED);
void problems();
void Serialprintln(const int& a);
void Serialprintln(const unsigned int& a);
void Serialprintln(const String& a);
void Serialprintln(const float& a);
void Serialprintln(const char* a);
void Serialprintln(const char& a);
void Serialprintln(const unsigned long& a);
// void Serialprintln(string& a);
#endif