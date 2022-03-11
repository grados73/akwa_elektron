#include "D:\ESP\Blynk__free_michelle_try1\inc\parse.h"

using namespace std;

typedef uint8_t TypeOfParsingData;

void UART_ParseLED();

extern ParseClass UARTuCParse;

uint8_t ParseClass::Parse(String * command)
{

    #if DEBUGING_MODE
    Serial.print("CMDRCV\n");
    #endif

    uint8_t TypeOfCommand = 0;
    String NewComand = *command;

    //Information about current Temperature inside tank
    //TEMPINS=25.5
    if(NewComand.indexOf("TEMPINS") >= 0) // TODO! Check "TEMP" or 'TEMP'
    {
        #if DEBUGING_MODE
        Serial.print("TEMPINSCMDRCV\n");
        #endif

        uint8_t PositionOfEqual = NewComand.indexOf("=");
        if (PositionOfEqual >= 0) // If Equal sign exist 
        {
            String STempI = NewComand.substring(PositionOfEqual+1);
            float FTempI = STempI.toFloat();
            UARTuCParse.TempIns = FTempI;
            TypeOfCommand = PARSING_TEMPWEW;
        }
    }
    //Information about current Temperature outside the tank
    //TEMPINS=25.5
    else if(NewComand.indexOf("TEMPOUT") >= 0) 
    {
        #if DEBUGING_MODE
        Serial.print("TEMPOUTCMDRCV\n");
        #endif

        uint8_t PositionOfEqual = NewComand.indexOf("=");
        if (PositionOfEqual >= 0) // If Equal sign exist 
        {
            String STempO = NewComand.substring(PositionOfEqual+1);
            float FTempO = STempO.toFloat();
            UARTuCParse.TempOut = FTempO;
            TypeOfCommand = PARSING_TEMPZEW;
        }
    }
    //Information about current Pressure
    //PRES=1014.5
    else if(NewComand.indexOf("PRES") >= 0)
    {
        #if DEBUGING_MODE
        Serial.print("PRESCMDRCV\n");
        #endif

        uint8_t PositionOfEqual = NewComand.indexOf("=");
        if (PositionOfEqual >= 0) // If Equal sign exist 
        {
            String SPres = NewComand.substring(PositionOfEqual+1);
            float FPres = SPres.toFloat();
            UARTuCParse.Press = FPres;
            TypeOfCommand = PARSING_PRESS;
        }
    }
    //Command to Turn Relay ON
    //RELAYON=1
    else if(NewComand.indexOf("RELAYON") >= 0)
    {
        #if DEBUGING_MODE
        Serial.print("RONCMDRCV\n");
        #endif

        uint8_t PositionOfEqual = NewComand.indexOf("=");
        if (PositionOfEqual >= 0) // If Equal sign exist 
        {
            String SRelayNr = NewComand.substring(PositionOfEqual+1);
            uint8_t IRelayNr = SRelayNr.toInt();
            UARTuCParse.NrOfLastChangedRelay = IRelayNr;
            TypeOfCommand = PARSING_RELAY_ON;
        }
    }
    //Command to Turn Relay OFF
    //RELAYOFF=1
    else if(NewComand.indexOf("RELAYOFF") >= 0)
    {
        #if DEBUGING_MODE
        Serial.print("ROFFCMDRCV\n");
        #endif

        uint8_t PositionOfEqual = NewComand.indexOf("=");
        if (PositionOfEqual >= 0) // If Equal sign exist 
        {
            String SRelayNr = NewComand.substring(PositionOfEqual+1);
            uint8_t IRelayNr = SRelayNr.toInt();
            UARTuCParse.NrOfLastChangedRelay =  IRelayNr;
            TypeOfCommand = PARSING_RELAY_OFF;          
        }
    }
    return TypeOfCommand;
}

void ParseClass::SendDataToBlynkServer()
{

}

// Parsing testing function to change state of LED
// "LED=1\n"
void UART_ParseLED()
{

}