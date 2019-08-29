#pragma once

class Ventus_Weathersensors
{
	//members

	//constructors
	public: Ventus_Weathersensors();

	//functions
	public: void Cyclic();

  private: void Process_Telegramm();
};
