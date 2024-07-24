#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#define ONE_WIRE_BUS 8 //pin pentru senzor temperatura 
#define internalLedPin 13 //pin led intern de pe placuta 
#define waterSensorPin A0 //pin pentru senzorul de apa

const int waterThreshold = 100; 
bool inundatieDetectata = false;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

const int maxMessages = 10;
const int messageSize = 2;

String messages[maxMessages]; 

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];

  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';

  return String(data);
}

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  pinMode(internalLedPin, OUTPUT);

  for (int i = 0; i < maxMessages; i++) {
    messages[i] = readStringFromEEPROM(i * messageSize);
  }

  if (messages[maxMessages - 1] == "A") {
    digitalWrite(internalLedPin, HIGH);
  }
  else if(messages[maxMessages - 1] == "S")
  {
    digitalWrite(internalLedPin, LOW);
  }
}

void loop(void){ 

  sensors.requestTemperatures(); 
  Serial.print("\nTemperatura celsius: ");
  Serial.print(sensors.getTempCByIndex(0));
  int waterSensorValue = analogRead(waterSensorPin);
 

  if (waterSensorValue > waterThreshold && !inundatieDetectata) {
    Serial.print("\n");
    Serial.print("Inundatie detectata!");
    inundatieDetectata = true;
  }

  if (waterSensorValue < waterThreshold && inundatieDetectata) {
    inundatieDetectata = false;
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input == "A" || input == "S") {
      // Se adaugă noul mesaj la începutul listei
      for (int i = 0; i < maxMessages - 1; i++) {
        messages[i] = messages[i + 1];
      }

      messages[9] = input;

      for (int i = 0; i < maxMessages; i++) {
        writeStringToEEPROM(i * messageSize, messages[i]);
      }
    }
    if (input == "A") {
      digitalWrite(internalLedPin, HIGH);
    } 
    else if (input == "S") {
      digitalWrite(internalLedPin, LOW);
    }
  }
 delay(100);
}
