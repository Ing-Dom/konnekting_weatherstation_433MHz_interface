#pragma once

#include <Arduino.h>

#define VENTUS_WEATHERSENSORS_DEBUG true

class Ventus_Weathersensors
{
  //members
  private: static int m_RX433DataPin;
  private: static volatile unsigned long m_rxBuffer;        // Variable zum speichern des Datentelegramms (32 Bit)
  private: static volatile uint8_t m_checksum;              // checksum bit 32 - 35
  private: static volatile bool m_TelegramReady;            // Variable zum anzeigen, das die Daten im Puffer vollstaendig sind

  private: uint16_t m_WindSpeed, m_WindDirection, m_WindGust, m_RainVolume; 
  private: int16_t m_Temperature;
  private: uint8_t m_Humidity;
  private: bool m_BatteryLowW174;
  private: bool m_BatteryLowW132;
  private: void (*m_NewDataCallback)() = NULL;

	//constructors
  public: Ventus_Weathersensors(int RX433DataPin);

  //functions
  public: void Task();

  public: void AttachNewDataCallback(void (*NewDataCallback)());

  private: static void Rx433Handler();
  private: uint8_t ChecksumW132(uint32_t packet);
  private: uint8_t ChecksumW174(uint32_t packet);
  private: int16_t Convert_12BitSignedValue_Int16(uint16_t inputvalue);
};
