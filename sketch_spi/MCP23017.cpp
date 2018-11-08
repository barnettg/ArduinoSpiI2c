// cpp file for MCP23017
#include <Arduino.h>
#include "MCP23017.h"


MCP23017::MCP23017()
{
  // cannot have code before setup()
}

void MCP23017::start(unsigned char I2C_address,unsigned char portAdir, unsigned char portBdir)
{
  Wire.begin();
  _I2C_address = I2C_address;  
  _portaDir = portAdir;
  _portbDir = portBdir;
  initialize();
}

void MCP23017::writeReg(unsigned char reg, unsigned char data)
{
  Wire.beginTransmission(_I2C_address);
  Wire.write(reg);
  Wire.write(data);   
  Wire.endTransmission();
}
int MCP23017::readReg(unsigned char reg)
{
  unsigned char result=0;
  Wire.beginTransmission(_I2C_address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_I2C_address,(uint8_t)1);
  //delay(10); // wait 10 ms
  if(Wire.available()>0)
  {
    result = Wire.read(); //
    return (int) result;
  }
  
	return -1;
}  

void MCP23017::initialize(void)
{
   // set up port A 
   writeReg(IODIRA, _portaDir);
   
    // set up port B  
   writeReg(IODIRB, _portbDir);
   delay(100); // wait 100 ms
      
   // set port B
   writeReg(GPIOB, 0xFF);
   // set port A
   writeReg(GPIOA, 0xFF);
//   delay(100); // wait 100 ms
}

unsigned char MCP23017::readPortA(void)
{
  return readReg(GPIOA);
}
unsigned char MCP23017::readPortB(void)
{
	return readReg(GPIOB);
}

void MCP23017::writePortA(unsigned char value)
{
  writeReg(GPIOA, value);
}
void MCP23017::writePortB(unsigned char value)
{
  writeReg(GPIOB, value);
}

/// read bit from portA
///
/// @param bit number 0-7
/// @return bit value
bool MCP23017::readPortaBit(unsigned char bitNumber)
{
   int val = readPortA();
   int mask = 0x01 << (bitNumber & (0x7));
   if ((val & mask) != 0) return true;
   return false;
}

/// read bit from portB
///
/// @param bit number 0-7
/// @return bit value
bool MCP23017::readPortbBit(unsigned char bitNumber)
{
   int val = readPortB();
   int mask = 0x01 << (bitNumber & (0x7));
   if ((val & mask) != 0) return true;
   return false;
}

/// write bit on portA
///
/// @param bit number 0-7 and value true/false
/// @return true if no error
void MCP23017::writePortaBit(unsigned char bitNumber, bool logicVal)
{
   int val = readPortA();
   int mask = 0x01 << (bitNumber & (0x7));
   if(logicVal)
      val |= mask;
   else
      val &= ~mask;

   writePortA(val);
}

/// write bit on portB
///
/// @param bit number 0-7 and value true/false
/// @return true if no error
void MCP23017::writePortbBit(unsigned char bitNumber, bool logicVal)
{ 
   int val = readPortB();
   int mask = 0x01 << (bitNumber & (0x7));
   if(logicVal)
      val |= mask;
   else
      val &= ~mask;

   writePortB(val);
}


