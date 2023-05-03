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
#include <Arduino.h>
#include "FlowTEX.h"
#include "TexFunctions.h"

//------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------
#define FLOWTEX_FLOW_OPCODE     ((uint8_t)'F')
#define FLOWTEX_VERSION_OPCODE  ((uint8_t)'v')
#define FLOWTEX_MODEL_OPCODE    ((uint8_t)'m')
#define FLOWTEX_SERIALN_OPCODE  ((uint8_t)'n')

//------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------
FlowTEX::FlowTEX()
{
    RequestMState = eRequestMState::eFLOW;
    RequestStatus = eRequestStatus::eSTANDBY;
    newSample = false;
    avgFlow = 0;
    nAvg = 0;
}

FlowTEX::~FlowTEX()
{
} 

void FlowTEX::begin(Stream &stream)
{
    TexNet.setResource(stream);
    newSample = false;
}

void FlowTEX::begin()
{ 
    begin(Serial);
}

bool FlowTEX::getStatus()
{
    return !bError;
}

void FlowTEX::poll()
{ 
   if((bError) && (RequestStatus == eRequestStatus::eNEW_REQUEST))
   {
        RequestStatus = eRequestStatus::eFAIL;
   }

   switch( RequestMState )
   {
      default:
      case eRequestMState::eFLOW:
      {       
          TexNet.sendRequest(FLOWTEX_FLOW_OPCODE, nullptr, 0 );
          RequestMState = eRequestMState::eFLOW_ANSWER;       
          break;
      }

      case eRequestMState::eFLOW_ANSWER:
      {
          if(!TexNet.isWaitingAnswer())
          {
              uint8_t *opcode;
              uint8_t *msg;
              uint32_t *length;
         
              if(TexNet.getAnswer(&opcode, &msg, &length))
              {
                  if(( *opcode == FLOWTEX_FLOW_OPCODE)&&( *length>=8))
                  {
                      float value;
                      memcpy( &value, &msg[0], sizeof(value));
                      float actualFlow = value;
             
                      memcpy( &value, &msg[4], sizeof(value));
                      float actualTemperature = value;

                      if(actualFlow > MaxFlow)
                      {
                          MaxFlow = actualFlow;
                      }

                      if(actualFlow < MinFlow)
                      {
                          MinFlow = actualFlow;
                      }

                      avgFlow = avgFlow*(nAvg/(nAvg+1));
                      avgTemp = avgTemp*(nAvg/(nAvg+1));
                      nAvg++;
                      avgFlow += actualFlow/nAvg;
                      avgTemp += actualTemperature/nAvg;
                      bError = false;
             
                      if(newSample == false)
                      {
                          flow =  avgFlow;
                          temperature = avgTemp;
                          avgFlow = 0;
                          avgTemp = 0;
                          nAvg = 0;
                          newSample = true;
                      }
                }
           
                TexNet.releaseAnswer();

                  if(RequestStatus == eRequestStatus::eNEW_REQUEST)
                  {
                      RequestMState = eRequestMState::eUSER_REQUEST;
                  }
                  else
                  {
                      TexNet.sendRequest(FLOWTEX_FLOW_OPCODE, nullptr, 0 );
                      RequestMState = eRequestMState::eFLOW_ANSWER;
                  }           
            }
            else
            {
                bError = true;
                RequestMState = eRequestMState::eFLOW;           
            }
          }
          break;
     }

     case eRequestMState::eUSER_REQUEST:
     {
          TexNet.sendRequest(NewRequest.Opcode, NewRequest.Msg, NewRequest.Length);
          RequestStatus = eRequestStatus::eWAITTING;
          RequestMState = eRequestMState::eUSER_ANSWER;
          break;
     }

     case eRequestMState::eUSER_ANSWER:
     {
          if(!TexNet.isWaitingAnswer())
          {
              uint8_t *opcode;
              uint8_t *msg;
              uint32_t *length;
         
              if(TexNet.getAnswer(&opcode, &msg, &length))
              {
                  if(*opcode == NewRequest.Opcode)
                  {
                      NewRequest.Opcode = *opcode;
                      memcpy(NewRequest.Msg, msg, *length);
                      NewRequest.Length = *length;
                      RequestStatus = eRequestStatus::eSUCCESS;
                      bError = false;
                  }
                  else
                  {
                      RequestStatus = eRequestStatus::eFAIL;
                      bError = true;
                  }           
                  TexNet.releaseAnswer();
              }
              else
              {
                  RequestStatus = eRequestStatus::eFAIL;
                  bError = true;
              }         
              RequestMState = eRequestMState::eFLOW;
          }
          break;
      }
   }

   TexNet.poll();

   updateFlow();
}


float_t FlowTEX::getFlow(void)
{
  return lastFlowRead;
}

float FlowTEX::getTemperature()
{
    return temperature;
}

void FlowTEX::updateFlow()
{ 
  if( newSample )
  {   
      lastFlowRead = flow;
      newSample = false;
      convRate = 1000/getElapsedMillis(timerRate);
      timerRate = millis();       
  }
}

const char *FlowTEX::getSerialNumber()
{
    while((RequestStatus == eRequestStatus::eNEW_REQUEST) || (RequestStatus == eRequestStatus::eWAITTING))
    {
        poll();
    }
    
    NewRequest.Opcode = FLOWTEX_SERIALN_OPCODE;
    NewRequest.Length = 0;
    RequestStatus = eRequestStatus::eNEW_REQUEST;

    while((RequestStatus == eRequestStatus::eNEW_REQUEST) || (RequestStatus == eRequestStatus::eWAITTING))
    {
        poll();
    }

    if(RequestStatus != eRequestStatus::eSUCCESS)
    {
        memset(serialNumber, 0, sizeof(serialNumber));
        return serialNumber;
    }

    memcpy(serialNumber, NewRequest.Msg, sizeof(serialNumber));
    serialNumber[FLOWTEX_SERIALN_LEN] = 0;
    TexNet.releaseAnswer();

    return serialNumber;
}

const char *FlowTEX::getVersion()
{
    while((RequestStatus == eRequestStatus::eNEW_REQUEST) || (RequestStatus == eRequestStatus::eWAITTING))
    {
        poll();
    }
    
    NewRequest.Opcode = FLOWTEX_VERSION_OPCODE;
    NewRequest.Length = 0;
    RequestStatus = eRequestStatus::eNEW_REQUEST;

    while((RequestStatus == eRequestStatus::eNEW_REQUEST) || (RequestStatus == eRequestStatus::eWAITTING))
    {
        poll();
    }

    if(RequestStatus != eRequestStatus::eSUCCESS)
    {
        memset(ftVersion, 0, sizeof(ftVersion));
        return ftVersion;
    }

    memcpy(ftVersion, NewRequest.Msg, sizeof(ftVersion));
    ftVersion[FLOWTEX_VERSION_LEN] = 0;
    TexNet.releaseAnswer();

    return ftVersion;
}

const char *FlowTEX::getModel()
{
    while((RequestStatus == eRequestStatus::eNEW_REQUEST) || (RequestStatus == eRequestStatus::eWAITTING))
    {
        poll();
    }
    
    NewRequest.Opcode = FLOWTEX_MODEL_OPCODE;
    NewRequest.Length = 0;
    RequestStatus = eRequestStatus::eNEW_REQUEST;

    while((RequestStatus == eRequestStatus::eNEW_REQUEST) || (RequestStatus == eRequestStatus::eWAITTING))
    {
        poll();
    }

    if(RequestStatus != eRequestStatus::eSUCCESS)
    {
        memset(model, 0, sizeof(model));
        return model;
    }

    memcpy(model, NewRequest.Msg, sizeof(model));
    model[FLOWTEX_MODEL_LEN] = 0;
    TexNet.releaseAnswer();

    return model;
}
