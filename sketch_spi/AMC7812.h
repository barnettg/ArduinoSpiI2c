// header file for AMC7812
#ifndef __AMC7812__H_
#define __AMC7812__H_

#include <Wire.h>

class AMC7812
{
  public:
    AMC7812();
    void start(unsigned char I2C_address);
    // I2C methods
    void writeReg16(int reg, unsigned int data);
    unsigned int readReg16(int reg);


    // specific peripherals
    void writeDac(unsigned char dacNum, unsigned int value);
    unsigned int readAdc(unsigned char adcNum);
    void setConfig0(unsigned int value);
    void setConfig1(unsigned int value);
    unsigned int getConfig0(void);
    unsigned int getConfig1(void);

    void setGPIO(unsigned char value);
    unsigned char getGPIO(void);

    // 
    void initialize(void);  // initialize the device 
    // write given ascii chars
    // address 0x(RH)(RL)  -- where RH and RL are an ascii char for the register
    // data 0x(DH)(DH2)(DL)(DL2)  -- where DH, DH2, DL, DL2 are an ascii char for 16 bit number
    void writeReg16s(unsigned char RH, unsigned char RL,
            unsigned char DH, unsigned char DH2, unsigned char DL, unsigned char DL2);
    // read given ascii chars
    // address 0x(AH)(AL)  -- where RH and RL are an ascii char for the register
    unsigned int readReg16s(unsigned char RH, unsigned char RL);            
    
  private:
   unsigned char _I2C_address;
};

#endif

