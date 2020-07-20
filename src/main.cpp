#include <Arduino.h>
//#include "Timer.h"
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
//drėgmės ir temperatūros sensorius
// #include "Barometer.h"
#include <DHT.h>
#include <DHT_U.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include "main.h"
#include "Oxygen.h"
#include "functions.h"
#include "consts.h"

void setup() {
    Serial.begin(9600);
    delay(3000);
    stp();
    //dht.begin();
    dht.begin();
    dht1.begin(); //start the sensors
    //start O2 sensor:
    float o = O2_value(RECALIBRATE); //6400 ms!
    Serialprintln(o);
    if (pressure.begin())
      Serial.println(F("REM: BMP180 init success:REM:!"));
    else
    {
      Serial.println(F("REM: BMP180 init fail (disconnected?):REM:!\n\n"));
      while (1)
      {
          problems();
      }  ; // Pause forever.
    }
    end();
}

void loop() {
    // put your main code here, to run repeatedly:
    //Serial.println("LOOP");
    if (Serial.available() > 0)
    {
        delay(20); // kad greičiau būtų,
        for (ii=0; ii<20; ii++){
        cmd_recorded[ii]=Serial.read();
        }
        // digitalWrite(LED_STAGE1, HIGH);
        data_received = true;
    }
    dht_report();
    //send_T_value();
    get_cmd();
    SENSOR = analogRead(sensorpin);
    O2_SENSOR = O2_value(RECALIBRATE);
    send_sensor_value();
    if (!USE_C)
    {
      if (algoritmas == 'A')
      {
          gass_over_limit();
      }
      else
      {
          RISE = false;
      }
      control_gass_flow(SENSOR);
      check_shutter(CLOSED);
    }
    if (USE_C && WATCH)
    {
      //f-ns in c section:
      run_init_gas();
      open_gas_time_mode();
    }
    if (USE_O2 && WATCH)
    {
        //o2 fork:
        RISE = false;
        control_o2_flow(O2_SENSOR);
        check_shutter(CLOSED);
    }
}

void run_init_gas()
{
    if (INIT_GAS)
    {
      gas_t = millis();
      while (millis()-gas_t <= init_t) {
          /* code */
          if(CLOSED)
          {
              open_gass_shutter(CLOSED);
          }
          send_sensor_value();
      }
      INIT_GAS = false;
    }
}

void open_gas_time_mode()
{
  if (!INIT_GAS)
  {
    gas_t = millis();
    while (millis()-gas_t <= open_t) {
        /* code */
        if(CLOSED)
        {
            open_gass_shutter(CLOSED);
        }
        send_sensor_value();
    }
    gas_t = millis();
    while (millis()-gas_t <= close_t) {
        /* code */
        if(!CLOSED)
        {
            close_gass_shutter(CLOSED);
        }
        send_sensor_value();
    }
  }
}

void get_cmd(/* arguments */) {
    /* code */
    if (data_received) {
        /* code */
        data_received = false;
        if (cmd_recorded[0] == 'p')
        {
            Serial.println(F("REM:PROGRAM START:REM:!"));
            Serial.println(cmd_recorded);
            RISE = true;
            USE_C = false;
            //value to keep:
            gass_keep = (cmd_recorded[1]-48)*100 + (cmd_recorded[2]-48)*10+(cmd_recorded[3]-48);
            p1 = (cmd_recorded[4]-48)*10+(cmd_recorded[5]-48);
            p2 = (cmd_recorded[6]-48)*10+(cmd_recorded[7]-48);
            gass_threshold_p1 = (int)((float)gass_keep*(p1/100.0));
            gass_threshold_p2 = (int)((float)gass_keep*(p2/100.0));
            //DELAY TIMES ARE IN MILLISECONDS, PASSED 0.1 SECONDS
            delay_s1 = 100*((cmd_recorded[8]-48)*10+(cmd_recorded[9]-48));
            delay_s2 = 100*((cmd_recorded[10]-48)*10+(cmd_recorded[11]-48));
            open_s1 = 100*((cmd_recorded[12]-48)*10+(cmd_recorded[13]-48));
            open_s2 = 100*((cmd_recorded[14]-48)*10+(cmd_recorded[15]-48));
            quick_rise = (cmd_recorded[16]-48)*100 + (cmd_recorded[17]-48)*10+(cmd_recorded[18]-48);
            algoritmas = cmd_recorded[19];
            Serial.println(F("REM:KEEP THIS:REM:!"));
            Serialprintln(gass_keep);
            Serial.println(F("REM:AT PRC 1:REM:!"));
            Serialprintln(gass_threshold_p1);
            Serial.println(F("REM:AT PRC 2:REM:!"));
            Serialprintln(gass_threshold_p2);
            Serial.println(F("REM:WAITS d_S1, d_S2, Os1, Os2:REM:!"));
            Serialprintln(delay_s1);
            Serialprintln(delay_s2);
            Serialprintln(open_s1);
            Serialprintln(open_s2);
            Serial.println(F("REM:PROGRAM END:REM:!"));
            for (ii=0; ii<20; ii++){
            cmd_recorded[ii]=0;
            }

        }
        if ((cmd_recorded[0] == 'h') && (cmd_recorded[1] == 'c'))
        {
            //C kodas in use:
            Serial.println(F("REM:C mode: timers:REM:!"));
            USE_C = true;
            init_t = (100ul*(cmd_recorded[2]-48)+10ul*(cmd_recorded[3]-48)+(cmd_recorded[4]-48))*1000ul;//millisecs?
            open_t = 100*((cmd_recorded[5]-48)*10+(cmd_recorded[6]-48));//millisecs
            close_t = 100*((cmd_recorded[7]-48)*10+(cmd_recorded[8]-48));//millisecs
            Serial.println(F("REM:C fork:REM:!"));
            Serialprintln(init_t);
            Serialprintln(open_t);
            Serialprintln(close_t);
            Serialprintln(cmd_recorded[2]);
            Serialprintln(cmd_recorded[3]);
            Serialprintln(cmd_recorded[4]);
            for (ii=0; ii<20; ii++){
            cmd_recorded[ii]=0;
            }
        }
        if (cmd_recorded[0] == 'b')
        {
            Serial.println(F("REM:MONITORING ENABLED:REM:!"));
            //RISE = true;
            WATCH = true;
            digitalWrite(LED_WATCH, HIGH);
            check_shutter(CLOSED);
            open_gass_shutter(CLOSED);
            //CLOSED = false; //because of;
            for (ii=0; ii<20; ii++){
            cmd_recorded[ii]=0;
            }
        }
        if (cmd_recorded[0] == 's')
        {
            Serial.println(F("REM:MONITORING STOPPED:REM:!"));
            WATCH = false;
            digitalWrite(LED_WATCH, LOW);
            //digitalWrite(37, LOW);
            //digitalWrite(35, LOW);
            close_gass_shutter(CLOSED);
            for (ii=0; ii<20; ii++){
            cmd_recorded[ii]=0;
            }
        }
        if (cmd_recorded[0] == 't')
        {
            open_shutter(CLOSED);
            for (ii=0; ii<20; ii++){
            cmd_recorded[ii]=0;
            }
        }
        if (cmd_recorded[0] == 'a')
        {
          //read and return all PDHT values at once:
          dht_reported = false;
          dht_report();
          for (ii=0; ii<20; ii++){
          cmd_recorded[ii]=0;
          }
        }
        if (cmd_recorded[0] == 'v')
        {
          //read and return currently attached voltage:
          get_voltage();
          for (ii=0; ii<20; ii++){
          cmd_recorded[ii]=0;
          }
        }
        if (cmd_recorded[0]== 'o' && ((cmd_recorded[1]-48) == 2))
        {
            //o2 section:
            Serial.println(F("REM:O2 CASE:REM:!"));
            keep_o2 = (cmd_recorded[2]-48)*10.0+(cmd_recorded[3]-48)*1.0+(cmd_recorded[4]-48)*0.1+(cmd_recorded[5]-48)*0.01; //%
            o2_threshold = ((cmd_recorded[6]-48)*10.0+(cmd_recorded[7]-48)*1.0)/100.0; //between 0 and 1.0;
            o2_th = (2.0-o2_threshold) * keep_o2;
            if (cmd_recorded[8] == '!')
            {
                Serial.println(F("REM:O2 OK, KEEP O2 AT:REM:!"));
                Serialprintln(keep_o2);
                Serial.println(F("REM:Threshold:REM:!"));
                Serialprintln(o2_threshold);
                Serialprintln(o2_th);
                Serial.println(F("REM:END:REM:!"));
                USE_O2 = true;
            }
            else
            {
                Serial.println(F("REM:OOPS, O2 FORK FAILED!!!:REM:!"));
                Serial.println(keep_o2);
                Serial.println(o2_threshold);
                Serialprintln(o2_th);
                Serial.println(cmd_recorded[9]);
                problems();
                while (1);                
            }                        
            for (ii=0; ii<20; ii++){
              cmd_recorded[ii]=0;
            }
        }
        if (cmd_recorded[0] == 'c' && cmd_recorded[1] == '!')
        {
            RECALIBRATE = true;
            O2_value(RECALIBRATE);
            Serial.println(F("REM:RECALIBRATED:REM:!"));
            for (ii=0; ii<20; ii++){
              cmd_recorded[ii]=0;
            }
        }
    }

}

void get_voltage()
{
    int v = analogRead(VOLTAGE_PIN);
    double Vin = (float)v*5.0/1023.0*1000.0; //return in kV!
    Serial.println("V:"+String(Vin)+":kV:!");
}

double getPressure()
{
  char status;
  double T,P;

  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    // Serial.println(byte(status));
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        // Serial.println(byte(status));
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("REM: error retrieving pressure measurement :REM:!");
        return(-1);
      }
      else Serial.println("REM: error starting pressure measurement :REM:!");
      return(-1);
    }
    else Serial.println("REM: error retrieving temperature measurement :REM:!");
    return(-1);
  }
  else Serial.println("REM: error starting temperature measurement :REM:!");
  return(-1);
}

void dht_report()
{
  if (!dht_reported)
  {
    String sens = "DHT:";
    // put your main code here, to run repeatedly:
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("REM:Error reading temperature!:REM:!"));
      while (1)
      {
          problems();
      }  
    }
    else {
       //Serial.print(F("Temperature: "));
       //Serial.print(event.temperature);
       //Serial.println(F("°C"));
      sens = sens +String(event.temperature)+"|";
    }

    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("REM:Error reading humidity!:REM:!"));
      while (1)
      {
          problems();
      }  
    }
    else {
       //Serial.print(F("Humidity: "));
       //Serial.print(event.relative_humidity);
       //Serial.println(F("%"));
      sens = sens + "H"+String(event.relative_humidity)+"|";
    }
    dht1.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("REM:Error reading humidity!:REM:!"));
      while (1)
      {
          problems();
      }      
      
    }
    else {
      sens = sens +String(event.relative_humidity)+"|";
    }
    float p = getPressure();
     //Serial.println("P:");
     //Serial.println(p);
    sens = sens+String(p);

    Serial.println(sens+":DHT:!");
    dht_reported = true;
  }
}

void send_sensor_value()
{
    if((millis() - t_report) > t_info)
    {
        // duju ir vidaus temperaturos duomenis sudedam i viena string ir siunciam kad patogiau jie kartu
        //taip pat pridedam O2 koncentracijos matavimus
        //FORMA: GAS:xxx;yy.yy;O2:zz.zz:!
        String T="GAS:";
        //float O2 = O2_value();
        String O = "O2:"+(String)O2_SENSOR+":O2:!";
        sensors_event_t event1;
        dht1.temperature().getEvent(&event1);
        if (isnan(event1.temperature)) {
          Serial.println(F("REM: Error reading temperature! :REM:!"));
          problems();
          T = T + (String)SENSOR+";"+String(999);
        }
        else {
          T = T + (String)SENSOR+ ";" + String(event1.temperature);
        }

        Serial.println(T+";"+O);
        t_report = millis();
    }
}
/*

*/
void gass_over_limit()
{
  /* At the begining the gass value in chamber must be higher than the gass_keep value  */
  if (WATCH && RISE)
  {
      digitalWrite(LED_RISE, HIGH);
      if (quick_rise > 0)
      {
        rise_value = quick_rise;
      }
      else
      {
        rise_value = gass_keep - 20;
      }

      if((SENSOR < rise_value)) //problem persists here;
      //What kind of a problem?
      {
          /*JUST OPEN A SHUTTER AND KEEP IT OPENED*/
          open_gass_shutter(CLOSED);
          send_sensor_value();
          digitalWrite(LED_SHUTTER, HIGH);
          digitalWrite(LED_RISE, HIGH);
          digitalWrite(LED_STAGE1, LOW);
          digitalWrite(LED_STAGE2, LOW);
          digitalWrite(LED_STAGE3, LOW);
      }
      else if(SENSOR >= gass_keep)
      {
          close_gass_shutter(CLOSED);
          RISE = false;
          digitalWrite(LED_RISE, LOW);
      }
   }
}
//Ar-CH4:
void control_gass_flow(int& SENSOR)
{
    if (WATCH && !RISE)
    {
        digitalWrite(LED_WATCH, HIGH);
        if((SENSOR < gass_threshold_p1)) //problem persists here;
        {
            /*JUST OPEN A SHUTTER AND KEEP IT OPENED*/
            open_gass_shutter(CLOSED);
            send_sensor_value();
            //digitalWrite(LED_SHUTTER, HIGH);
            digitalWrite(LED_STAGE1, HIGH);
            digitalWrite(LED_STAGE2, LOW);
            digitalWrite(LED_STAGE3, LOW);
        }
        else if ((SENSOR > gass_threshold_p1) && (SENSOR < gass_threshold_p2))
        {
            digitalWrite(LED_STAGE1, LOW);
            digitalWrite(LED_STAGE2, HIGH);
            digitalWrite(LED_STAGE3, LOW);
            t_watch = millis();
            while (millis()-t_watch <= open_s1) {
                /* code */
                if(CLOSED)
                {
                    open_gass_shutter(CLOSED);
                }
                send_sensor_value();
            }
            t_watch = millis();
            while (millis()-t_watch <= delay_s1) {
                /* code */
                if(!CLOSED)
                {
                    close_gass_shutter(CLOSED);
                }
                send_sensor_value();
            }


        }
        else if ((SENSOR > gass_threshold_p2) && (SENSOR < gass_keep))
        {
            digitalWrite(LED_STAGE1, LOW);
            digitalWrite(LED_STAGE2, LOW);
            digitalWrite(LED_STAGE3, HIGH);
            t_watch = millis();
            while (millis()-t_watch <= open_s2) {
                /* code */
                if(CLOSED)
                {
                    open_gass_shutter(CLOSED);
                }
                send_sensor_value();
            }
            t_watch = millis();
            while (millis()-t_watch <= delay_s2) {
                /* code */
                if(!CLOSED)
                {
                    close_gass_shutter(CLOSED);
                }
                send_sensor_value();
            }
        }
        else if (SENSOR >= gass_keep)
        {
            digitalWrite(LED_STAGE1, LOW);
            digitalWrite(LED_STAGE2, HIGH);
            digitalWrite(LED_STAGE3, HIGH);
            if(!CLOSED)
            {
                close_gass_shutter(CLOSED);
            }
            send_sensor_value();
        }
    }
    else if (WATCH)
    {
      watch();
    }
    else
    {
      low();
    }

}
//O2:
void control_o2_flow(float& O2_SENSOR)
{
    //Serial.println(F("REM:NOT IMPLEMENTED YET:REM:!"));
    if (USE_O2 && WATCH) {
        send_sensor_value();
        if (O2_SENSOR <= keep_o2)
        {
            /*JUST CLOSE A SHUTTER AND KEEP IT CLOSED*/
            if(!CLOSED)
                {
                    close_gass_shutter(CLOSED);
                }
            send_sensor_value();  
            overflow();          
        }
        if (O2_SENSOR < o2_th && O2_SENSOR > keep_o2) //hardcoded also
        {
            digitalWrite(LED_STAGE1, LOW);
            digitalWrite(LED_STAGE2, HIGH);
            digitalWrite(LED_STAGE3, LOW);
            t_watch = millis();
            while (millis()-t_watch <= t_o2_watch) {
                /* code */
                if(CLOSED)
                {
                    open_gass_shutter(CLOSED);
                }
                send_sensor_value();
            }
            t_watch = millis();
            //close and keep closed up:
            while (millis()-t_watch <= t_o2_wait) {
                /* code */
                if(!CLOSED)
                {
                    close_gass_shutter(CLOSED);
                }
                send_sensor_value();
            }
        }
        if (O2_SENSOR > o2_th) //hardcoded also
        {
            open_gass_shutter(CLOSED);
            send_sensor_value();
            digitalWrite(LED_STAGE1, HIGH);
            digitalWrite(LED_STAGE2, LOW);
            digitalWrite(LED_STAGE3, LOW);
        }
    }   
}