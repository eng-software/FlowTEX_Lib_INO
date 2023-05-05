
#include <FlowTEX.h>

//---------------------------------------------
// DEFINES
//---------------------------------------------

//Serial port 1 that will comunicate with FlowTEX
#define TX1   26
#define RX1   27

//---------------------------------------------
// OBJECTS
//---------------------------------------------
FlowTEX FlowTex;  //The FlowTEX Object

//---------------------------------------------
// VARIABLES
//---------------------------------------------
uint32_t logTimer;    //Timer for periodic log

void setup() 
{
    Serial.begin(115200, SERIAL_8N1 );

    //FlowTEX is conected at serial 1
    Serial1.begin(115200, SERIAL_8N1, RX1, TX1);    
    FlowTex.begin(Serial1); //Set Serial1 as the resource port
    
    //Start log timer
    logTimer = millis();
}

void loop() 
{
    //ATENTION!!
    //You need to poll FlowTEX object periodically to ensure
    //the healtrh of comunication stack 
    FlowTex.poll();  

    if( getElapsedMillis(logTimer) >= 1000)
    {
        Serial.print("Flow:");
        Serial.print(FlowTex.getFlow());
        Serial.print("\r\n");

        Serial.print("Temperature:");
        Serial.print(FlowTex.getTemperature());
        Serial.print("\r\n");

        Serial.print("Serial Number: ");
        Serial.print(FlowTex.getSerialNumber());
        Serial.print("\r\n");
        
        Serial.print("Model: ");
        Serial.print(FlowTex.getModel());
        Serial.print("\r\n");

        Serial.print("Version: ");
        Serial.print(FlowTex.getVersion());
        Serial.print("\r\n");

        //Restart timer
        logTimer = millis();
    }
}
