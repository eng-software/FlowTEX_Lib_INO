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
#ifndef __FLOWTEXI2C_H__
#define __FLOWTEXI2C_H__

//------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------
#include <Wire.h>
#include <cstdint>
#include "TexFunctions.h"

//------------------------------------------------------------------
// ENUMS
//------------------------------------------------------------------
enum class eFlowTEXRegs
{
  eFLOW                = 0,      //0..2
  eFLOWCHKS            = 3,      //3
  eTEMP                = 4,      //4..5
  eTEMPCHKS            = 6,      //6
  eFULLSCALE           = 7,      //7..9
  eFULLSCALECHKS       = 10,     //10
  eSERIALNUMBER        = 11,     //11..20
  eSERIALNUMBERCHKS    = 21,     //21
  eVERSION             = 22,     //22..25
  eVERSIONCHKS         = 26,     //26
  eFWCHKS              = 27,     //27..30
  eFWCHKSCHKS          = 31,     //31
  eRANGE               = 32,     //32..34
  eRANGECHKS           = 35,     //35
  eRANGEFLOAT          = 36,     //36..39
  eRANGEFLOATCHKS      = 40,     //40
  eFULLSCALEFLOAT      = 41,     //41..44
  eFULLSCALEFLOATCHKS  = 45,     //45
  eFLOWFLOAT           = 46,     //46..49
  eFLOWFLOATCHKS       = 50,     //50
};

//------------------------------------------------------------------
// TYPEDEFS
//------------------------------------------------------------------
typedef struct tFlowTEXI2CTable
{                                   //Address range
    uint8_t flow[3];                //0..2
    uint8_t flowChks;               //3
    uint8_t temp[2];                //4..5
    uint8_t tempChks;               //6
    uint8_t fullScale[3];           //7..9
    uint8_t fullScaleChks;          //10
    uint8_t serialNumber[10];       //11..20
    uint8_t serialNumberChks;       //21
    uint8_t ftVersion[4];             //22..25
    uint8_t versionChks;            //26
    uint8_t fwChks[4];              //27..30
    uint8_t fwChksChks;             //31

    uint8_t range[3];               //32..34
    uint8_t rangeChks;              //35
    uint8_t rangeFloat[4];          //36..39
    uint8_t rangeFloatChks;         //40
    uint8_t fullScaleFloat[4];      //41..44
    uint8_t fullScaleFloatChks;     //45
    uint8_t flowFloat[4];           //46..49
    uint8_t flowFloatChks;          //50
}tFlowTEXI2CTable;

//------------------------------------------------------------------
// CLASSES
//------------------------------------------------------------------
class FlowTEXI2C
{
  private:
    TwoWire *ptWire;
    uint8_t address;
    char serialNumber[sizeof(tFlowTEXI2CTable::serialNumber)+1];
    char ftVersion[10+1];
    uint32_t successCounter;
    uint32_t failCounter;

  public:
    FlowTEXI2C();
    void begin();
    void begin(uint8_t addr);
    void begin(TwoWire &twoWire);
    void begin(TwoWire &twoWire, uint8_t addr);
    float getFlow();
    float getTemperature();
    const char * getSerialNumber();
    const char * getVersion();
    uint32_t getSuccessCounter();
    uint32_t getFailCounter();
    void resetCounters();

  private:
    bool masterRead(uint8_t address, uint8_t dataAddress, uint8_t *ptBuffer, uint32_t len );
};

#endif
