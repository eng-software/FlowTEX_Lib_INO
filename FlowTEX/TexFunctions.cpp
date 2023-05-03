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
#include <arduino.h>
#include "TexFunctions.h"

//------------------------------------------------------------------
// IMPLEMENTATIONS
//------------------------------------------------------------------
uint32_t getElapsedMillis(uint32_t value)
{
  uint32_t now = millis();

  return now - value;
}
