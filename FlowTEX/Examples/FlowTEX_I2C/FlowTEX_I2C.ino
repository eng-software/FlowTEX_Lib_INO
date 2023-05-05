/*
   This example code is in the Public Domain

   This software is distributed on an "AS IS" BASIS, 
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
   either express or implied.

   Este código de exemplo é de uso publico,

   Este software é distribuido na condição "COMO ESTÁ",
   e NÃO SÃO APLICÁVEIS QUAISQUER GARANTIAS, implicitas 
   ou explicitas
*/

//------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------
#include <Wire.h>
#include <FlowTEXI2C.h>

//------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------

//Hardware pins where FlowTEX is connected
#define SDA 17
#define SCL 5

#define FILTER_ALPHA    0.9f   //EMA Filter. Set between 0 ... 0.9999

//------------------------------------------------------------------
// OBJECTS
//------------------------------------------------------------------
FlowTEXI2C  FlowTex20;		//FlowTEX Address @0x20
FlowTEXI2C  FlowTex21;		//FlowTEX Address @0x21

//------------------------------------------------------------------
// VARIABLES
//------------------------------------------------------------------
uint32_t logTimer;
float  flow20;
float  flow21;
uint32_t cnt;
uint32_t sampleTimer;


void setup() 
{
    Serial.begin(115200);
    
	  //Sart I2C
	  Wire.begin(SDA, SCL);
    Wire.setClock(400000UL);  //400KHz for I2C
    FlowTex20.begin(0x20);
    FlowTex21.begin(0x21);

    //Start log timer
    logTimer = millis();

    //Sampling control
    cnt = 0;
    sampleTimer = millis();
}

void loop() 
{
    //Sample control to avoid sampling overrun on sensor
    //FlowTEX should not be sampled more than 1000sps
    if( getElapsedMillis(sampleTimer) >= 1)
    {
        sampleTimer = millis();
        flow20 = (flow20*FILTER_ALPHA) + ((1.0f - FILTER_ALPHA)*FlowTex20.getFlow());
        flow21 = (flow21*FILTER_ALPHA) + ((1.0f - FILTER_ALPHA)*FlowTex21.getFlow());
        cnt++;
    }
  
    if( getElapsedMillis(logTimer) >= 1000)
    {
        Serial.println("*** FLOWTEX 0x20 ****");
        Serial.print("Flow:");
        Serial.print(flow20);
        Serial.print("\r\n");

        Serial.print("Temperature:");
        Serial.print(FlowTex20.getTemperature());
        Serial.print("\r\n");

        Serial.print("Serial Number: ");
        Serial.print(FlowTex20.getSerialNumber());
        Serial.print("\r\n");

        Serial.print("Version: ");
        Serial.print(FlowTex20.getVersion());
        Serial.print("\r\n");

        Serial.print("Errors: ");
        Serial.print(FlowTex20.getFailCounter());
        Serial.print("\r\n");

        Serial.print("Success: ");
        Serial.print(FlowTex20.getSuccessCounter());
        Serial.print("\r\n");

        Serial.println("*******\r\n\r\n");


        Serial.println("*** FLOWTEX 0x21 ****");
        Serial.print("Flow:");
        Serial.print(FlowTex21.getFlow());
        Serial.print("\r\n");

        Serial.print("Temperature:");
        Serial.print(flow21);
        Serial.print("\r\n");

        Serial.print("Serial Number: ");
        Serial.print(FlowTex21.getSerialNumber());
        Serial.print("\r\n");

        Serial.print("Version: ");
        Serial.print(FlowTex21.getVersion());
        Serial.print("\r\n");

        Serial.print("Errors: ");
        Serial.print(FlowTex21.getFailCounter());
        Serial.print("\r\n");

        Serial.print("Success: ");
        Serial.print(FlowTex21.getSuccessCounter());
        Serial.print("\r\n");

        Serial.println("*******\r\n\r\n");

        Serial.print("Leituras/s =");
        Serial.print(cnt);
        Serial.print("\r\n\r\n");
        cnt = 0;        

        //Restart timer
        logTimer = millis();
    }
}
