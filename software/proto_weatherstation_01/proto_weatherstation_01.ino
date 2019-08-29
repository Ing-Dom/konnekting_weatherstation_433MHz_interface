/* Developement ToDos

  1) check if it works out-of-the-box                     
  2) add Temp and Hum to sketch with current nano board   DONE
  3) check if it works                                    
  4) make a class for reading that stuff...               
  5) port it to ItsyBitsy                                 
  6) add knx and historical value storing stuff           


*/


#define RX433DATAPIN 2    // Pin 2 vom Nano ist der Dateneingang vom Funkmodul RXB6 (Superheterodyne-Empfaenger)
#define RX433INTERRUPT 0  // Interrupt 0 ist der dazugehoerige Hardware-Interrupt
#define DEBUG 1           // mit 1 werden die Debug-Meldungen auf dem seriellen Monitor angezeigt

volatile unsigned long rxBuffer; // Variable zum speichern des Datentelegramms (32 Bit)
volatile bool rxOk = false;      // Variable zum anzeigen, das die Daten im Puffer vollstaendig sind
char printbuffer[64];            // Variable zum formatieren der Ausgabe ueber die serielle Schnittstelle

uint16_t WindSpeed, WindDirection, WindGust, RainVolume; // Variablen zum speichern der Daten
int16_t Temperature;
uint8_t Humidity;
byte BatteryState = 0; // der Batterie-Status von beiden Sensoren (Bit 0 = Windsensor und Bit 1 = Regensensor)

void rx433Handler() {  // Interrupt-Routine
  if (rxOk) return;    // wenn rxOk noch gesetzt ist, dann Funktion verlassen, damit der Puffer nicht ueberschrieben wird
  static unsigned long rxHigh = 0, rxLow = 0;
  static bool syncBit = 0, dataBit = 0;
  static byte rxCounter = 0;
  bool rxState = digitalRead(RX433DATAPIN); // Daten-Eingang auslesen
  if (!rxState) {                           // wenn Eingang = Low, dann...
    rxLow = micros();                       // Mikrosekunden fuer Low merken
  } else {                                  // ansonsten (Eingang = High)
    rxHigh = micros();                      // Mikrosekunden fuer High merken
    if (rxHigh - rxLow > 8500) { // High-Impuls laenger als 8.5 ms dann SyncBit erkannt (9 ms mit 0.5 ms Toleranz)
      syncBit = 1;               // syncBit setzen
      rxBuffer = 0;              // den Puffer loeschen
      rxCounter = 0;             // den Counter auf 0 setzen
      return;                    // auf den naechsten Interrupt warten (Funktion verlassen)
    }
    if (syncBit) {                                          // wenn das SyncBit erkannt wurde, dann High-Impuls auswerten:
      if (rxHigh - rxLow > 1500) dataBit = 0;               // High-Impuls laenger als 1.5 ms (2 ms mit 0.5 ms Tolenz), dann DataBit = 0
      if (rxHigh - rxLow > 3500) dataBit = 1;               // High-Impuls laenger als 3.5 ms (4 ms mit 0.5 ms Tolenz), dann DataBit = 1
      if (rxCounter < 32) {                                 // Wenn noch keine 32 Bits uebertragen wurden, dann...
        rxBuffer |= (unsigned long) dataBit << rxCounter++; // das Datenbit in den Puffer schieben und den Counter erhoehen
      } else {         // wenn das Datentelegramm (32 Bit) vollstaendig uebertragen wurde, dann...
        rxCounter = 0; // den Counter zuruecksetzen
        syncBit = 0;   // syncBit zuruecksetzen
        rxOk = true;   // Ok signalisieren (rxBuffer ist vollstaendig) fuer die Auswertung in Loop
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RX433DATAPIN, INPUT);
  attachInterrupt(RX433INTERRUPT, rx433Handler, CHANGE);
  if (DEBUG) Serial.println(F("Start"));
}

void loop()
{
  if (rxOk)
  {
    // wenn die Interrupt-Routine rxOk auf true gesetzt hat, dann ist der Puffer mit den Daten gefuellt
    byte randomID = (unsigned long) rxBuffer & 0xff;           // die ersten 8 Bits enthalten eine Zufalls-ID (wird beim Batteriewechsel neu generiert)
    bool bState = (unsigned long) (rxBuffer >> 8) & 0x1;       // wenn Bit 8 gesetzt ist, sind die Batterien schwach
    byte xBits = (unsigned long) (rxBuffer >> 9) & 0x3;        // die Bits 9 und 10 sind immer 1 (Wert von xBits = 3)
    bool buttonState = (unsigned long) rxBuffer >> 11 & 0x1;   // wenn Bit 11 gesetzt ist, wurde die Taste am Sensor betaetigt
    byte subID = (unsigned long) (rxBuffer >> 12) & 0x7;       // die Bits 12, 13, 14 enthalten eine Sub-ID (Windmesser sendet 2 Telegramme Sub-ID 1 und 7)
    
    //byte checksum = (unsigned long) (rxBuffer >> 12) & 0x7;       // die Bits 12, 13, 14

    /* 
     *  Hinweis!
     *  Weil sich die Zufalls-ID bei jedem Batteriewechsel aendert, werden nur die xBits (sind immer gesetzt),
     *  das Bit 4 der Zufalls-ID (ist immer Null) und die subID benutzt, um die richtigen Sensoren zu finden.
     *  Das sollte ausreichen, solange sich nicht eine gleiche Wetterstation in Reichweite befindet.
     */
    if (xBits == 3 && !(randomID & 0x10))
    {
      // wenn die xBits gesetzt sind und Bit 4 der Zufalls-ID Null ist (ist beim Wind- und Regensensor immer der Fall)
      if (DEBUG)
      {
        Serial.print(F("RandomID: "));
        Serial.print(randomID);
        Serial.print(F("  SubID: "));
        Serial.println(subID);
        Serial.print(F("xBits: "));
        Serial.println(xBits);
        Serial.print(F("bState: "));
        Serial.println(bState);
      }
      if (subID == 1)
      {
        // subID = 1 ist der Windsensor (Durchschnitts-Windgeschwindigkeit)
        bState ? BatteryState |= 1 : BatteryState &= 0; // wenn die Batterien schwach sind, Bit 0 von batteryState auf 1 setzen
        WindSpeed = ((unsigned long) (rxBuffer >> 24) & 0xff) * 2; // die Bits 24-31 enthalten die durchschnittliche Windgeschwindigkeit in
                                                                   // Einheiten zu 0.2 m/s und mal 10 ergibt: "* 2" fuer einen Integerwert
        uint16_t wsh = WindSpeed * 36; // Windgeschwindigkeit in km/h umrechnen
        if (DEBUG) {
          Serial.print(F("Windgeschwindigkeit (/): "));
          snprintf(printbuffer, 28, "%d.%d m/s  (%d.%d km/h)", (WindSpeed / 10), (WindSpeed % 10), (wsh / 100), (wsh % 100));
          Serial.println(printbuffer);
        }
      }
      if (subID == 7)
      {
        // subID = 7 ist der Windsensor (Windrichtung und Windboen)
        bState ? BatteryState |= 1 : BatteryState &= 0; // wenn die Batterien schwach sind, Bit 0 von batteryState auf 1 setzen
        uint16_t wdir = (unsigned long) (rxBuffer >> 15) & 0x1ff;    // die Bits 15-23 enthalten die Windrichtung in Grad (0-360)
        if (wdir <= 360) WindDirection = wdir; // die Windrichtung wird manchmal falsch uebertragen, deshalb nur Daten bis 360 Grad uebernehmen
        uint16_t wg = ((unsigned long) (rxBuffer >> 24) & 0xff) * 2; // die Bits 24-31 enthalten die Windboen in
                                                                     // Einheiten zu 0.2 m/s und mal 10 ergibt: "* 2" fuer einen Integerwert
        if (wg < 500) WindGust = wg;  // die Windboen werden manchmal falsch uebertragen, deshalb nur Daten unter 50m/s (180km/h) uebernehmen
        uint16_t wgh = WindGust * 36; // Windgeschwindigkeit in km/h umrechnen
        if (DEBUG) {
          Serial.print(F("Windrichtung: "));
          snprintf(printbuffer, 12, "%3d Grad", WindDirection);
          Serial.println(printbuffer);
          Serial.print(F("Windboen: "));
          snprintf(printbuffer, 28, "%d.%d m/s  (%d.%d km/h)", (WindGust / 10), (WindGust % 10), (wgh / 100), (wgh % 100));
          Serial.println(printbuffer);
        }
      }
      if (subID == 3)
      {
        // subID = 3 ist der Regensensor (absolute Regenmenge seit Batteriewechsel)
        bState ? BatteryState |= 2 : BatteryState &= 0; // wenn die Batterien schwach sind, Bit 1 von batteryState auf 1 setzen
        RainVolume = ((unsigned long) (rxBuffer >> 16) & 0xffff) * 25; // die Bits 16-31 enthalten die Niederschlagsmenge
                                                                       // in Einheiten zu je 0.25 mm -> 1 mm = 1 L/m2
                                                                       // hier zusaetzlich mal 10 um einen Integerwert zu erhalten
        if (DEBUG)
        {
          Serial.print(F("Regenmenge: "));
          snprintf(printbuffer, 12, "%d.%d mm", (RainVolume / 100), (RainVolume % 100));
          Serial.println(printbuffer);
        }
      }
      if (DEBUG)
        Serial.print(F("zum PC senden: "));
      /*
       * Daten, die zum PC gesendet werden:
       * BatteryState enhaelt den Batterie-Status der beiden Sensoren (Bit 0 und/oder Bit 1 gesetzt = Batterien schwach)
       * WindSpeed enthaelt die durchschnittliche Windgeschwindigkeit mal 10 (in m/s)
       * WindDirection enthaelt die Windrichtung (in Grad)
       * WindGust enthaelt die Windboen mal 10 (in m/s)
       * RainVolume enthaelt die absolute Regenmenge seit Batteriewechsel mal 100 (in mm)
       */
      snprintf(printbuffer, 26, "%u,%u,%u,%u,%u#", BatteryState, WindSpeed, WindDirection, WindGust, RainVolume);
      Serial.println(printbuffer);
      if (DEBUG)
        Serial.println();
    }
    else if(!(randomID & 0x10))
    {
      // wenn die xBits nicht beide gesetzt sind und Bit 4 der Zufalls-ID Null ist (ist beim Wind- und Regensensor immer der Fall)
      // Telegramm mit temp und hum

      if (DEBUG)
      {
        Serial.print(F("RandomID: "));
        Serial.print(randomID);
        Serial.print(F("xBits: "));
        Serial.println(xBits);
        Serial.print(F("bState: "));
        Serial.println(bState);
      }
      
      bool temp_negative = (unsigned long) (rxBuffer >> 12) & 0x1;
      uint16_t temp_absval = ((unsigned long) (rxBuffer >> 13) & 0x07ff);
      if(temp_negative)
      {
        // ToDo
        Temperature = -1000;
      }
      else
      {
        Temperature = temp_absval;
      }

      int16_t Temperature2 = Convert_12BitSignedValue_Int16(((unsigned long) (rxBuffer >> 12) & 0x0fff));


      if (DEBUG)
      {
        Serial.print(F("Temperatur (1): "));
        snprintf(printbuffer, 23, "%2d.%d °C", (Temperature / 10), (abs(Temperature % 10)));
        Serial.println(printbuffer);

        Serial.print(F("Temperatur (2): "));
        snprintf(printbuffer, 23, "%2d.%d °C", (Temperature2 / 10), (abs(Temperature2 % 10)));
        Serial.println(printbuffer);
      }
      
      uint8_t rh_ones = ((unsigned long) (rxBuffer >> 24) & 0x10);
      uint8_t rh_tens = ((unsigned long) (rxBuffer >> 28) & 0x10);
      Humidity = rh_ones + rh_tens*10;

      if (DEBUG)
      {
        Serial.print(F("Humidity      : "));
        snprintf(printbuffer, 20, "%2d %", Humidity);
        Serial.println(printbuffer);
      }
    }
    rxBuffer = 0; // den Puffer loeschen
    rxOk = false; // Auswertung beendet, dann rxOk zuruecksetzen fuer die Interrupt-Routine
  }
}

int16_t Convert_12BitSignedValue_Int16(uint16_t inputvalue)
{
  inputvalue = inputvalue & 0x0fff;  // make sure the value is realy just 12bit

  if((inputvalue >> 4) & 0x01)  // sign bit is setup
  {
    // ToDo
    // value is negative

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