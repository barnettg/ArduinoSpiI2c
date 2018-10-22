// cpp file for MCP23017
#include "MCP23017.h"


MCP23017::MCP23017(unsigned char I2C_address,unsigned char portAdir, unsigned char portBdir)
{
  _I2C_address = I2C_address;  
  initialize(portAdir, portBdir);
}

void MCP23017::writeReg(unsigned char reg, unsigned char data)
{
}
unsigned char MCP23017::readReg(unsigned char reg)
{
	return 0;
}  

void MCP23017::initialize(unsigned char portAdir, unsigned char portBdir)
{
}

unsigned char MCP23017::readPortA(void)
{
	return 0;
}
unsigned char MCP23017::readPortB(void)
{
	return 0;
}

void MCP23017::writePortA(unsigned char value)
{

}
void MCP23017::writePortB(unsigned char value)
{

}
	
	



