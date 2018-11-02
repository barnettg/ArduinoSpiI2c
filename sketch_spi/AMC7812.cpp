// cpp file for AMC7812
#include <Arduino.h>
#include "AMC7812.h"

AMC7812::AMC7812()
{
 // cannot have code before setup()
}

void AMC7812::start(unsigned char I2C_address)
{
  Wire.begin();
  _I2C_address = I2C_address;  
  initialize();
}

void AMC7812::writeReg16(int reg, unsigned int data) // 
{
  unsigned char tempdataH = (unsigned char) (data >>8) & 0xFF;
  unsigned char tempdataL = (unsigned char) data  & 0xFF;
  Wire.beginTransmission(_I2C_address);
  Wire.write(reg);
  Wire.write(tempdataH);  
  Wire.write(tempdataL);  
  Wire.endTransmission();
}
unsigned int AMC7812::readReg16(int reg) // 
{
  unsigned char result = 0;
  unsigned int resultTotal = 0;
  Wire.beginTransmission(_I2C_address);
  Wire.write(reg);
  Wire.endTransmission(false); // need to investigate more on false parameter?? --------------------------------------------------------------------
  Wire.requestFrom((uint8_t)_I2C_address,(uint8_t)2);
  //delay(10); // wait 10 ms
  //if(Wire.available()>0)
  //{
    result = Wire.read(); //
    resultTotal =  (unsigned int) result;
  //}
  //delay(10); // wait 10 ms
  //if(Wire.available()>0)
  //{
    result = Wire.read(); //
    resultTotal<<=8;
    resultTotal +=  (unsigned int) result;
  //}
  return resultTotal;
}

// 
void AMC7812::initialize(void)  // initialize the device 
{
   // turn on the dacs and reference
   writeReg16(0x6B, 0x7FFE);
   writeReg16(0xA, 0x00FF);
   writeReg16(0x4D, 0x0270);
   // enabling ch0 to 12 as single ended
   writeReg16(0x50, 0x6DFF);
   // enabling ch13 to 15 as single ended
   writeReg16(0x51, 0x7000);
   writeReg16(0x53, 0x0000);
   // setting the dacs as asynch mode
   writeReg16(0x58, 0x0000);
   //GPIO
   writeReg16(0x4B, 0x0000);
   // setting auto conversion, internal ref, async DAC writes, disable the alarm
   writeReg16(0x4C, 0x3C00);
}

// specific peripherals
void AMC7812::writeDac(unsigned char dacNum, unsigned int value)
{
  // DAC0  0x33
  // DAC1  0x34
  // DAC2  0x35
  // DAC3  0x36
  // DAC4  0x37
  // DAC5  0x38
  // DAC6  0x39
  // DAC7  0x3A
  // DAC8  0x3B
  // DAC9  0x3C
  // DAC10  0x3D
  // DAC11  0x3E
  if (dacNum <12) writeReg16(0x33 + dacNum, value);
  
}
unsigned int AMC7812::readAdc(unsigned char adcNum)
{
  // ADC0  0x23
  // ADC1  0x24
  // ADC2  0x25
  // ADC3  0x26
  // ADC4  0x27
  // ADC5  0x28
  // ADC6  0x29
  // ADC7  0x2A
  // ADC8  0x2B
  // ADC9  0x2C
  // ADC10  0x2D
  // ADC11  0x2E
  // ADC12  0x2F
  // ADC13  0x30
  // ADC14  0x31
  // ADC15  0x32
  //writeReg16(0x4C, 0x3C00); // need this??? only need in initialize7812??
  
  if(adcNum <=15) return readReg16(0x23 + adcNum);
  else return 0;  
}
void AMC7812::setConfig0(unsigned int value)
{
  // config0 0x4C
  writeReg16(0x4C, value);
}
void AMC7812::setConfig1(unsigned int value)
{
  // config0 0x4D
  writeReg16(0x4D, value);  
}
unsigned int AMC7812::getConfig0(void)
{
  // config0 0x4C
  return readReg16(0x4C);  
}
unsigned int AMC7812::getConfig1(void)
{
  // config0 0x4D
  return readReg16(0x4D);  
}

void AMC7812::setGPIO(unsigned char value)
{
  //GPIO 0x4B
  writeReg16(0x4B, value);
}
unsigned char AMC7812::getGPIO(void)
{
  //GPIO 0x4B
  return readReg16(0x4B);   
}


//// write given ascii chars
//// address 0x(RH)(RL)  -- where RH and RL are an ascii char for the register
//// data 0x(DH)(DH2)(DL)(DL2)  -- where DH, DH2, DL, DL2 are an ascii char for 16 bit number
//void AMC7812::writeReg16s(unsigned char RH, unsigned char RL,
//        unsigned char DH, unsigned char DH2, unsigned char DL, unsigned char DL2)
//{
//          
//}
//// read given ascii chars
//// address 0x(AH)(AL)  -- where RH and RL are an ascii char for the register
//unsigned int AMC7812::readReg16s(unsigned char RH, unsigned char RL)
//{
//  return 0;    
//}



