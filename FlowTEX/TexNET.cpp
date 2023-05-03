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
#include <iostream>
#include "TexNET.h"


//------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------
#define STX                       0x02
#define NAK                       0x03

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------

TexNET::TexNET()
{    
    MessageState = eMessageState::eSTX;
    RequestState = eRequestState::eSTANDBY;
    MessageTimer = 0;
    RequestTimer = 0;
    bNAKAllowed = false;
    ptStream = &Serial;
}

void TexNET::poll()
{
    int32_t data = -1;
    do 
    {
        data = ptStream->read();
     
        if(data >= 0)
        {
            messagePoll(data);
        }    
    } 
    while (data >= 0);
  
    process();  
  
    uint8_t bufferTX[MAX_TEXNET_SERIAL_BUFFER];
    uint32_t bufferTXLen = getDataToSend(bufferTX);
      
    if(bufferTXLen> 0)
    {
        ptStream->write(bufferTX,bufferTXLen);
        dataSent();
    }    
}

void TexNET::setResource(Stream &stream)
{
    ptStream = &stream;  
}

void TexNET::messagePoll(int32_t data)
{  
    switch(MessageState)
    {
        default:
        case eMessageState::eSTX:
        {
            if(data >= 0)
            {
                if( (uint8_t)data == STX)
                {
                    bNAKAllowed = false;
                    MessageState = eMessageState::eOPCODE;
                    MessageTimer = millis();
                }
                else if( (uint8_t)data == NAK)
                {
                    if(bNAKAllowed == true)
                    {
                        MessageState = eMessageState::eNAK_RECEIVED;
                        MessageTimer = millis();
                    }
                }
            }
            break;
        }

        case eMessageState::eOPCODE:
        {
            if(data >= 0)
            {
                MessageBuffer.Opcode = (uint8_t)data;
                MessageTimer = millis();
                MessageState = eMessageState::eLENGTH;
            }
            else if ( getElapsedMillis(MessageTimer) > Timeout)
            {
                MessageState = eMessageState::eSTX;
            }
            break;
        }

        case eMessageState::eLENGTH:
        {
            if(data >= 0)
            {
                MessageBuffer.Length = (uint8_t)data;
                MessageTimer = millis();
                  
                if(MessageBuffer.Length > 0)
                {
                    MessageReceivedBytes = 0;
                    MessageState = eMessageState::eMSG;
                }
                else
                {
                    MessageState = eMessageState::eCHKS;
                }
            }
            else if (getElapsedMillis(MessageTimer) > Timeout)
            {
                MessageState = eMessageState::eSTX;
            }
            break;
        }
        
        case eMessageState::eMSG:
        {
          if(data >= 0)
          {
              MessageBuffer.Msg[MessageReceivedBytes] = (uint8_t)data;
              MessageReceivedBytes++;
              MessageTimer = millis();
      
              if(MessageReceivedBytes == MessageBuffer.Length)
              {
                  MessageState = eMessageState::eCHKS;
              }
              else if(MessageReceivedBytes > MessageBuffer.Length)
              {
                  MessageState = eMessageState::eSTX;
              }
          }
          else if (getElapsedMillis(MessageTimer) > Timeout)
          {
              MessageState = eMessageState::eSTX;
          }
    
          break;
        }
    
        case eMessageState::eCHKS:
        {
            if(data >= 0)
            {
                MessageTimer = millis();
    
                if((uint8_t)data == MessageBuffer.chks())
                {
                    MessageState = eMessageState::eNEW_MSG;
                }
                else
                {
                    MessageState = eMessageState::eNAK_TO_SEND;
                }
            }
            else if (getElapsedMillis(MessageTimer) > Timeout)
            {
                MessageState = eMessageState::eSTX;
            }
            break;
        }
    
        case eMessageState::eNEW_MSG:
        case eMessageState::eNAK_RECEIVED:
        case eMessageState::eNAK_TO_SEND:
        {
            break;
        }
    }
}

void TexNET::process()
{
    // Check machine state message
    if(MessageState == eMessageState::eNAK_TO_SEND)
    {
        if((RequestState == eRequestState::eSTANDBY)||
          (RequestState == eRequestState::eWAITTING_ANSWER))
        {
            RequestStateBeforeNAK = RequestState;
            RequestState = eRequestState::eSENDING_NAK;
            RequestTimer = millis();
        }
    }
        
    // Check
    switch(RequestState)
    {
        default: break;      
        case eRequestState::eSTANDBY:
        {
            if(MessageState == eMessageState::eNEW_MSG)
            {
                RequestState = eRequestState::eNEW_REQUEST;
                RequestTimer = millis();
            }
            else if(MessageState == eMessageState::eNAK_RECEIVED)
            {
                if(retry > 0)
                {
                    RequestState = eRequestState::eSENDING_ANSWER;
                    RequestTimer = millis();      
                    retry--;
                }
                else
                {
                    MessageState = eMessageState::eSTX;
                    MessageTimer  = millis();
                }
            }
            break;
        }
          
        case eRequestState::eNEW_REQUEST:
        {
            if(getElapsedMillis(RequestTimer) > Timeout)
            {
                RequestState = eRequestState::eSTANDBY;
                MessageState = eMessageState::eSTX;
                RequestTimer  = millis();
                MessageTimer  = millis();
            }  
            break;
        }
  
        case eRequestState::eSENDING_ANSWER:
        case eRequestState::eANSWER_SENT:
        {
            if( getElapsedMillis(RequestTimer) > Timeout)
            {
                RequestState = eRequestState::eSTANDBY;
                MessageState = eMessageState::eSTX;
                RequestTimer  = millis();
                MessageTimer  = millis();
            }
            break;
        }
          
        case eRequestState::eSENDING_NAK:
        case eRequestState::eNAK_SENT:
        {
            if(getElapsedMillis(RequestTimer) > Timeout)
            {
                RequestState  = RequestStateBeforeNAK;
                MessageState = eMessageState::eSTX;
                RequestTimer  = millis();
                MessageTimer  = millis();
            }
            break;
        }
  
        case eRequestState::eWAITTING_ANSWER:
        {
            if(MessageState == eMessageState::eNEW_MSG)
            {           
                RequestTimer = millis();
                RequestState = eRequestState::eNEW_ANSWER;
            }
            else
            {
                if( getElapsedMillis(RequestTimer) > Timeout)
                {
                    if(retry > 0)
                    {
                        RequestState = eRequestState::eSENDING_REQUEST;
                        MessageState = eMessageState::eSTX;
                        RequestTimer = millis();
                        MessageTimer = millis();
                        retry--;
                    }
                    else
                    {
                        RequestState = eRequestState::eSTANDBY;
                        MessageState = eMessageState::eSTX;
                        RequestTimer = millis();
                        MessageTimer = millis();
                    }
                }
            }
            break;
        }
  
        case eRequestState::eNEW_ANSWER:
        {
            if(getElapsedMillis(RequestTimer) > Timeout)
            {
                RequestState = eRequestState::eSTANDBY;
                MessageState = eMessageState::eSTX;
                RequestTimer = millis();
                MessageTimer = millis();
            }    
            break;
        }  
    }
}

uint32_t TexNET::getDataToSend(uint8_t *bufferToSend)
{
    if(RequestState == eRequestState::eSENDING_NAK)
    {       
        bufferToSend[0] = NAK;
        RequestState = eRequestState::eNAK_SENT;
        return 1;
    }
    else if(RequestState == eRequestState::eSENDING_ANSWER)
    {
        bufferToSend[0] = STX;
        bufferToSend[1] = MessageBuffer.Opcode;
        bufferToSend[2] = MessageBuffer.Length;
        memcpy(&bufferToSend[3], MessageBuffer.Msg, MessageBuffer.Length);
        bufferToSend[MessageBuffer.Length + 3] = MessageBuffer.chks();
        RequestTimer = millis();
        RequestState  = eRequestState::eANSWER_SENT;
        return MessageBuffer.Length + 4;
    }
    else if(RequestState == eRequestState::eSENDING_REQUEST)
    {
        bufferToSend[0] = STX;
        bufferToSend[1] = MessageBuffer.Opcode;
        bufferToSend[2] = MessageBuffer.Length;
        memcpy(&bufferToSend[3], MessageBuffer.Msg, MessageBuffer.Length);
        bufferToSend[MessageBuffer.Length + 3] = MessageBuffer.chks();
        RequestTimer = millis();
        RequestState  = eRequestState::eREQUEST_SENT;
        return MessageBuffer.Length + 4;
    }
    return 0;
}

void TexNET::dataSent()
{
    if(RequestState == eRequestState::eNAK_SENT)
    {
        bNAKAllowed = false;
        RequestState = RequestStateBeforeNAK;
        MessageState = eMessageState::eSTX;
        RequestTimer = millis();
        MessageTimer = millis();
    }
    else if(RequestState == eRequestState::eANSWER_SENT)
    {
        bNAKAllowed = true;
        RequestState = eRequestState::eSTANDBY;
        MessageState = eMessageState::eSTX;
        RequestTimer = millis();
        MessageTimer = millis();
    }
    else if(RequestState == eRequestState::eREQUEST_SENT)
    {
        bNAKAllowed = true;
        RequestState = eRequestState::eWAITTING_ANSWER;
        MessageState = eMessageState::eSTX;
        RequestTimer = millis();
        MessageTimer = millis();
    }
}

bool TexNET::sendRequest(uint8_t opcode, uint8_t *msg, uint32_t length)
{
    if(RequestState == eRequestState::eSTANDBY)
    {
        bNAKAllowed = false;
        MessageBuffer.Opcode = opcode;
        MessageBuffer.Length = (uint8_t)length;
        memcpy( MessageBuffer.Msg, msg, length);
        retry = MaxRetry;
        RequestState = eRequestState::eSENDING_REQUEST;
        RequestTimer = millis();
        return true;
    }
    return true;
}

bool TexNET::isWaitingAnswer()
{
    if((RequestState == eRequestState::eSENDING_REQUEST) ||
       (RequestState == eRequestState::eREQUEST_SENT) ||
       (RequestState == eRequestState::eWAITTING_ANSWER))
    {      
        return true;
    }
    return false;
}

bool TexNET::getAnswer(uint8_t **opcode, uint8_t **msg, uint32_t **length)
{
  if(RequestState == eRequestState::eNEW_ANSWER)
  {
      *opcode = &MessageBuffer.Opcode;
      *length = &MessageBuffer.Length;
      *msg =  MessageBuffer.Msg;
      RequestTimer = millis();
      MessageTimer = millis();
      return true;
  }
  return false;
}

void TexNET::releaseAnswer()
{
  if(RequestState == eRequestState::eNEW_ANSWER)
  {
      bNAKAllowed = false;
      RequestState = eRequestState::eSTANDBY;
      MessageState = eMessageState::eSTX;
      RequestTimer = millis();
      MessageTimer = millis();
  }
}

bool TexNET::getRequest(uint8_t **opcode, uint8_t **msg, uint32_t **length)
{
  if(RequestState == eRequestState::eNEW_REQUEST)
  {
      *opcode = &MessageBuffer.Opcode;
      *length = &MessageBuffer.Length;
      *msg = MessageBuffer.Msg;
      RequestTimer = millis();
      MessageTimer = millis();
      return true;
  }

  return false;
}

bool TexNET::sendAnswer(uint8_t opcode, uint8_t *msg, uint32_t length)
{
  if(RequestState == eRequestState::eNEW_REQUEST)
  {
      bNAKAllowed = false;
      MessageBuffer.Opcode = opcode;
      MessageBuffer.Length = (uint8_t)length;
      memcpy( MessageBuffer.Msg, msg, length);
      retry = MaxRetry;
      RequestState = eRequestState::eSENDING_ANSWER;
      RequestTimer = millis();
      return true;
  }

  return true;
}
