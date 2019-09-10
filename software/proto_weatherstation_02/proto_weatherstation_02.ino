/*
EDS-WDD433.01 Weatherstation for 433Mhz Sensors
V0.0.1
*/



/*
ToDo:
  1) check if it works out-of-the-box                     
  2) add Temp and Hum to sketch with current nano board   DONE
  3) check if it works                                    
  4) make a class for reading that stuff...               
  5) port it to ItsyBitsy                                 
  6) add knx and historical value storing stuff           
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
//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#endif

#define DEBUGSERIAL Serial
#define DEBUG true


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
  uint32_t data_age [16];
  uint32_t knx_last_sent [16];

//Dotstar LED
Adafruit_DotStar px(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);


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
  mysensors->AttachNewDataCallback(NewVentus_WeathersensorsDataAvailible);

  Debug.println(F("Setup is ready. go to loop..."));

}


void NewVentus_WeathersensorsDataAvailible()
{
  Debug.println(F("NewData"));

  uint32_t currentmillis = millis();
  uint16_t NewDataBitset = mysensors->GetNewDataBitset();

  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_TEMPERATURE))
  {
    int16_t NewTemperature = mysensors->GetTemperature();

    Debug.println(F("Temperature: %d"), NewTemperature);

    calculateElapsedMillis(data_age[VENTUS_WEATHERSENSORS_TEMPERATURE], currentmillis);
    // check parameters if it should be sent

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_TEMPERATURE] = currentmillis;
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_HUMIDITY))
  {
    uint8_t NewHumidity = mysensors->GetHumidity();
    Debug.println(F("NewHumidity: %d"), NewHumidity);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_HUMIDITY] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDSPEED))
  {
    uint16_t NewWindspeed = mysensors->GetWindSpeed();
    Debug.println(F("NewWindspeed: %d"), NewWindspeed);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_WINDSPEED] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDGUST))
  {
    uint16_t NewWindGust = mysensors->GetWindGust();
    Debug.println(F("NewWindGust: %d"), NewWindGust);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_WINDGUST] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDDIRECTION))
  {
    uint16_t NewWindDirection = mysensors->GetWindDirection();
    Debug.println(F("NewWindDirection: %d"), NewWindDirection);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_WINDDIRECTION] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RAIN))
  {
    uint16_t NewRainVolume = mysensors->GetRainVolume();
    Debug.println(F("NewRainVolume: %d"), NewRainVolume);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_RAIN] = millis();
  }

  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_BATTERYLOWW132))
  {
    bool NewBatteryLowW132 = mysensors->GetBatteryLowW132();
    Debug.println(F("NewBatteryLowW132: %d"), NewBatteryLowW132);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_BATTERYLOWW132] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_BATTERYLOWW174))
  {
    bool NewBatteryLowW174 = mysensors->GetBatteryLowW174();
    Debug.println(F("NewBatteryLowW174: %d"), NewBatteryLowW174);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    //knx.write()...
    data_age[VENTUS_WEATHERSENSORS_BATTERYLOWW174] = millis();
  }


  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW132))
  {
    uint8_t NewRandomIDW132 = mysensors->GetRandomIDW132();
    Debug.println(F("NewRandomIDW132: %d"), NewRandomIDW132);

    Knx.write(COMOBJ_randomidw132, NewRandomIDW132);
    data_age[VENTUS_WEATHERSENSORS_RANDOMIDW132] = millis();
  }
  if(NewDataBitset & ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW174))
  {
    uint8_t NewRandomIDW174 = mysensors->GetRandomIDW174();
    Debug.println(F("NewRandomIDW174: %d"), NewRandomIDW174);
    // check parameters if it should be sent
    // last_time_sent, KO value vs. new value

    Knx.write(COMOBJ_randomidw174, NewRandomIDW174);
    data_age[VENTUS_WEATHERSENSORS_RANDOMIDW174] = millis();
  }

  

}

void loop()
{
  mysensors->Task();
}

