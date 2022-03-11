/*************************************************************
* author:    grados73
* project:   free_michelle_blynk
 *************************************************************/

/*************************************************************
 ********* PART OF CODE NEEDED TO BLYNK INIT *****************
 *************************************************************/
// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLS6OU4IQQ"
#define BLYNK_DEVICE_NAME           "GRAD Quickstart Device"
#define BLYNK_AUTH_TOKEN            "6-UIFG5EkFyAbtLATzif3RagR20M8ey2"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "D:\ESP\Blynk__free_michelle_try1\inc\parse.h"
#include "D:\ESP\Blynk__free_michelle_try1\inc\Blynk_functions.h"

char auth[] = BLYNK_AUTH_TOKEN;

#define TELEPHONE_HOTSPOT 0

// Your WiFi credentials.
// Set password to "" for open networks.

char ssid[] = "SuperExtraSzybkiInternetZuzy"; 
char pass[] = "gradowscy6";



WidgetLED led1(V1);

BlynkTimer timer;



// Real - Build In BLUE LED Declaration
#define LED 2

// Variables to Serial comunication
String inputString = "";                 // a String to hold incoming data
String * WskInputString = &inputString;  // Pointer on String holding incoming data
bool stringComplete = false;             // whether the string is complete

// Objects with data and methods to Parse Command from uC
ParseClass UARTuCParse;
// Variable to hold information about last receive command
uint8_t LastCommand;
void SendDataToBlynk(uint8_t CommandID);

// Objects with data and methods to get information about Relay from BLYNK Server
RelayClass Relay1;
RelayClass Relay2;
RelayClass Relay3;
RelayClass Relay4;
RelayClass Relay5;
RelayClass Relay6;
RelayClass Relay7;
RelayClass Relay8;

BLYNK_CONNECTED() {
  //get data stored in virtual pin V0 from server
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V14);
  Blynk.syncVirtual(V15);
  Blynk.syncVirtual(V16);
  Blynk.syncVirtual(V17);

  #if DEBUGING_MODE
  Serial.print("RSTATE=");
  Serial.print(Relay1.State);
  Serial.print(Relay2.State);
  Serial.print(Relay3.State);
  Serial.print(Relay4.State);
  Serial.print(Relay5.State);
  Serial.print(Relay6.State);
  Serial.print(Relay7.State);
  Serial.print(Relay8.State);
  Serial.print("\n");
  #endif
}

BLYNK_WRITE(V8) //Button Widget is writing to pin V8 - Relay1
{
  Relay1.State = param.asInt(); 
  Serial.print("CHRELAY=1,");
  Serial.print(Relay1.State);
  Serial.print("\n");
}
BLYNK_WRITE(V9) //Button Widget is writing to pin V9 - Relay2
{
  Relay2.State = param.asInt(); 
  Serial.print("CHRELAY=2,");
  Serial.print(Relay2.State);
  Serial.print("\n");
}
BLYNK_WRITE(V10) //Button Widget is writing to pin V10 - Relay3
{
  Relay3.State = param.asInt(); 
  Serial.print("CHRELAY=3,");
  Serial.print(Relay3.State);
  Serial.print("\n");
}
BLYNK_WRITE(V11) //Button Widget is writing to pin V11 - Relay4
{
  Relay4.State = param.asInt(); 
  Serial.print("CHRELAY=4,");
  Serial.print(Relay4.State);
  Serial.print("\n");
}

BLYNK_WRITE(V14) //Button Widget is writing to pin V14 - Light1
{
  Relay4.State = param.asInt(); 
  Serial.print("CHRELAY=5,");
  Serial.print(Relay4.State);
  Serial.print("\n");
}
BLYNK_WRITE(V15) //Button Widget is writing to pin V15 - Light2
{
  Relay4.State = param.asInt(); 
  Serial.print("CHRELAY=6,");
  Serial.print(Relay4.State);
  Serial.print("\n");
}

BLYNK_WRITE(V16) //Button Widget is writing to pin V16 - Light3
{
  Relay4.State = param.asInt(); 
  Serial.print("CHRELAY=7,");
  Serial.print(Relay4.State);
  Serial.print("\n");
}

BLYNK_WRITE(V17) //Button Widget is writing to pin V17 - Light4
{
  Relay4.State = param.asInt(); 
  Serial.print("CHRELAY=8,");
  Serial.print(Relay4.State);
  Serial.print("\n");
}


void setup()
{
  //  Serial Init
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  //  Initialization of Blynk Cloud access
  Serial.print("BLYNKINITSTART\n");
  Blynk.begin(auth, ssid, pass);
  //TODO! ADD implementation of possibility to change WIFI ssid or pass to connect!
  Serial.print("BLYNKINITSUCCESS\n");
  //  Real - Build In BLUE LED Declaration
  pinMode(LED, OUTPUT);
}

void loop()
{
  Blynk.run();
  timer.run();
  if (stringComplete) {
    // PARSE COMMAND FUNCTION 
    LastCommand = UARTuCParse.Parse(WskInputString);
    SendDataToBlynk(LastCommand);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


void SendDataToBlynk(uint8_t CommandID)
{
  switch (CommandID)
  {
    case PARSING_TEMPWEW:
      Blynk.virtualWrite(V6, UARTuCParse.TempIns);
      break;
      
    case PARSING_TEMPZEW:
      Blynk.virtualWrite(V7, UARTuCParse.TempOut);
      break;
    
    case PARSING_PRESS:
      Blynk.virtualWrite(V5, UARTuCParse.Press);
      break;
    
    case PARSING_RELAY_ON:
      switch (UARTuCParse.NrOfLastChangedRelay)
      {
        case 1:
          Blynk.virtualWrite(V8, 1);
          break;
        case 2:
          Blynk.virtualWrite(V9, 1);
          break;
        case 3:
          Blynk.virtualWrite(V10, 1);
          break;
        case 4:
          Blynk.virtualWrite(V11, 1);
          break;
        case 5:
          Blynk.virtualWrite(V14, 1);
          break;
        case 6:
          Blynk.virtualWrite(V15, 1);
          break;
        case 7:
          Blynk.virtualWrite(V16, 1);
          break;
        case 8:
          Blynk.virtualWrite(V17, 1);
          break;                              
        default:
          break;
      }
      break;
    
    case PARSING_RELAY_OFF:
    switch (UARTuCParse.NrOfLastChangedRelay)
      {
        case 1:
          Blynk.virtualWrite(V8, 0);
          break;
        case 2:
          Blynk.virtualWrite(V9, 0);
          break;
        case 3:
          Blynk.virtualWrite(V10, 0);
          break;
        case 4:
          Blynk.virtualWrite(V11, 0);
          break;
        case 5:
          Blynk.virtualWrite(V14, 0);
          break;
        case 6:
          Blynk.virtualWrite(V15, 0);
          break;
        case 7:
          Blynk.virtualWrite(V16, 0);
          break;
        case 8:
          Blynk.virtualWrite(V17, 0);
          break; 
        default:
          break;
      }
      break;

    default:
      break;
  }
}
