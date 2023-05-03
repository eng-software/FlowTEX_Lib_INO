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
#ifndef __FLOWTEX_H__
#define __FLOWTEX_H__

//------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------
#include <cstdint>
#include <Stream.h>
#include "TexNET.h"

//------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------
#define FLOWTEX_VERSION_LEN     10
#define FLOWTEX_MODEL_LEN       20
#define FLOWTEX_SERIALN_LEN     10

//------------------------------------------------------------------
// ENUMS
//------------------------------------------------------------------
enum class eRequestMState
{
    eFLOW,
    eFLOW_ANSWER,
    eRAW,
    eRAW_ANSWER,
    eUSER_REQUEST,
    eUSER_ANSWER,
};

enum class eRequestStatus
{
    eSTANDBY,
    eNEW_REQUEST,
    eWAITTING,
    eSUCCESS,
    eFAIL,
};

//------------------------------------------------------------------
// CLASSES
//------------------------------------------------------------------
class FlowTEX 
{
  //variables
  public:
  protected:
  private:
      TexNET TexNet;  
      float lastFlowRead; 
      bool newSample;
      volatile uint32_t timerRate;
      volatile uint32_t convRate;  
      eRequestMState RequestMState;
      eRequestStatus RequestStatus; 
      TexNET::cMessage NewRequest;
      bool bError = true;
      float flow;
      float temperature;
      float raw;
      float MaxFlow;
      float MinFlow;
      float avgFlow;
      float avgTemp;
      float nAvg;      
      char serialNumber[FLOWTEX_SERIALN_LEN + 1];
      char model[FLOWTEX_MODEL_LEN + 1];
      char ftVersion[FLOWTEX_VERSION_LEN + 1];

  public:
    FlowTEX();
    ~FlowTEX();
    void begin(Stream &stream);
    void begin();
    void poll();
    float getFlow();      
    float getTemperature();
    bool getStatus();
    void updateFlow();
    const char *getSerialNumber();
    const char *getVersion();
    const char *getModel();
  
  protected:  
  private:

};

#endif //__FLOWTEX_H__
