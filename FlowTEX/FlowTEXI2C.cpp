/*
   This example code is in the Public Domain

   This software is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
   either express or implied.

   Este c�digo de exemplo � de uso publico,

   Este software � distribuido na condi��o "COMO EST�",
   e N�O S�O APLIC�VEIS QUAISQUER GARANTIAS, implicitas
   ou explicitas
*/

//------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------
#include "FlowTEXI2C.h"

//------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------
FlowTEXI2C::FlowTEXI2C()
{
    ptWire = &Wire;
    address = 0x20;
    resetCounters();
}

void FlowTEXI2C::begin()
{
    resetCounters();
}

void FlowTEXI2C::begin(uint8_t addr)
{
    address = addr;    
    resetCounters();
}

void FlowTEXI2C::begin(TwoWire &twoWire)
{
    ptWire = &twoWire;
    resetCounters();
}

void FlowTEXI2C::begin(TwoWire &twoWire, uint8_t addr)
{
    address = addr;
    ptWire = &twoWire;
    resetCounters();
}

float FlowTEXI2C::getFlow()
{
    uint8_t retry = 3;
    while(retry--)
    {
        uint8_t buffer[sizeof(tFlowTEXI2CTable::flowFloat) + 1];
        if(masterRead(address, (uint8_t)eFlowTEXRegs::eFLOWFLOAT, buffer, sizeof(buffer)))
        {
            uint8_t chks = 0;
            for(uint8_t i = 0; i < sizeof(buffer); i++)
            { chks += buffer[i];  }

            if(chks == 0)
            {
                successCounter++;
                float result;
                memcpy(&result, buffer, sizeof(float));
                return result;
            }

            failCounter++;
        }
		else
		{
			failCounter++;
		}

    }

    return 0;
}

float FlowTEXI2C::getTemperature()
{
    uint8_t retry = 3;
    while(retry--)
    {
        uint8_t buffer[sizeof(tFlowTEXI2CTable::temp) + 1];
        if(masterRead(address, (uint8_t)eFlowTEXRegs::eTEMP, buffer, sizeof(buffer)))
        {
            uint8_t chks = 0;
            for(uint8_t i = 0; i < sizeof(buffer); i++)
            { chks += buffer[i];  }

            if(chks == 0)
            {
                successCounter++;
                int16_t result;
                memcpy(&result, buffer, sizeof(int16_t));
                return  (float)result/100.0f;
            }

            failCounter++;
        }
		else
		{
			failCounter++;
		}
            
    }

    return 0;
}

const char *FlowTEXI2C::getSerialNumber()
{
    uint8_t retry = 3;
    while(retry--)
    {
        uint8_t buffer[sizeof(tFlowTEXI2CTable::serialNumber) + 1];
        if(masterRead(address, (uint8_t)eFlowTEXRegs::eSERIALNUMBER, buffer, sizeof(buffer)))
        {
            uint8_t chks = 0;
            for(uint8_t i = 0; i < sizeof(buffer); i++)
            { chks += buffer[i];  }

            if(chks == 0)
            {
                successCounter++;
                float result;
                memcpy(serialNumber, buffer, sizeof(tFlowTEXI2CTable::serialNumber));
                serialNumber[sizeof(tFlowTEXI2CTable::serialNumber)] = 0;
                return serialNumber;
            }

            failCounter++;
        }
		else
		{
			failCounter++;
		}

    }

    memset(serialNumber, 0, sizeof(serialNumber));
    return serialNumber;
}

const char *FlowTEXI2C::getVersion()
{
    uint8_t retry = 3;
    while(retry--)
    {
        uint8_t buffer[sizeof(tFlowTEXI2CTable::ftVersion) + 1];
        if(masterRead(address, (uint8_t)eFlowTEXRegs::eVERSION, buffer, sizeof(buffer)))
        {
            uint8_t chks = 0;
            for(uint8_t i = 0; i < sizeof(buffer); i++)
            { chks += buffer[i];  }

            if(chks == 0)
            {
                successCounter++;
                snprintf(ftVersion, sizeof(ftVersion),"%d.%d.%d.%d", buffer[0], buffer[1], buffer[2], buffer[3]); 
                return ftVersion;
            }

            failCounter++;
        }
		else
		{
			failCounter++;
		}
		
    }

    memset(ftVersion, 0, sizeof(ftVersion));
    return ftVersion;
}

bool FlowTEXI2C::masterRead(uint8_t address, uint8_t dataAddress, uint8_t *ptBuffer, uint32_t len )
{
    ptWire->beginTransmission(address);                                
    ptWire->write(dataAddress);
    ptWire->endTransmission();
  
    uint32_t rxLen = 0;
    ptWire->requestFrom((uint8_t)address, (uint8_t)len, (uint8_t)true);    // request 'len' bytes from slave device
  
    while(ptWire->available())    // slave may send less than requested
    {
        if(rxLen < len)
        {
            *ptBuffer++ = ptWire->read();    // receive a byte as character
        }     

        rxLen++;
    }
    return len == rxLen;
}

uint32_t FlowTEXI2C::getSuccessCounter()
{
    return successCounter;
}

uint32_t FlowTEXI2C::getFailCounter()
{ 
    return failCounter;
}

void FlowTEXI2C::resetCounters()
{
    successCounter = 0;
    failCounter = 0;
}
