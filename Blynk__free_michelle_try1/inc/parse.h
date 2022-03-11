#pragma once

#include <ESP8266WiFi.h>
#include <string>

#define DEBUGING_MODE 0

#define PARSING_TEMPWEW 1
#define PARSING_TEMPZEW 2
#define PARSING_PRESS 3
#define PARSING_WLVL 6
#define PARSING_RELAY_ON 4
#define PARSING_RELAY_OFF 5


typedef uint8_t TypeOfParsingData;

class ParseClass {

public:
 int RcvCmd;
 float TempIns;
 float TempOut;
 float Press;
 int WLVL;
 bool Relay1;
 bool Relay2;
 bool Relay3;
 bool Relay4;
 uint8_t NrOfLastChangedRelay;

 uint8_t Parse(String * command);
 void SendDataToBlynkServer();
};

class RelayClass {

public:
 uint8_t Number;
 bool State;

 void TurnOn();
 void TurnOff();
};

