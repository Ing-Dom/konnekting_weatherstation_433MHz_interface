#pragma once

#include <Arduino.h>

#define VENTUS_WEATHERSENSORS_DEBUG true

#define VENTUS_WEATHERSENSORS_TEMPERATURE 0
#define VENTUS_WEATHERSENSORS_HUMIDITY 1
#define VENTUS_WEATHERSENSORS_WINDSPEED 2
#define VENTUS_WEATHERSENSORS_WINDGUST 3
#define VENTUS_WEATHERSENSORS_WINDDIRECTION 4
#define VENTUS_WEATHERSENSORS_RAIN 5
#define VENTUS_WEATHERSENSORS_DEWPOINT 6
#define VENTUS_WEATHERSENSORS_ABSHUMIDITY 7
#define VENTUS_WEATHERSENSORS_BATTERYLOWW132 9
#define VENTUS_WEATHERSENSORS_BATTERYLOWW174 10
#define VENTUS_WEATHERSENSORS_RANDOMIDW132 11
#define VENTUS_WEATHERSENSORS_RANDOMIDW174 12




class Ventus_Weathersensors
{
  //members
  private: static int m_RX433DataPin;
  private: static volatile unsigned long m_rxBuffer;        // Variable zum speichern des Datentelegramms (32 Bit)
  private: static volatile uint8_t m_checksum;              // checksum bit 32 - 35
  private: static volatile bool m_TelegramReady;            // Variable zum anzeigen, das die Daten im Puffer vollstaendig sind

  private: uint16_t m_WindSpeed = 0xffff;
  private: uint16_t m_WindDirection = 0xffff;
  private: uint16_t m_WindGust = 0xffff;
  private: uint16_t m_RainVolume = 0xffff;
  private: int16_t m_Temperature = 0xffff;
  private: int16_t m_Dewpoint = 0xffff;
  private: uint8_t m_Humidity = 0xff;
  private: uint16_t m_AbsHumidity = 0xffff;
  private: bool m_BatteryLowW174;
  private: bool m_BatteryLowW132;
  private: uint8_t m_RandomIDW132;
  private: uint8_t m_RandomIDW174;

  private: uint16_t m_NewDataBitset;
  private: void (*m_NewDataCallback)() = NULL;

  public: uint8_t RandomIDFilterW132 = 0;
  public: uint8_t RandomIDFilterW174 = 0;

	//constructors
  public: Ventus_Weathersensors();
  public: Ventus_Weathersensors(int RX433DataPin);

  //functions
  public: void Task();

  public: void AttachNewDataCallback(void (*NewDataCallback)());
  public: uint16_t GetNewDataBitset();

  public: uint16_t GetWindSpeed();
  public: uint16_t GetWindDirection();
  public: uint16_t GetWindGust();
  public: uint16_t GetRainVolume();
  public: int16_t GetTemperature();
  public: uint8_t GetHumidity();
  public: int16_t GetDewpoint();
  public: uint16_t GetAbsoluteHumidity();
  public: bool GetBatteryLowW174();
  public: bool GetBatteryLowW132();
  public: uint8_t GetRandomIDW132();
  public: uint8_t GetRandomIDW174();

  private: static void Rx433Handler();
  private: uint8_t ChecksumW132(uint32_t packet);
  private: uint8_t ChecksumW174(uint32_t packet);
  private: int16_t Convert_12BitSignedValue_Int16(uint16_t inputvalue);
  private: uint16_t ConvertHumidityRelative2Absolute(int16_t temperature, uint8_t relative_humidity);
  private: int16_t CalculateDewpoint(int16_t temperature, uint8_t relative_humidity);
};
