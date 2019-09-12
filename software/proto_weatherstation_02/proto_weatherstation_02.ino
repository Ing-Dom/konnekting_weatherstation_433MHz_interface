/*
EDS-WDD433.01 Weatherstation for 433Mhz Sensors
V0.0.1
*/



/*
ToDo:
params in xml
abs humidity
dewpoint
batterylow
bme280
data age is not correct only updated when changed)
*/



// Developed/Tested with
//
// Library Versions:
//
// KONNEKTING_Device_Library: 1.0.0-BETA4b
// Adafriut_Dotstar: 1.1.2
// Adafriut_SPIFlash: 1.0.8 / 1.1.0
//
// Other Versions:
// Arduino IDE: 1.8.8
// Boardmanager Arduino SAMD Boards: 1.8.3
// Boardmanager Adafruit SAMD Boards: 1.5.3

/* DotStar RBG LED Color definitions

red:    Waiting for establishing Debug Connection USB-Serial
blue:   FactoryMode - Programm with Konnekting Suite
green:  Normale Mode
cyan:   Normale Mode with Serial Debug
*/

#include <KonnektingDevice.h>
// include device related configuration code, created by "KONNEKTING CodeGenerator"
#include "kdevice_EDS-WS433.01.h"
#include "FileAsEEPROM.h"
#include "timeslice.h"
#include <Adafruit_DotStar.h>
#include "ventus_weathersensors.h"


// ################################################
// ### DEBUG CONFIGURATION
// ################################################
#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#endif

#define DEBUGSERIAL Serial


// ################################################
// ### IO Configuration
// ################################################
#define PROG_LED_PIN 13
#define PROG_BUTTON_PIN NONE
#define RX433PIN 7

//Use these pin definitions for the ItsyBitsy M0 Dotstar LED
#define DATAPIN    41
#define CLOCKPIN   40
#define NUMPIXELS   1


// ################################################
// ### Global variables, sketch related
// ################################################
  
Ventus_Weathersensors *mysensors;
uint32_t data_last_received [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t knx_last_sent [16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t knx_last_sent_value[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//Dotstar LED
Adafruit_DotStar px(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);


// Konnekting parameters
uint8_t param_max_data_age;
uint8_t param_send_on_change[6];
uint8_t param_cyclic_send_rate[6];


// ################################################
// ### KONNEKTING Configuration
// ################################################

#define KNX_SERIAL Serial1 // Leonardo/Micro etc. use Serial1


//FlashStorage
byte readMemory(int index)
{
    Debug.println(F("FLASH read on index %d"),index);
    return FileEEPROM.read(index);
}
void writeMemory(int index, byte val)
{
    Debug.println(F("FLASH write value %d on index %d"),val, index);
    FileEEPROM.write(index, val);
}
void updateMemory(int index, byte val)
{
    Debug.println(F("FLASH update"));
    if (FileEEPROM.read(index) != val) {
        FileEEPROM.write(index, val);
    }
}
void commitMemory()
{
    Debug.println(F("FLASH commit"));
    FileEEPROM.commit();
}

// ################################################
// ### set ProgLED status
// ################################################
//this function is used to indicate programming mode.
//you can use LED, LCD display or what ever you want...
void progLed(bool state)
{
    digitalWrite(PROG_LED_PIN, state);
}

// ################################################
// ### KNX EVENT CALLBACK
// ################################################

void knxEvents(byte index)
{
  switch (index)
  {
    case COMOBJ_rainvolume1_reset:
    {
      if(Knx.read(COMOBJ_rainvolume1_reset))
      {
        ;
      }
    }
    break;
    default:
    {
    }
    break;
  }
}


void setup()
{

  px.begin(); // Initialize pins for output
  px.show();  // Turn all LEDs off ASAP
  px.setPixelColor(0, 50, 0, 0); // red
  px.show();


  // debug related stuff
  #ifdef KDEBUG
  // Start debug serial with 9600 bauds
  DEBUGSERIAL.begin(115200);
  while (!DEBUGSERIAL)

  // make debug serial port known to debug class
  // Means: KONNEKTING will use the same serial port for console debugging
  Debug.setPrintStream(&DEBUGSERIAL);
  #endif

  Konnekting.setMemoryReadFunc(&readMemory);
  Konnekting.setMemoryWriteFunc(&writeMemory);
  Konnekting.setMemoryUpdateFunc(&updateMemory);
  Konnekting.setMemoryCommitFunc(&commitMemory);

  // Initialize KNX enabled Arduino Board
    Konnekting.init(KNX_SERIAL,
                    &progLed,
                    MANUFACTURER_ID,
                    DEVICE_ID,
                    REVISION);
  
  if (!Konnekting.isFactorySetting())
  {
    param_max_data_age = 10;
    for(int i = 0;i<6;i++)
    {
      param_send_on_change[i] = 0;
      param_cyclic_send_rate[i] = 1;  // ToDo: assign real param
    }

    /*
    param_device_mode = (unsigned short)Konnekting.getUINT8Param(PARAM_device_mode);
    param_code = (unsigned long)Konnekting.getUINT32Param(PARAM_code);
    param_default_cmd = (unsigned short)Konnekting.getUINT8Param(PARAM_default_cmd);
    for(int i=0;i<10;i++)
      param_cmd_out_mode[i] = (unsigned short)Konnekting.getUINT8Param(PARAM_cmd0_out_mode+i);
    param_motorlock_onclose = (unsigned short)Konnekting.getUINT8Param(PARAM_motorlock_onclose);
    param_motorlock_open_cmd = (unsigned short)Konnekting.getUINT8Param(PARAM_motorlock_open_cmd);
    param_motorlock_lock_cmd = (unsigned short)Konnekting.getUINT8Param(PARAM_motorlock_lock_cmd);
    param_motorlock_unlock_cmd = (unsigned short)Konnekting.getUINT8Param(PARAM_motorlock_unlock_cmd);
    param_codelock_wrongcode_timeout1_no = (unsigned short)Konnekting.getUINT8Param(PARAM_codelock_wrongcode_timeout1_no);
    param_codelock_wrongcode_timeout1_time = (unsigned short)Konnekting.getUINT8Param(PARAM_codelock_wrongcode_timeout1_time);
    param_codelock_wrongcode_timeout2_no = (unsigned short)Konnekting.getUINT8Param(PARAM_codelock_wrongcode_timeout2_no);
    param_codelock_wrongcode_timeout2_time = (unsigned short)Konnekting.getUINT8Param(PARAM_codelock_wrongcode_timeout2_time);
    param_codelock_keypress_timeout_time = (unsigned short)Konnekting.getUINT8Param(PARAM_codelock_keypress_timeout_time);
    */

    
    #ifdef KDEBUG
    px.setPixelColor(0, 63, 136, 143); // cyan
    #else
    px.setPixelColor(0, 0, 50, 0); // green
    #endif
    px.show();
  }
  else
  {
    Debug.println(F("Device is in factory mode. Starting programming mode..."));
    px.setPixelColor(0, 0, 0, 50); // blue
    px.show();
    Konnekting.setProgState(true);
  }
  timeslice_setup();
  mysensors = new Ventus_Weathersensors(RX433PIN);
  //ToDo: write correct filters here from param
  mysensors->RandomIDFilterW132 = 99;
  mysensors->RandomIDFilterW174 = 142;
  mysensors->AttachNewDataCallback(NewVentus_WeathersensorsDataAvailible);

  Debug.println(F("Setup is ready. go to loop..."));

}


void NewVentus_WeathersensorsDataAvailible()
{
  //Debug.println(F("NewData"));

  uint32_t currentmillis = millis();
  uint16_t NewDataBitset = mysensors->GetNewDataBitset();

  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_TEMPERATURE))
  {
    int16_t NewTemperature = mysensors->GetTemperature();

    Debug.println(F("Temperature: %d"), NewTemperature);

    data_last_received[VENTUS_WEATHERSENSORS_TEMPERATURE] = currentmillis;
    
    // check if the parameter allows sending
    if(abs(knx_last_sent_value[COMOBJ_temperature] - NewTemperature) > param_send_on_change[VENTUS_WEATHERSENSORS_TEMPERATURE]) //ToDo: check here what happens with negative values....
    {
      Knx.write(COMOBJ_temperature, NewTemperature / 10.0); // want to use double to not lose precision, so divide by 10.0
      knx_last_sent[COMOBJ_temperature] = currentmillis;
      knx_last_sent_value[COMOBJ_temperature] = NewTemperature;
    }
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_HUMIDITY))
  {
    uint8_t NewHumidity = mysensors->GetHumidity();

    Debug.println(F("NewHumidity: %d"), NewHumidity);
    
    data_last_received[VENTUS_WEATHERSENSORS_HUMIDITY] = millis();

    // check if the parameter allows sending
    if(abs(knx_last_sent_value[VENTUS_WEATHERSENSORS_HUMIDITY] - NewHumidity) > param_send_on_change[VENTUS_WEATHERSENSORS_HUMIDITY])
    {
      Knx.write(COMOBJ_humidity, NewHumidity); // can feed the int in directly, will be converted to F16 by Lib correctly
      knx_last_sent[COMOBJ_humidity] = currentmillis;
      knx_last_sent_value[VENTUS_WEATHERSENSORS_HUMIDITY] = NewHumidity;
    }
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDSPEED))
  {
    uint16_t NewWindspeed = mysensors->GetWindSpeed();
    Debug.println(F("NewWindspeed: %d"), NewWindspeed);

    data_last_received[VENTUS_WEATHERSENSORS_WINDSPEED] = millis();

    // check if the parameter allows sending
    if(abs(knx_last_sent_value[VENTUS_WEATHERSENSORS_WINDSPEED] - NewWindspeed) > param_send_on_change[VENTUS_WEATHERSENSORS_WINDSPEED])
    {
      Knx.write(COMOBJ_windspeed, NewWindspeed / 10.0); // value is in int 0.1m/s divide by 10.0 to get value in float m/s
      knx_last_sent[COMOBJ_windspeed] = currentmillis;
      knx_last_sent_value[VENTUS_WEATHERSENSORS_WINDSPEED] = NewWindspeed;
    }
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDGUST))
  {
    uint16_t NewWindGust = mysensors->GetWindGust();
    Debug.println(F("NewWindGust: %d"), NewWindGust);
    
    data_last_received[VENTUS_WEATHERSENSORS_WINDGUST] = millis();

    // check if the parameter allows sending
    if(abs(knx_last_sent_value[VENTUS_WEATHERSENSORS_WINDGUST] - NewWindGust) > param_send_on_change[VENTUS_WEATHERSENSORS_WINDGUST])
    {
      Knx.write(COMOBJ_windgust, NewWindGust / 10.0); // value is in int 0.1m/s divide by 10.0 to get value in float m/s
      knx_last_sent[COMOBJ_windgust] = currentmillis;
      knx_last_sent_value[VENTUS_WEATHERSENSORS_WINDGUST] = NewWindGust;
    }
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDDIRECTION))
  {
    uint16_t NewWindDirection = mysensors->GetWindDirection();
    Debug.println(F("NewWindDirection: %d"), NewWindDirection);
    
    data_last_received[VENTUS_WEATHERSENSORS_WINDDIRECTION] = millis();

    // check if the parameter allows sending
    if(abs(knx_last_sent_value[VENTUS_WEATHERSENSORS_WINDDIRECTION] - NewWindDirection) > param_send_on_change[VENTUS_WEATHERSENSORS_WINDDIRECTION])
    {
      Knx.write(COMOBJ_winddirection, (NewWindDirection * 255) / 360); // can feed the int scaled 0-360° scaled to 0-255 // ToDo Check this with ETS...
      knx_last_sent[COMOBJ_winddirection] = currentmillis;
      knx_last_sent_value[VENTUS_WEATHERSENSORS_WINDDIRECTION] = NewWindDirection;
    }
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RAIN))
  {
    uint16_t NewRainVolume = mysensors->GetRainVolume();
    Debug.println(F("NewRainVolume: %d"), NewRainVolume);
    
    data_last_received[VENTUS_WEATHERSENSORS_RAIN] = millis();

    // check if the parameter allows sending
    if(abs(knx_last_sent_value[VENTUS_WEATHERSENSORS_RAIN] - NewRainVolume) > param_send_on_change[VENTUS_WEATHERSENSORS_RAIN])
    {
      Knx.write(COMOBJ_rainvolume, (NewRainVolume / 100.0)); // value is in 0,01 mm, divide by 100 to get mm. Lib will convert to F16..
      knx_last_sent[COMOBJ_rainvolume] = currentmillis;
      knx_last_sent_value[VENTUS_WEATHERSENSORS_RAIN] = NewRainVolume;
    }
  }

  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW132))
  {
    uint8_t NewRandomIDW132 = mysensors->GetRandomIDW132();
    Debug.println(F("NewRandomIDW132: %d"), NewRandomIDW132);
    if(mysensors->RandomIDFilterW132 == 0)  // only send RandomIDs when Filter is disabled
      Knx.write(COMOBJ_randomidw132, NewRandomIDW132);
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW174))
  {
    uint8_t NewRandomIDW174 = mysensors->GetRandomIDW174();
    Debug.println(F("NewRandomIDW174: %d"), NewRandomIDW174);
    if(mysensors->RandomIDFilterW174 == 0)  // only send RandomIDs when Filter is disabled
      Knx.write(COMOBJ_randomidw174, NewRandomIDW174);
  }

  

}

void loop()
{
    Knx.task();
    mysensors->Task();
    timeslice_scheduler();

    if (Konnekting.isReadyForApplication())
    {
      
    }
}

void T1() // 1ms
{
}

void T2() // 5ms
{
}

void T3() // 25ms
{
}

void T4() // 500ms
{
  // cyclic sending
  if(param_cyclic_send_rate[VENTUS_WEATHERSENSORS_TEMPERATURE] > 0)  // value > 0 indicates that feature is active
  {
    if( data_last_received[VENTUS_WEATHERSENSORS_TEMPERATURE] != 0 &&     // check if there is value to send AND
        calculateElapsedMillis(data_last_received[VENTUS_WEATHERSENSORS_TEMPERATURE], millis()) < param_max_data_age * 30000) // stop sending when data is to old (param digit is 30s = 30000ms)
    {
      if(calculateElapsedMillis(knx_last_sent[COMOBJ_temperature], millis()) > param_cyclic_send_rate[VENTUS_WEATHERSENSORS_TEMPERATURE] * 10000) // one param digit is 10s
      {
        Knx.write(COMOBJ_temperature, mysensors->GetTemperature() / 10.0); // want to use double to not lose precision, so divide by 10.0
        knx_last_sent[COMOBJ_temperature] = millis();
      }
    }
  }

  if(param_cyclic_send_rate[VENTUS_WEATHERSENSORS_HUMIDITY] > 0)  // value > 0 indicates that feature is active
  {
    if( data_last_received[VENTUS_WEATHERSENSORS_HUMIDITY] != 0 &&     // check if there is value to send AND
        calculateElapsedMillis(data_last_received[VENTUS_WEATHERSENSORS_HUMIDITY], millis()) < param_max_data_age * 30000) // stop sending when data is to old (param digit is 30s = 30000ms)
    {
      if(calculateElapsedMillis(knx_last_sent[COMOBJ_humidity], millis()) > param_cyclic_send_rate[VENTUS_WEATHERSENSORS_HUMIDITY] * 10000) // one param digit is 10s
      {
        Knx.write(COMOBJ_humidity, mysensors->GetHumidity()); // can feed the int in directly, will be converted to F16 by Lib correctly
        knx_last_sent[COMOBJ_humidity] = millis();
        knx_last_sent_value[COMOBJ_humidity] =  mysensors->GetHumidity();
      }
    }
  }

  if(param_cyclic_send_rate[VENTUS_WEATHERSENSORS_WINDSPEED] > 0)  // value > 0 indicates that feature is active
  {
    if( data_last_received[VENTUS_WEATHERSENSORS_WINDSPEED] != 0 &&     // check if there is value to send AND
        calculateElapsedMillis(data_last_received[VENTUS_WEATHERSENSORS_WINDSPEED], millis()) < param_max_data_age * 30000) // stop sending when data is to old (param digit is 30s = 30000ms)
    {
      if(calculateElapsedMillis(knx_last_sent[COMOBJ_windspeed], millis()) > param_cyclic_send_rate[VENTUS_WEATHERSENSORS_WINDSPEED] * 10000) // one param digit is 10s
      {
        Knx.write(COMOBJ_windspeed, mysensors->GetWindSpeed() / 10.0); // value is in int 0.1m/s divide by 10.0 to get value in float m/s
        knx_last_sent[COMOBJ_windspeed] = millis();
        knx_last_sent_value[COMOBJ_windspeed] =  mysensors->GetWindSpeed();
      }
    }
  }

  if(param_cyclic_send_rate[VENTUS_WEATHERSENSORS_WINDGUST] > 0)  // value > 0 indicates that feature is active
  {
    if( data_last_received[VENTUS_WEATHERSENSORS_WINDGUST] != 0 &&     // check if there is value to send AND
        calculateElapsedMillis(data_last_received[VENTUS_WEATHERSENSORS_WINDGUST], millis()) < param_max_data_age * 30000) // stop sending when data is to old (param digit is 30s = 30000ms)
    {
      if(calculateElapsedMillis(knx_last_sent[COMOBJ_windgust], millis()) > param_cyclic_send_rate[VENTUS_WEATHERSENSORS_WINDGUST] * 10000) // one param digit is 10s
      {
        Knx.write(COMOBJ_windgust, mysensors->GetWindGust() / 10.0); // value is in int 0.1m/s divide by 10.0 to get value in float m/s
        knx_last_sent[COMOBJ_windgust] = millis();
        knx_last_sent_value[COMOBJ_windgust] =  mysensors->GetWindGust();
      }
    }
  }

  if(param_cyclic_send_rate[VENTUS_WEATHERSENSORS_WINDDIRECTION] > 0)  // value > 0 indicates that feature is active
  {
    if( data_last_received[VENTUS_WEATHERSENSORS_WINDDIRECTION] != 0 &&     // check if there is value to send AND
        calculateElapsedMillis(data_last_received[VENTUS_WEATHERSENSORS_WINDDIRECTION], millis()) < param_max_data_age * 30000) // stop sending when data is to old (param digit is 30s = 30000ms)
    {
      if(calculateElapsedMillis(knx_last_sent[COMOBJ_winddirection], millis()) > param_cyclic_send_rate[VENTUS_WEATHERSENSORS_WINDDIRECTION] * 10000) // one param digit is 10s
      {
        Knx.write(COMOBJ_winddirection, (mysensors->GetWindDirection() * 255) / 360); // can feed the int scaled 0-360° scaled to 0-255 // ToDo Check this with ETS...
        knx_last_sent[COMOBJ_winddirection] = millis();
        knx_last_sent_value[COMOBJ_winddirection] =  mysensors->GetWindDirection();
      }
    }
  }

  if(param_cyclic_send_rate[VENTUS_WEATHERSENSORS_RAIN] > 0)  // value > 0 indicates that feature is active
  {
    if( data_last_received[VENTUS_WEATHERSENSORS_RAIN] != 0 &&     // check if there is value to send AND
        calculateElapsedMillis(data_last_received[VENTUS_WEATHERSENSORS_RAIN], millis()) < param_max_data_age * 30000) // stop sending when data is to old (param digit is 30s = 30000ms)
    {
      if(calculateElapsedMillis(knx_last_sent[COMOBJ_rainvolume], millis()) > param_cyclic_send_rate[VENTUS_WEATHERSENSORS_RAIN] * 10000) // one param digit is 10s
      {
        Knx.write(COMOBJ_rainvolume, (mysensors->GetRainVolume() / 100.0)); // value is in 0,01 mm, divide by 100 to get mm. Lib will convert to F16..
        knx_last_sent[COMOBJ_rainvolume] = millis();
        knx_last_sent_value[COMOBJ_rainvolume] =  mysensors->GetRainVolume();
      }
    }
  }
  #define COMOBJ_batteryloww132 14 //ToDo remove
  if( calculateElapsedMillis(knx_last_sent[COMOBJ_batteryloww132], millis()) > 3600000 ||   // send once an hour
      knx_last_sent_value[COMOBJ_batteryloww132] !=  mysensors->GetBatteryLowW132())    // or when it changed
  {
    Knx.write(COMOBJ_batteryloww132, mysensors->GetBatteryLowW132());
    knx_last_sent[COMOBJ_batteryloww132] = millis();
    knx_last_sent_value[COMOBJ_batteryloww132] =  mysensors->GetBatteryLowW132();
  }
  #define COMOBJ_batteryloww174 15 //ToDo remove
  if( calculateElapsedMillis(knx_last_sent[COMOBJ_batteryloww174], millis()) > 3600000 ||   // send once an hour
      knx_last_sent_value[COMOBJ_batteryloww174] !=  mysensors->GetBatteryLowW174())    // or when it changed
  {
    Knx.write(COMOBJ_batteryloww174, mysensors->GetBatteryLowW174());
    knx_last_sent[COMOBJ_batteryloww174] = millis();
    knx_last_sent_value[COMOBJ_batteryloww174] =  mysensors->GetBatteryLowW174();
  }


}

void T5() // 10000ms
{
  
}