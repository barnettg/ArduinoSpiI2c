// header file for AMC7812
#ifndef __MCP23017__H_
#define __MCP23017__H_
#include <Wire.h>

class MCP23017
{
  public:
    MCP23017();
    void start(unsigned char I2C_address,unsigned char portAdir, unsigned char portBdir);
    // I2C methods
    void writeReg(unsigned char reg, unsigned char data);
    int readReg(unsigned char reg);  

	void initialize(void);
	
	unsigned char readPortA(void);
	unsigned char readPortB(void);
	
	void writePortA(unsigned char value);
	void writePortB(unsigned char value);

  bool readPortaBit(unsigned char bitNumber);
  bool readPortbBit(unsigned char bitNumber);
  void writePortaBit(unsigned char bitNumber, bool logicVal);
  void writePortbBit(unsigned char bitNumber, bool logicVal);
  

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

   typedef enum  
   {
      Base0 = 0x20,
      Base1,
      Base2,
      Base3,
      Base4,
      Base5,
      Base6
   }Address;
   
    
  private:
   unsigned char _I2C_address;
   unsigned char _portaDir;
   unsigned char _portbDir;
};

#endif

