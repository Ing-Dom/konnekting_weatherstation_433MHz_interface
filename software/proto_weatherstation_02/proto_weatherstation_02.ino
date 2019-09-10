/* Developement ToDos

  1) check if it works out-of-the-box                     
  2) add Temp and Hum to sketch with current nano board   DONE
  3) check if it works                                    
  4) make a class for reading that stuff...               
  5) port it to ItsyBitsy                                 
  6) add knx and historical value storing stuff           


*/

#include "ventus_weathersensors.h"
#include "timeslice.h"




#define DEBUG true

Ventus_Weathersensors mysensors(2);

uint32_t data_age [16];
uint32_t knx_last_sent [16];





void setup()
{
  Serial.begin(115200);
  if (DEBUG) Serial.println(F("Start"));

  mysensors.AttachNewDataCallback(NewVentus_WeathersensorsDataAvailible);
}


void NewVentus_WeathersensorsDataAvailible()
{
  if (DEBUG) Serial.println(F("NewData"));

  uint32_t currentmillis = millis();
  uint16_t NewDataBitset = mysensors.GetNewDataBitset();

  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_TEMPERATURE))
  {
    int16_t NewTemperature = mysensors.GetTemperature();

    if (DEBUG) Serial.print(F("Temperature: "));
    if (DEBUG) Serial.println(NewTemperature);

    calculateElapsedMillis(data_age[VENTUS_WEATHERSENSORS_TEMPERATURE], currentmillis);
    // check parameters if it should be sent

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_TEMPERATURE] = currentmillis;
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_HUMIDITY))
  {
    uint8_t NewHumidity = mysensors.GetHumidity();
    if (DEBUG) Serial.print(F("NewHumidity: "));
    if (DEBUG) Serial.println(NewHumidity);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_HUMIDITY] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDSPEED))
  {
    uint16_t NewWindspeed = mysensors.GetWindSpeed();
    if (DEBUG) Serial.print(F("NewWindspeed: "));
    if (DEBUG) Serial.println(NewWindspeed);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_WINDSPEED] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDGUST))
  {
    uint16_t NewWindGust = mysensors.GetWindGust();
    if (DEBUG) Serial.print(F("NewWindGust: "));
    if (DEBUG) Serial.println(NewWindGust);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_WINDGUST] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDDIRECTION))
  {
    uint16_t NewWindDirection = mysensors.GetWindDirection();
    if (DEBUG) Serial.print(F("NewWindDirection: "));
    if (DEBUG) Serial.println(NewWindDirection);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_WINDDIRECTION] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RAIN))
  {
    uint16_t NewRainVolume = mysensors.GetRainVolume();
    if (DEBUG) Serial.print(F("NewRainVolume: "));
    if (DEBUG) Serial.println(NewRainVolume);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_RAIN] = millis();
  }

  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_BATTERYLOWW132))
  {
    bool NewBatteryLowW132 = mysensors.GetBatteryLowW132();
    if (DEBUG) Serial.print(F("NewBatteryLowW132: "));
    if (DEBUG) Serial.println(NewBatteryLowW132);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_BATTERYLOWW132] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_BATTERYLOWW174))
  {
    bool NewBatteryLowW174 = mysensors.GetBatteryLowW174();
    if (DEBUG) Serial.print(F("NewBatteryLowW174: "));
    if (DEBUG) Serial.println(NewBatteryLowW174);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_BATTERYLOWW174] = millis();
  }


  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW132))
  {
    uint8_t NewRandomIDW132 = mysensors.GetRandomIDW132();
    if (DEBUG) Serial.print(F("NewRandomIDW132: "));
    if (DEBUG) Serial.println(NewRandomIDW132);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_RANDOMIDW132] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW174))
  {
    uint8_t NewRandomIDW174 = mysensors.GetRandomIDW174();
    if (DEBUG) Serial.print(F("NewRandomIDW174: "));
    if (DEBUG) Serial.println(NewRandomIDW174);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_RANDOMIDW174] = millis();
  }

  

}

void loop()
{
  mysensors.Task();
}

