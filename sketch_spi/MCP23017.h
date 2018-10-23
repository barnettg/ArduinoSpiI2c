// header file for AMC7812
#ifndef __MCP23017__H_
#define __MCP23017__H_
#include <Wire.h>

class MCP23017
{
  public:
    MCP23017(unsigned char I2C_address,unsigned char portAdir, unsigned char portBdir);
    // I2C methods
    void writeReg(unsigned char reg, unsigned char data);
    unsigned char readReg(unsigned char reg);  

	void initialize(unsigned char portAdir, unsigned char portBdir);
	
	unsigned char readPortA(void);
	unsigned char readPortB(void);
	
	void writePortA(unsigned char value);
	void writePortB(unsigned char value);

  bool readPortaBit(int bitNumber);
  bool readPortbBit(int bitNumber);
  void writePortaBit(int bitNumber, bool logicVal);
  void writePortbBit(int bitNumber, bool logicVal);
  

    typedef enum  
   {
      IODIRA = 0,
      IODIRB,
      IPOLA,
      IPOLB,
      GPINTENA,
      GPINTENB,
      DEFVALA,
      DEFVALB,
      INTCONA,
      INTCONB,
      IOCONA,
      IOCONB,
      GPPUA ,
      GPPUB,
      INTFA,
      INTFB,
      INTCAPA,
      INTCAPB,
      GPIOA = 0x12,
      GPIOB = 0x13,
      OLATA,
      OLATB
   }Registers;

   typedef enum  
   {
      DOUT  = 0x00,
      DIN = 0xFF
   }Direction;

   
    
  private:
   unsigned char _I2C_address;
};

#endif

