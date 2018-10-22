// header file for AMC7812
#ifndef __MCP23017__H_
#define __MCP23017__H_
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
    
  private:
   unsigned char _I2C_address;
};

#endif

