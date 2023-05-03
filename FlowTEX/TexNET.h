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
#ifndef __TEXNET_H__
#define __TEXNET_H__

//------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------
#include <cstdint>
#include <cstring>
#include "Stream.h"
#include "TexFunctions.h"

//------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------
#define  MAX_TEXNET_SERIAL_BUFFER    50

//------------------------------------------------------------------
// CLASSES
//------------------------------------------------------------------
class TexNET
{  
  public:
    class cMessage
    {
      public:
          uint8_t Opcode;                         
          uint32_t Length;                        
          uint8_t Msg[MAX_TEXNET_SERIAL_BUFFER];  

      protected:
      private:
      public:
        uint8_t chks()
        {
          uint8_t chksValue  = (uint8_t)(Opcode + Length);
          uint8_t *ptMsg = Msg;              
          for(uint16_t i = (uint16_t)Length; i > 0  ; i--)
          {
            chksValue += *ptMsg++;
          }
          return chksValue;
        }

      protected:
      private:
    };

  private:
    const uint32_t MaxRetry = 3;                    
    const uint32_t Timeout = 200;                   
    uint32_t MessageTimer;                        
    uint32_t RequestTimer;                        
    Stream *ptStream;
    cMessage MessageBuffer;
    bool bNAKAllowed;

    //Message state machine
    enum class eMessageState
    {
      eSTX,                
      eOPCODE,             
      eLENGTH,             
      eMSG,                 
      eCHKS,                
      eNEW_MSG,             
      eNAK_RECEIVED,        
      eNAK_TO_SEND,         
    };

    //Resquest and response state machine
    enum class eRequestState
    {
      eSTANDBY,            
      eSENDING_REQUEST,    
      eREQUEST_SENT,       
      eWAITTING_ANSWER,    
      eNEW_ANSWER,        
      eNEW_REQUEST,        
      eSENDING_ANSWER,     
      eANSWER_SENT,       
      eSENDING_NAK,       
      eNAK_SENT,          
    };

    eMessageState MessageState;
    eRequestState RequestState;
    eRequestState RequestStateBeforeNAK;
    uint8_t MessageReceivedBytes = 0;
    uint32_t retry = 0;
    
  public:
    TexNET();
    void poll();
    void setResource(Stream &stream);
    bool sendRequest(uint8_t opcode, uint8_t *msg, uint32_t length);
    bool isWaitingAnswer();
    bool getAnswer(uint8_t **opcode, uint8_t **msg, uint32_t **length);
    void releaseAnswer();                           // Release answer
    bool getRequest(uint8_t **opcode, uint8_t **msg, uint32_t **length);
    bool sendAnswer(uint8_t opcode, uint8_t *msg, uint32_t length);

  protected:
  private:
    void messagePoll(int32_t data);
    uint32_t getDataToSend(uint8_t *bufferToSend);
    void dataSent();
    void process();

};

//------------------------------------------------------------------
// Compiler directives  - end
//------------------------------------------------------------------
#endif //__TexNET_H__
