#include "Oxygen.h"
#include <Arduino.h>
#include <EEPROM.h>
#define O2_preheat_time	60000		//Warm-up time 1 minute
bool O2_init_flag = 0;					//1为准备好了
float calibration_voltage;	//Calibrated voltage
#define O2_percentage 20.80


float O2_value(bool& recalibrate)		//Get O2 data
{
	unsigned int sum = 0;
    if (recalibrate)
    {
        recalibrate = false;
        O2_init_flag = 0;
    }
	if (O2_init_flag == 0)
	{
		O2_init_flag = 1;					//Power-on calibration once
		pinMode(O2_PIN, INPUT);
		for (unsigned char i = 64;i > 0;i--)
		{
			sum = sum + analogRead(O2_PIN);
			delay(100);
		}
		sum = sum >> 6;//right shift by 6 positions (why?) //division by 2^5 (=64, divided by 64)
		calibration_voltage = sum / O2_percentage;//Obtain the correction factor
		Serial.println(F("REM:calibration voltage:REM:!"));
		Serial.println("REM:"+(String)calibration_voltage+":REM:!");
		return O2_percentage;						//Calibration succeeded
	}
	else
	{
		for (unsigned char i = 32;i > 0;i--)
		{
			sum = sum + analogRead(O2_PIN);
			delay(1);
		}
		sum = sum >> 5; //right shift by 5 positions (why?) //division by 2^5 (=32, divided by 32)
		float output = (float)sum / calibration_voltage;
		return output;
	}
}

