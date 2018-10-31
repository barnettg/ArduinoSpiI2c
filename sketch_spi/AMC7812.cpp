// cpp file for AMC7812
#include "AMC7812.h"

AMC7812::AMC7812()
{
 // cannot have code before setup()
}

void AMC7812::start(unsigned char I2C_address)
{
  _I2C_address = I2C_address;  
  initialize();
}

void AMC7812::writeReg16(int reg, unsigned int data)
{
  
}
unsigned int AMC7812::readReg16(int reg)
{
  return 0;
}

// specific peripherals
void AMC7812::writeDac(unsigned char dacNum, unsigned int value)
{
  
}
unsigned int AMC7812::readAdc(unsigned char adcNum)
{
  return 0;  
}
void AMC7812::setConfig0(unsigned int value)
{
  
}
void AMC7812::setConfig1(unsigned int value)
{
  
}
unsigned int AMC7812::getConfig0(void)
{
  return 0;  
}
unsigned int AMC7812::getConfig1(void)
{
  return 0;  
}

void AMC7812::setGPIO(unsigned char value)
{
  
}
unsigned char AMC7812::getGPIO(void)
{
  return 0;  
}

// 
void AMC7812::initialize(void)  // initialize the device 
{
  
}
// write given ascii chars
// address 0x(RH)(RL)  -- where RH and RL are an ascii char for the register
// data 0x(DH)(DH2)(DL)(DL2)  -- where DH, DH2, DL, DL2 are an ascii char for 16 bit number
void AMC7812::writeReg16s(unsigned char RH, unsigned char RL,
        unsigned char DH, unsigned char DH2, unsigned char DL, unsigned char DL2)
{
          
}
// read given ascii chars
// address 0x(AH)(AL)  -- where RH and RL are an ascii char for the register
unsigned int AMC7812::readReg16s(unsigned char RH, unsigned char RL)
{
  return 0;    
}



