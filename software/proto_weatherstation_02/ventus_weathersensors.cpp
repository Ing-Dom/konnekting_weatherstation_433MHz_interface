#include "ventus_weathersensors.h"
#include <Arduino.h>

  //static members
  static int  Ventus_Weathersensors::m_RX433DataPin;
  static volatile unsigned long  Ventus_Weathersensors::m_rxBuffer;        // Variable zum speichern des Datentelegramms (32 Bit)
  static volatile uint8_t  Ventus_Weathersensors::m_checksum;              // checksum bit 32 - 35
  static volatile bool  Ventus_Weathersensors::m_TelegramReady;            // Variable zum anzeigen, das die Daten im Puffer vollstaendig sind

	//constructors
  Ventus_Weathersensors::Ventus_Weathersensors(int RX433DataPin)
  {
    Ventus_Weathersensors::m_RX433DataPin = RX433DataPin;
    pinMode(Ventus_Weathersensors::m_RX433DataPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(Ventus_Weathersensors::m_RX433DataPin), Rx433Handler, CHANGE);
  }

  //functions
  void Ventus_Weathersensors::Task()
  {
    // periodically check for received telegrams and process them
    if (m_TelegramReady)
    {
      byte randomID = (unsigned long) m_rxBuffer & 0xff;           // random ID, but BIt 4 is always 0
      bool bState = (unsigned long) (m_rxBuffer >> 8) & 0x1;       // Bit 8 means battery low
      byte xBits = (unsigned long) (m_rxBuffer >> 9) & 0x3;        // 
      bool buttonState = (unsigned long) m_rxBuffer >> 11 & 0x1;   // Bit 11 indicates the transmission was triggerd manually
      byte subID = (unsigned long) (m_rxBuffer >> 12) & 0x7;       // Bits 12, 13, 14 contain a Sub-ID (W132 sends 3 telegrams Sub-ID 1, 3 and 7)

      m_NewDataBitset = 0;                                         // Reset the Bitset indicating which Data is new

      // skip telegram if checksum is wrong
      if(m_checksum == ChecksumW132(m_rxBuffer) || m_checksum == ChecksumW174(m_rxBuffer))
      {
        if(xBits == 3)
        {
          // when xBits is 3, it is a rain, windspeed or winddirection telegram
          if(subID == 1 && m_checksum == ChecksumW132(m_rxBuffer))
          {
            // subID = 1 is windspeed from W132
            m_BatteryLowW132 = bState;
            m_RandomIDW132 = randomID;
            m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_RANDOMIDW132);   // always update the RandomID

            uint16_t WindSpeed = ((unsigned long) (m_rxBuffer >> 24) & 0xff) * 2;  // die Bits 24-31 enthalten die durchschnittliche Windgeschwindigkeit in Einheiten zu 0.2 m/s und mal 10 ergibt: "* 2" fuer einen Integerwert

            if(WindSpeed != m_WindSpeed)
            {
              m_WindSpeed = WindSpeed;
              m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDSPEED);
            }

            if(VENTUS_WEATHERSENSORS_DEBUG) Serial.println(F("Windspeed"));
          }
          else if(subID == 3 && m_checksum == ChecksumW174(m_rxBuffer))
          {
            // subID = 3 is rain from W174
            m_BatteryLowW174 = bState;
            m_RandomIDW174 = randomID;


            uint16_t RainVolume = ((unsigned long) (m_rxBuffer >> 16) & 0xffff) * 25; // die Bits 16-31 enthalten die Niederschlagsmenge in Einheiten zu je 0.25 mm -> 1 mm = 1 L/m2 hier zusaetzlich mal 10 um einen Integerwert zu erhalten

            if(RainVolume != m_RainVolume)
            {
              m_RainVolume = RainVolume;
              m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_RAIN);
            }

            if(VENTUS_WEATHERSENSORS_DEBUG) Serial.println(F("Rain"));
          }
          else if(subID == 7 && m_checksum == ChecksumW132(m_rxBuffer))
          {
            // subID = 7 is wind direction and gusts from W132
            m_BatteryLowW132 = bState;
            m_RandomIDW132 = randomID;

            uint16_t WindDirection = m_WindDirection;
            uint16_t WindGust = m_WindGust;

            uint16_t wdir = (unsigned long) (m_rxBuffer >> 15) & 0x1ff;    // die Bits 15-23 enthalten die Windrichtung in Grad (0-360)
            if(wdir <= 360 && wdir != m_WindDirection)
            {
              m_WindDirection = wdir; // die Windrichtung wird manchmal falsch uebertragen, deshalb nur Daten bis 360 Grad uebernehmen
              m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDDIRECTION);
            }
            
            uint16_t wg = ((unsigned long) (m_rxBuffer >> 24) & 0xff) * 2;  // die Bits 24-31 enthalten die Windboen in Einheiten zu 0.2 m/s und mal 10 ergibt: "* 2" fuer einen Integerwert
            if(wg < 500)
            {
              m_WindGust = wg;  // die Windboen werden manchmal falsch uebertragen, deshalb nur Daten unter 50m/s (180km/h) uebernehmen
              m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_WINDGUST);
            }


            if(VENTUS_WEATHERSENSORS_DEBUG) Serial.println(F("Wind Direction"));
          }
        }
        else if(m_checksum == ChecksumW132(m_rxBuffer))
        {
          // if the xBits are not both set, it is a temp/hum telegram from W132
          if(VENTUS_WEATHERSENSORS_DEBUG) Serial.println(F("Temp/Hum"));

          m_BatteryLowW132 = bState;
          m_RandomIDW132 = randomID;

          int16_t Temperature = Convert_12BitSignedValue_Int16(((unsigned long) (m_rxBuffer >> 12) & 0x0fff));

          //if(Temperature != m_Temperature)
          {
            m_Temperature = Temperature;
            m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_TEMPERATURE);
          }

          uint8_t rh_ones = ((unsigned long) (m_rxBuffer >> 24) & 0x0F);
          uint8_t rh_tens = ((unsigned long) (m_rxBuffer >> 28) & 0x0F);
          uint8_t Humidity = rh_ones + rh_tens*10;

          //if(Humidity != m_Humidity)
          {
            m_Humidity = Humidity;
            m_NewDataBitset |= ((uint32_t)1 << VENTUS_WEATHERSENSORS_HUMIDITY);
          }
        }
        
        if(m_NewDataBitset && m_NewDataCallback != NULL)    // when there were new Data in this transmission, call callback function
          m_NewDataCallback();
      }

      m_TelegramReady = false;
      m_rxBuffer = 0;
      m_checksum = 0;
    }
  }


  void Ventus_Weathersensors::AttachNewDataCallback(void (*NewDataCallback)())
  {
    m_NewDataCallback = NewDataCallback;
  }

  uint16_t Ventus_Weathersensors::GetNewDataBitset()
  {
    return m_NewDataBitset;
  }

  uint16_t Ventus_Weathersensors::GetWindSpeed()
  {
    return m_WindSpeed;
  }

  uint16_t Ventus_Weathersensors::GetWindDirection()
  {
    return m_WindDirection;
  }

  uint16_t Ventus_Weathersensors::GetWindGust()
  {
    return m_WindGust;
  }

  uint16_t Ventus_Weathersensors::GetRainVolume()
  {
    return m_RainVolume;
  }

  int16_t Ventus_Weathersensors::GetTemperature()
  {
    return m_Temperature;
  }

  uint8_t Ventus_Weathersensors::GetHumidity()
  {
    return m_Humidity;
  }

  bool Ventus_Weathersensors::GetBatteryLowW174()
  {
    return m_BatteryLowW174;
  }

  bool Ventus_Weathersensors::GetBatteryLowW132()
  {
    return m_BatteryLowW132;
  }


  void Ventus_Weathersensors::Rx433Handler()
  {
    // Interrupt-Routine
    if(m_TelegramReady)
      return;    // wenn m_TelegramReady noch gesetzt ist, dann Funktion verlassen, damit der Puffer nicht ueberschrieben wird
    
    static unsigned long rxHigh = 0, rxLow = 0;
    static bool syncBit = 0, dataBit = 0;
    static byte rxCounter = 0;
    bool rxState = digitalRead(Ventus_Weathersensors::m_RX433DataPin); // Daten-Eingang auslesen
    if(!rxState)
    {                           
      // wenn Eingang = Low, dann...
      rxLow = micros();                       // Mikrosekunden fuer Low merken
    }
    else
    {                                  
      // ansonsten (Eingang = High)
      rxHigh = micros();                      // Mikrosekunden fuer High merken
      if(rxHigh - rxLow > 8500) // ToDo && rxLow != 0 // not the first "High" without reference for low
      { 
        // High-Impuls laenger als 8.5 ms dann SyncBit erkannt (9 ms mit 0.5 ms Toleranz)
        syncBit = 1;               // syncBit setzen
        m_rxBuffer = 0;              // den Puffer loeschen
        rxCounter = 0;             // den Counter auf 0 setzen
        m_checksum = 0;
        return;                    // auf den naechsten Interrupt warten (Funktion verlassen)
      }
      if(syncBit)
      {                                          
        // wenn das SyncBit erkannt wurde, dann High-Impuls auswerten:
        if (rxHigh - rxLow > 1500) dataBit = 0;               // High-Impuls laenger als 1.5 ms (2 ms mit 0.5 ms Tolenz), dann DataBit = 0
        if (rxHigh - rxLow > 3500) dataBit = 1;               // High-Impuls laenger als 3.5 ms (4 ms mit 0.5 ms Tolenz), dann DataBit = 1
        if (rxCounter < 32)
        {                                
          // Wenn noch keine 32 Bits uebertragen wurden, dann...
          m_rxBuffer |= (unsigned long) dataBit << rxCounter++; // das Datenbit in den Puffer schieben und den Counter erhoehen
        }
        else if(rxCounter < 36)
        {
          m_checksum |= (uint8_t) dataBit << ((rxCounter++) - 32);
          if(rxCounter == 36)  // add one because rxCounter was already incremented with ++
          {
            rxCounter = 0; // den Counter zuruecksetzen
            syncBit = 0;   // syncBit zuruecksetzen
            m_TelegramReady = true;   // Ok signalisieren (m_rxBuffer ist vollstaendig) fuer die Auswertung in Loop
          }
        }
        else
        {         
          // wenn das Datentelegramm (36 Bit) vollstaendig uebertragen wurde, dann...
          rxCounter = 0; // den Counter zuruecksetzen
          syncBit = 0;   // syncBit zuruecksetzen
          m_TelegramReady = true;   // Ok signalisieren (m_rxBuffer ist vollstaendig) fuer die Auswertung in Loop
        }
      }
    }
  }


  uint8_t Ventus_Weathersensors::ChecksumW132(uint32_t packet)
  {
    uint8_t n0 = (unsigned long) (packet >> 0) & 0xf;
    uint8_t n1 = (unsigned long) (packet >> 4) & 0xf;
    uint8_t n2 = (unsigned long) (packet >> 8) & 0xf;
    uint8_t n3 = (unsigned long) (packet >> 12) & 0xf;
    uint8_t n4 = (unsigned long) (packet >> 16) & 0xf;
    uint8_t n5 = (unsigned long) (packet >> 20) & 0xf;
    uint8_t n6 = (unsigned long) (packet >> 24) & 0xf;
    uint8_t n7 = (unsigned long) (packet >> 28) & 0xf;

    return ( 0xf - n0 - n1 - n2 - n3 - n4 - n5 - n6 - n7 ) & 0xf;
  }


  uint8_t Ventus_Weathersensors::ChecksumW174(uint32_t packet)
  {
    uint8_t n0 = (unsigned long) (packet >> 0) & 0xf;
    uint8_t n1 = (unsigned long) (packet >> 4) & 0xf;
    uint8_t n2 = (unsigned long) (packet >> 8) & 0xf;
    uint8_t n3 = (unsigned long) (packet >> 12) & 0xf;
    uint8_t n4 = (unsigned long) (packet >> 16) & 0xf;
    uint8_t n5 = (unsigned long) (packet >> 20) & 0xf;
    uint8_t n6 = (unsigned long) (packet >> 24) & 0xf;
    uint8_t n7 = (unsigned long) (packet >> 28) & 0xf;

    return ( 0x7 + n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7 ) & 0xf;
  }

  int16_t Ventus_Weathersensors::Convert_12BitSignedValue_Int16(uint16_t inputvalue)
  {
    inputvalue = inputvalue & 0x0fff;  // make sure the value is realy just 12bit
    if((inputvalue) & 0x800)  // sign bit is setup
    {
      // ToDo
      // Test required

      inputvalue = inputvalue & 0x07ff; // remove sign bit
      inputvalue--;   // subtract 1
      inputvalue = ~inputvalue; 
      inputvalue = inputvalue & 0x07ff; // again remove bit 11-15

      return ((int16_t)inputvalue) * -1;
    }
    else
    {
      // value is positive
      return inputvalue;
    }
  }