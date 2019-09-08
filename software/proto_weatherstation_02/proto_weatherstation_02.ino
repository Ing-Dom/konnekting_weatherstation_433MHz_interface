/* Developement ToDos

  1) check if it works out-of-the-box                     
  2) add Temp and Hum to sketch with current nano board   DONE
  3) check if it works                                    
  4) make a class for reading that stuff...               
  5) port it to ItsyBitsy                                 
  6) add knx and historical value storing stuff           


*/

#include "ventus_weathersensors.h"




#define DEBUG true

Ventus_Weathersensors mysensors(2);





void setup()
{
  Serial.begin(115200);
  if (DEBUG) Serial.println(F("Start"));

  mysensors.AttachNewDataCallback(NewVentus_WeathersensorsDataAvailible);
}


void NewVentus_WeathersensorsDataAvailible()
{
  if (DEBUG) Serial.println(F("NewData"));
}

void loop()
{
  mysensors.Task();
}

