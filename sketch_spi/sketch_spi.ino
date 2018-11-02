// Glen Barnett
// May 16, 2018

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include "MCP23017.h"
#include "AMC7812.h"

Adafruit_MCP4725 dac;

// Notes
// 


/*
 * Serial Commands 
 * SPI
 * S AA DD DD<cr>  AA address 00-7F write, 80-FF read  DD DD 16-bit data MSB first
 *            returns s AA DD DD YY YY<cr>  YY YY 16 bit returned data
 *  CSB: pin 7
    MOSI: pin 11
    MISO: pin 12
    SCK: pin 13           
 *            
 *            
 *  I2C          
 *  
 *  s - data byte size 0,1,2,3,4..
 *  AA - hex address
 *  RR - hex register
 *  DD - hex data
 *  p -raw read
 *  r - read register
 *  w - write
 *  pAAS<cr>   - raw read address of S bytes
 *  rAARRS<cr> - read address AA register RR number of bytes S
 *  wAASDDDDDD<cr> - write address AA, number of bytes S, data DD up to 3 bytes
 *  
 *   
 */

//MCP4725DAC
// send DWxxx<cr>   set DAC to hex value xxx
//      rec back dwxxx
//
//AMC7812
//   send AWRRDDDD<cr>    write register 8-bit hex RR with 16-bit hex  DDDD
//       rec back awrrdddd
//   send ARrr<cr>    read 16-bit register rr (8-bit hex)
//      rec back arRRDDDD 16-bit hex  DDDD
//
//   send ADNDDD<cr>  write DAC N(4-bit hex) with 12-bit hex  DDD
//    rec back adnddd
//
//   send AAN<cr>  read ADC N(4-bit hex) 
//    rec back aanDDD     12-bit hex  DDD
//
//   send AGWDD<cr>  write GPIO DD(8-bit hex) 
//    rec back agwdd   
//
//   send AGR<cr>  read GPIO 
//    rec back agrdd   DD(8-bit hex) 
//
//   send AI<cr>  initialize the AMC7812 with the test fixtur default configuraton
//    rec back ai  
//
//
//MPC23017
//   send GWNRRDD<cr>  write device N(0-5) reg RR(8-bit hex)  with data  DD(8-bit hex) 
//    rec back gwnrrdd   
//
//   send GRNRR<cr>  read device N(0-5) reg RR(8-bit hex)  
//    rec back grnrrdd   with data  DD(8-bit hex) 
//
//   send GIN<cr>  initialize device N(0-5) with the test fixture default configuraton
//    rec back gin
//
//   send GARN<cr>  read device N(0-5) port A
//    rec back garnDD   with data  DD(8-bit hex) 
//
//   send GBRN<cr>  read device N(0-5) port B
//    rec back gbrnDD   with data  DD(8-bit hex) 
//
//   send GAWNDD<cr>  write device N(0-5) port A with data  DD(8-bit hex) 
//    rec back gawnDD   
//
//   send GBWNDD<cr>  read device N(0-5) port B with data  DD(8-bit hex) 
//    rec back gbwnDD  

 // function prototypes
void sendOutData(String dtaStr);
int getRecBufferSize(void);
char popSerialData(void);
void pushSerialData(char dta);
void saveRecString(void);
void decodeSerial();
void printoutRecCommandPacket();
char getNextChar(void);
void drainTheBuffer(void);
char commandBufferEmpty(void);
char commandBufferFull(void);
//void addToStash(char tmp);
//void clearStash(void);
//char commandBreakdown(void);
bool charIsNum(char tmpChar);
void commandBad(void);
void sendOutPrint(String dtaStr);
void sendOutPrintln(String dtaStr);
String convertToHexChar(int data8);
String convertToSingleHexChar(uint8_t data8);

String conver4bitToString(unsigned char val);
String conver8bitToString(unsigned char val);
String conver12bitToString(unsigned int val);
String conver16bitToString(unsigned int val);

String _ver = "1.0";
// serial buffer
char serialArray[256];
unsigned char serialTop = 0;
unsigned char serialBottom = 0; 
// to communicat with laser serial library
char END_CHAR = '\r'; // \n  
String _recString="";

char _address=0;
String _function = ""; 
String _dataString = ""; 
bool _cmdDataFound = false;
bool _cmdDataError = false;
const char _ID = 1; // serial protocol ID number

const int chipSelectPin = 7;


//MCP23017 *m_MCP23017_20;//(CMCP23017::Base0);
//MCP23017 *m_MCP23017_21;//(CMCP23017::Base1);
//MCP23017 *m_MCP23017_22;//(CMCP23017::Base2);
//MCP23017 *m_MCP23017_23; //(CMCP23017::Base3);
//MCP23017 *m_MCP23017_24; //(CMCP23017::Base4);
//
//AMC7812 *m_AMC7812_64;
// these:
MCP23017 m_MCP23017_20;
MCP23017 m_MCP23017_21;
MCP23017 m_MCP23017_22;
MCP23017 m_MCP23017_23;
MCP23017 m_MCP23017_24;
AMC7812 m_AMC7812_64;
  
char inChar;
void setup() {
  Serial.begin(57600);
  while (!Serial);
  Serial.println("Starting... ");

  m_MCP23017_20.start(MCP23017::Base0, MCP23017::DIN,  MCP23017::DIN);
  m_MCP23017_21.start(MCP23017::Base1, MCP23017::DOUT, MCP23017::DOUT);
  m_MCP23017_22.start(MCP23017::Base2, MCP23017::DIN,  MCP23017::DIN);
  m_MCP23017_23.start(MCP23017::Base3, MCP23017::DOUT, MCP23017::DIN);
  m_MCP23017_24.start(MCP23017::Base4, MCP23017::DOUT, MCP23017::DIN);
  m_AMC7812_64.start(0x64);
  
  // start the SPI library:
//  SPI.begin();
//  pinMode(chipSelectPin, OUTPUT);
//  pinMode(SS, OUTPUT);
//  digitalWrite(chipSelectPin, HIGH);
  //SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0)); // 8MHz clock

  //default too
//  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); // 4MHz clock 
  // data out changes on clack fall, there is a clock rise in centr of data
  // to transfer 3 bytes is 12.5us- ~ 1us between bytes- 3us from last data clock to cs rise

  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
//  dac.begin(0x62);
  //dac.setVoltage(1024, false); // should set to 1/4 voltage
}

void loop() {

  if(Serial.available())
  {
    inChar = (char)Serial.read();
    if( (inChar >= 32 && inChar <= 126) || inChar == '\r' || inChar == '\n' )
    {
      pushSerialData(inChar);
      if (inChar == END_CHAR)
      {
        decodeSerial();
        drainTheBuffer();
        
      }
      //Serial.println(inChar);
    }
    else
    {
      // purge buffer
      serialBottom = serialTop;
    }
  }

  // check data buffer size
  // purge if too large
  if(getRecBufferSize() >25)
  {
    // purge buffer
      serialBottom = serialTop;
  }

}// end loop


//------------------------
// get char from the serial buffer
//------------------------
char popSerialData(void)
{
  char tmp = 0xFF;
  //Serial.println("in popSerialData");
  if(serialBottom != serialTop)
  {
    tmp = serialArray[serialBottom];
    //Serial.print("popped data: ");
    //Serial.println(tmp);
    serialBottom++; // of if using 256 array size
  }
  return tmp;

}
//------------------------
// save serial char in a buffer
//------------------------
void pushSerialData(char dta)
{
  serialArray[serialTop] = dta;
  serialTop++;
  if(serialBottom == serialTop)
  {
    serialTop--; // overran buffer so back off by one
    //ok if buffer size is 256 otherwise have to watch math
  }
}

//---------------------------------------
//
//-----------------------------------------
//void printoutRecCommandPacket()
//{
//    Serial.print("comand: ");
//    Serial.print(_function);
//    if (_cmdDataFound) 
//    {
//      Serial.print("  Data: ");
//      Serial.print(_dataString);
//    }
//    Serial.println("");
//}

//------------------------
// skip white spaces or junk
//------------------------
char getNextChar(void)
{
  // could also limit number of pops to 5
  // to make sure not to crash
  char tempchar;
  char maxcount = 0;
  //Serial.println("in getNextChar");

  tempchar = 0;
  while(tempchar < 33 || tempchar> 125)
  {
    maxcount++;
    tempchar = popSerialData();
    
    //Serial.println("-"+(String)tempchar+"-");
    if(tempchar == END_CHAR) return 0; // end char could be non printable ie <cr> so 
                                        // return from function if found it
    if(maxcount >5 ) return 0; // that's too much- have to have some constraints on format
    //if(commandBufferEmpty() == 1) return 0; // if screw up will break out if buffer empty
  }
  //Serial.println("-return "+(String)tempchar+"-");
  return tempchar;
}
//------------------------
// drain the last command
//------------------------
void drainTheBuffer(void)
{
  char tmp;
  int count; 
  tmp = getNextChar();
  //addToStash(tmp);
  while(tmp!=END_CHAR)
  {
    tmp=getNextChar();
    if(commandBufferEmpty() == 1) return; // if screw up will break out if buffer empty
  }
}
//------------------------
// serial command buffer empty 
// returns 1 for empty, 0 otherwise
//------------------------
char commandBufferEmpty(void)
{
  if(serialBottom == serialTop) return 1;
  return 0;
}//------------------------
// serial command buffer Full 
// returns 1 for full, 0 otherwise
//------------------------
char commandBufferFull(void)
{
  // only works if buffer size is 256
  if(serialTop == (serialBottom -1)) return 1;
  return 0;
}

//--------------------------------------------
//
//--------------------------------------------
bool charIsNum(char tmpChar)
{
  if(tmpChar>= '0' && tmpChar <= '9') return true;
  return false;
}
//------------------------------
//
//----------------------------------
int convertCharToDecimal(char val)
{
  if (charIsNum(val))
  {
    return (int)(val-'0');
  }
  else if(val>= 'a' && val <= 'f')
  {
    return (int)(val-'a' +10);
  }
  else if(val>= 'A' && val <= 'F')
  {
    return (int)(val-'A' +10);
  }  
  return -1;
}
//------------------------------------
//
//-------------------------------------
int get8bitval(char MSD, char LSD)
{
  int ret_val;
  ret_val = convertCharToDecimal(MSD);
  //Serial.println("#"+(String)ret_val+"#");
  ret_val <<=4;
  ret_val += convertCharToDecimal(LSD);
  return ret_val;
}
// --------------- commands ---------------------------------
void commandBad(void)
{
  sendOutPrintln("X");
}

// --------------- end commands ---------------------------------

String convertToHexChar(int data8)
{
  char MSD = (char)((data8>>4)&0xF);
  char LSD = (char)((data8)&0xF);

  if(MSD <=9) MSD += '0';
  else MSD += 'A'-10;

  if(LSD <=9) LSD += '0';
  else LSD += 'A'-10;

  return (String)MSD+(String)LSD;
}
String convertToSingleHexChar(uint8_t data8)
{
  char LSD = (char)((data8)&0xF);

  if(LSD <=9) LSD += '0';
  else LSD += 'A'-10;

  return (String)LSD;
}
String conver4bitToString(unsigned char val)
{
  String resp = "";
  uint8_t convRegN2 = (uint8_t) ( (val) & 0xF);
  resp += convertToSingleHexChar(convRegN2);
  return resp;  
}

String conver8bitToString(unsigned char val)
{
  String resp = "";
  
  uint8_t convRegN1 = (uint8_t) ( (val >> 4) & 0xF);
  uint8_t convRegN2 = (uint8_t) ( (val) & 0xF);
  resp += convertToSingleHexChar(convRegN1);
  resp += convertToSingleHexChar(convRegN2);  
  return resp;  
}
String conver12bitToString(unsigned int val)
{
  String resp = "";
  uint8_t convDataN2 = (uint8_t) ( (val >> 8) & 0xF);
  uint8_t convDataN3 = (uint8_t) ( (val >> 4) & 0xF);
  uint8_t convDataN4 = (uint8_t) ( (val) & 0xF); 
  resp += convertToSingleHexChar(convDataN2);
  resp += convertToSingleHexChar(convDataN3);
  resp += convertToSingleHexChar(convDataN4);
  return resp;     
}
String conver16bitToString(unsigned int val)
{
  String resp = "";
  uint8_t convDataN1 = (uint8_t) ( (val >> 12) & 0xF);
  uint8_t convDataN2 = (uint8_t) ( (val >> 8) & 0xF);
  uint8_t convDataN3 = (uint8_t) ( (val >> 4) & 0xF);
  uint8_t convDataN4 = (uint8_t) ( (val) & 0xF); 
  resp += convertToSingleHexChar(convDataN1);
  resp += convertToSingleHexChar(convDataN2);
  resp += convertToSingleHexChar(convDataN3);
  resp += convertToSingleHexChar(convDataN4);
  return resp;     
}      

//------------------------
// decode the serial commands after \n received
//------------------------
void decodeSerial()
{
  char tmp;
  //Serial.print("decode  "); // --
  int txrxbuff[20];
  char buffSize = 0; 
/*
 * Serial Commands 
 *  
 */
  tmp = getNextChar();
  if(tmp == 'D')
  {
    // format DWxxx
    // return dwxxx
    //Serial.println("\n found DAC comand ");
    char tmp2;
    tmp2 = getNextChar();
    if (tmp2 == 'W')
    {
      int setVal = 0;
      char tmp3;
      char tmp4;
  
      tmp3 = getNextChar();
      tmp4 = getNextChar();
      int temp16 = get8bitval(tmp3, tmp4); // upper 8 bits

      tmp3 = getNextChar();
      temp16 <<=4;
      temp16 += get8bitval('0', tmp3); // lower 4 bits -- combine for 12 bits
      //Serial.print("temp16: ");
      //Serial.println(temp16, HEX);
      
      dac.setVoltage(temp16, false); // 

      uint8_t convTop4 = (uint8_t) ( (temp16 >> 8) & 0xF);
      uint8_t convMid4 = (uint8_t) ( (temp16 >> 4) & 0xF);
      uint8_t convLower4 = (uint8_t) ( (temp16) & 0xF);
      
      // return response
      String resp = "dw";
      //Serial.print(resp);
      resp += convertToSingleHexChar(convTop4);
      //Serial.print("  " + resp);
      resp += convertToSingleHexChar(convMid4);
      //Serial.print("  " + resp);
      resp += convertToSingleHexChar(convLower4);
      //Serial.print("  " + resp);

      Serial.println(resp);
    }
    else
    {
      // return response
      String resp = "d";
      resp += tmp ;
      resp += "?" ;
      Serial.println(resp);
    }
    
  }

  else if(tmp == 'A')
  {
    //AMC7812
    //   send AWRRDDDD<cr>    write register 8-bit hex RR with 16-bit hex  DDDD
    //       rec back awrrdddd
    //   send ARrr<cr>    read 16-bit register rr (8-bit hex)
    //      rec back arRRDDDD 16-bit hex  DDDD
    //
    //   send ADNDDD<cr>  write DAC N(4-bit hex) with 12-bit hex  DDD
    //    rec back adnddd
    //
    //   send AAN<cr>  read ADC N(4-bit hex) 
    //    rec back aanDDD     12-bit hex  DDD
    //
    //   send AGWDD<cr>  write GPIO DD(8-bit hex) 
    //    rec back agwdd   
    //
    //   send AGR<cr>  read GPIO 
    //    rec back agrdd   DD(8-bit hex) 
    //
    //   send AI<cr>  initialize the AMC7812 with the test fixtur default configuraton
    //    rec back ai  

    char tmp2;
    tmp2 = getNextChar();
    if (tmp2 == 'W') // write reg
    {
      // get RR
      char tmp3;
      tmp3 = getNextChar();
      char tmp4;
      tmp4 = getNextChar();
      int tempReg = get8bitval(tmp3, tmp4); // upper 8 bits

      // get DDDD
      tmp3 = getNextChar();
      tmp4 = getNextChar();
      char tmp5;
      tmp5 = getNextChar();
      char tmp6;
      tmp6 = getNextChar();
      unsigned int tempData = get8bitval(tmp3, tmp4); // upper 8 bits
      tempData<<=8;
      tempData += get8bitval(tmp5, tmp6); // lower 8 bits

      m_AMC7812_64.writeReg16(tempReg, tempData);

      String regStr = conver8bitToString((unsigned char)tempReg);
      String dataStr = conver16bitToString(tempData);
      
      // return response
      String resp = "aw"+regStr+dataStr;
      Serial.println(resp);
    }
    else if (tmp2 == 'R') // read reg
    {
      // get RR
      char tmp3;
      tmp3 = getNextChar();
      char tmp4;
      tmp4 = getNextChar();
      int tempReg = get8bitval(tmp3, tmp4); // upper 8 bits
      unsigned int results = 0;
      results = m_AMC7812_64.readReg16(tempReg);
      String regStr = conver8bitToString((unsigned char)tempReg);
      String dataStr = conver16bitToString(results);
      
      // return response
      String resp = "ar"+regStr+dataStr;
      Serial.println(resp);      
    }
    else if (tmp2 == 'D') // set dac
    {
      // get dac number
      char tmp3;
      tmp3 = getNextChar();
      unsigned char dacNum = (unsigned char) convertCharToDecimal(tmp3);

      // get dac 12-bit data
      tmp3 = getNextChar();
      char tmp4;
      tmp4 = getNextChar();
      char tmp5;
      tmp5 = getNextChar();
      unsigned int tempData = get8bitval(tmp3, tmp4); // upper 8 bits   /// error here ???????????????????????
      //Serial.println(conver8bitToString(tempData));
      
      tempData<<=4;
      unsigned int tempData2 = get8bitval('0', tmp5);
      //Serial.println(conver8bitToString(tempData2));
      tempData += get8bitval('0', tmp5); // lower 4 bits   /// error here ???????????????????????
      unsigned int value = 0;
      
      m_AMC7812_64.writeDac(dacNum, value);
      String numStr = conver4bitToString((unsigned char)dacNum);
      String dataStr = conver12bitToString(tempData);    /// error here ???????????????????????
      
      // return response
      String resp = "ad"+numStr+dataStr;
      Serial.println(resp);
    }
    else if (tmp2 == 'A') // read adc        
    {
      char tmp3;
      tmp3 = getNextChar();
      unsigned char adcNum = (unsigned char) convertCharToDecimal(tmp3);
      unsigned int tempData = 0;
      tempData = m_AMC7812_64.readAdc(adcNum);
      
      String numStr = conver4bitToString((unsigned char)adcNum);
      String dataStr = conver12bitToString(tempData);
      
      // return response
      String resp = "aa"+numStr+dataStr;
      Serial.println(resp);
      
    }
    else if (tmp2 == 'I') // Initialize defaults
    {
      m_AMC7812_64.initialize();
      Serial.println("ai");
    }
    else if (tmp2 == 'G') // GPIO 
    {
        char tmp3;
        tmp3 = getNextChar();
        if (tmp3 == 'W') // write GPIO
        {
          char tmp4;
          tmp4 = getNextChar();
          char tmp5;
          tmp5 = getNextChar();
          unsigned int tempData = get8bitval(tmp4, tmp5); // upper 8 bits
          m_AMC7812_64.setGPIO((unsigned char) tempData);
          String dataStr = conver8bitToString(tempData);
          
          // return response
          String resp = "agw"+dataStr;
          Serial.println(resp);
        }
        else if (tmp3 == 'R') // read GPIO
        {
          unsigned char tempData = 0;
          tempData = m_AMC7812_64.getGPIO();
          String dataStr = conver8bitToString(tempData);
      
          // return response
          String resp = "agr"+dataStr;
          Serial.println(resp);
        }
        else
        {
          Serial.println("Error GPIO");
        }
    }
    else
    {
      // bad command
      Serial.println("Error A command");
    }

  }// end if(tmp == 'A')
  else if(tmp='G')  
  {
    //MPC23017
    //   send GWNRRDD<cr>  write device N(0-5) reg RR(8-bit hex)  with data  DD(8-bit hex) 
    //    rec back gwnrrdd   
    //
    //   send GRNRR<cr>  read device N(0-5) reg RR(8-bit hex)  
    //    rec back grnrrdd   with data  DD(8-bit hex) 
    //
    //   send GIN<cr>  initialize device N(0-5) with the test fixture default configuraton
    //    rec back gin
    //
    //   send GARN<cr>  read device N(0-5) port A
    //    rec back garnDD   with data  DD(8-bit hex) 
    //
    //   send GBRN<cr>  read device N(0-5) port B
    //    rec back gbrnDD   with data  DD(8-bit hex) 
    //
    //   send GAWNDD<cr>  write device N(0-5) port A with data  DD(8-bit hex) 
    //    rec back gawnDD   
    //
    //   send GBWNDD<cr>  read device N(0-5) port B with data  DD(8-bit hex) 
    //    rec back gbwnDD  
    char tmp2;
    tmp2 = getNextChar();
    if (tmp2 == 'W') // write reg   
    {
      char tmp3;
      tmp3 = getNextChar(); // device number
      unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp3);

      // get register
      tmp3 = getNextChar();
      char tmp4;
      tmp4 = getNextChar();
      unsigned char tempReg = get8bitval(tmp3, tmp4); //  8 bits
      
      // get data
      tmp3 = getNextChar();
      tmp4 = getNextChar();
      unsigned char tempData = get8bitval(tmp3, tmp4); //  8 bits
      switch (deviceNum)
      {
        case 0:
          m_MCP23017_20.writeReg(tempReg, tempData);
          break;
        case 1:
          m_MCP23017_21.writeReg(tempReg, tempData);
          break;
        case 2:
          m_MCP23017_22.writeReg(tempReg, tempData);
          break;
        case 3:
          m_MCP23017_23.writeReg(tempReg, tempData);
          break;
        case 4:
          m_MCP23017_24.writeReg(tempReg, tempData);
          break;
        default:
          break;
      }
      String deviceStr = conver4bitToString(deviceNum);
      String regStr = conver8bitToString(tempReg);
      String dataStr = conver8bitToString(tempData);      
      // return response
      String resp = "gw" + deviceStr + regStr + dataStr;
      Serial.println(resp);
    }
    else if (tmp2 == 'R') // read reg  
    {
      char tmp3;
      tmp3 = getNextChar(); // device number
      unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp3);

      // get register
      tmp3 = getNextChar();
      char tmp4;
      tmp4 = getNextChar();
      unsigned char tempReg = get8bitval(tmp3, tmp4); //  8 bits
      unsigned char tempData = 0;
      switch (deviceNum)
      {
        case 0:
          tempData = m_MCP23017_20.readReg(tempReg);
          break;
        case 1:
          tempData = m_MCP23017_21.readReg(tempReg);
          break;
        case 2:
          tempData = m_MCP23017_22.readReg(tempReg);
          break;
        case 3:
          tempData = m_MCP23017_23.readReg(tempReg);
          break;
        case 4:
          tempData = m_MCP23017_24.readReg(tempReg);
          break;
        default:
          break;
      }
      String deviceStr = conver4bitToString(deviceNum);
      String regStr = conver8bitToString(tempReg);
      String dataStr = conver8bitToString(tempData);      
      // return response
      String resp = "gr" + deviceStr + regStr + dataStr;
      Serial.println(resp);
      
    }
    else if (tmp2 == 'I') // Initialize  
    {
      char tmp3;
      tmp3 = getNextChar(); // device number
      unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp3);

      switch (deviceNum)
      {
        case 0:
          m_MCP23017_20.initialize();
          break;
        case 1:
          m_MCP23017_21.initialize();
          break;
        case 2:
          m_MCP23017_22.initialize();
          break;
        case 3:
          m_MCP23017_23.initialize();
          break;
        case 4:
          m_MCP23017_24.initialize();
          break;
        default:
          break;
      }     
      String deviceStr = conver4bitToString(deviceNum);  
      // return response
      String resp = "gi" + deviceStr;
      Serial.println(resp);
    }
    else if (tmp2 == 'A') // Port A 
    {
      char tmp3;
      tmp3 = getNextChar();
      if (tmp3 == 'W') // write port A
      {
        char tmp4;
        tmp4 = getNextChar(); // device number
        unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp4);
  
        // get data
        tmp4 = getNextChar();
        char tmp5;
        tmp5 = getNextChar();
        unsigned char tempData = get8bitval(tmp4, tmp5); //  8 bits
        switch (deviceNum)
        {
          case 0:
            m_MCP23017_20.writePortA(tempData);
            break;
          case 1:
            m_MCP23017_21.writePortA(tempData);
            break;
          case 2:
            m_MCP23017_22.writePortA(tempData);
            break;
          case 3:
            m_MCP23017_23.writePortA(tempData);
            break;
          case 4:
            m_MCP23017_24.writePortA(tempData);
            break;
          default:
            break;
        }
        String deviceStr = conver4bitToString(deviceNum);
        String dataStr = conver8bitToString(tempData);      
        // return response
        String resp = "gaw" + deviceStr + dataStr;
        Serial.println(resp);
      
      }
      else if (tmp3 == 'R') // read port A
      {
        char tmp4;
        tmp4 = getNextChar(); // device number
        unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp4);
  
        unsigned char tempData = 0;
        switch (deviceNum)
        {
          case 0:
            tempData = m_MCP23017_20.readPortA();
            break;
          case 1:
            tempData = m_MCP23017_21.readPortA();
            break;
          case 2:
            tempData = m_MCP23017_22.readPortA();
            break;
          case 3:
            tempData = m_MCP23017_23.readPortA();
            break;
          case 4:
            tempData = m_MCP23017_24.readPortA();
            break;
          default:
            break;
        }
        String deviceStr = conver4bitToString(deviceNum);
        String dataStr = conver8bitToString(tempData);      
        // return response
        String resp = "gar" + deviceStr + dataStr;
        Serial.println(resp);
      }
      else
      {
        Serial.println("Error GA command");
      }
    }
    else if (tmp2 == 'B') // Port B 
    {
      char tmp3;
      tmp3 = getNextChar();
      if (tmp3 == 'W') // write port B
      {
        char tmp4;
        tmp4 = getNextChar(); // device number
        unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp4);
  
        // get data
        tmp4 = getNextChar();
        char tmp5;
        tmp5 = getNextChar();
        unsigned char tempData = get8bitval(tmp4, tmp5); //  8 bits
        switch (deviceNum)
        {
          case 0:
            m_MCP23017_20.writePortB(tempData);
            break;
          case 1:
            m_MCP23017_21.writePortB(tempData);
            break;
          case 2:
            m_MCP23017_22.writePortB(tempData);
            break;
          case 3:
            m_MCP23017_23.writePortB(tempData);
            break;
          case 4:
            m_MCP23017_24.writePortB(tempData);
            break;
          default:
            break;
        }
        String deviceStr = conver4bitToString(deviceNum);
        String dataStr = conver8bitToString(tempData);      
        // return response
        String resp = "gbw" + deviceStr + dataStr;
        Serial.println(resp);        
      }
      else if (tmp3 == 'R') // read port B
      {
         char tmp4;
        tmp4 = getNextChar(); // device number
        unsigned char deviceNum = (unsigned char) convertCharToDecimal(tmp4);
  
        unsigned char tempData = 0;
        switch (deviceNum)
        {
          case 0:
            tempData = m_MCP23017_20.readPortB();
            break;
          case 1:
            tempData = m_MCP23017_21.readPortB();
            break;
          case 2:
            tempData = m_MCP23017_22.readPortB();
            break;
          case 3:
            tempData = m_MCP23017_23.readPortB();
            break;
          case 4:
            tempData = m_MCP23017_24.readPortB();
            break;
          default:
            break;
        }
        String deviceStr = conver4bitToString(deviceNum);
        String dataStr = conver8bitToString(tempData);      
        // return response
        String resp = "gbr" + deviceStr + dataStr;
        Serial.println(resp);
      } 
      else
      {
        Serial.println("Error GB command");
      }      
    }
    else // Bad
    {
      Serial.println("Error G command");
    }
    
  }// end else if(tmp='G')
  else if(tmp == 'S')
  {
      Serial.print("found SPI comand ");
      char tmp2;
      while(true)
      {
        tmp = getNextChar();
        
        if (tmp !=0)
        {
          tmp2 = getNextChar();
          int temp16 = get8bitval(tmp, tmp2);
          if(temp16 != -1)
          {
            txrxbuff[buffSize] = temp16;
            buffSize++;
          }
          else break;
        }
        else break;
      }// end while
      // send SPI data
      // to do !!!!!!!!!!!!!
      unsigned int result1 = 0;        // result to return
      unsigned int result2 = 0;        // result to return
      digitalWrite(chipSelectPin, LOW);
      //digitalWrite(SS, LOW);
      //delay(1);
//      SPI.transfer(txrxbuff[0]);       // address
//      result1 = SPI.transfer(txrxbuff[1]);    // data msb
//      result2 = SPI.transfer(txrxbuff[2]);    // data LSB
      //delay(1);
      digitalWrite(chipSelectPin, HIGH);
      //digitalWrite(SS, HIGH);
   
      // return response
      String resp = "s";
      for(int i=0; i<buffSize; i++)
      {
        resp += " ";
        resp += convertToHexChar(txrxbuff[i]);
        //Serial.println(resp + ":"+(String)(i));
      }
      resp += " " + convertToHexChar(result1);
      resp += " " + convertToHexChar(result2);
      //resp += " 00 00"; // temparary untill read SPI
      Serial.println(resp);
  }// end if 
  else if (tmp == 'I')
  {
    Serial.print("found I2C comand ");
  }
  else
  {
    commandBad();
  }
}
//--------------------------
//
//--------------------------
int getRecBufferSize(void)
{
  if (serialBottom == serialTop) return 0;
  if (serialBottom > serialTop) // serialTop has crossed max and gone back through 0
  {
    return (serialTop+1) + (255 - serialBottom);
  }
  else 
  {
    return serialTop - serialBottom ;
  }

}
//-----------------------------------------
//Send out data to serial or telnet
//-----------------------------------------
void sendOutPrint(String dtaStr)
{
    //if(_connected) _server.print(dtaStr);
   // Serial.print(dtaStr); 
    
}
void sendOutPrintln(String dtaStr)
{
    //if(_connected) _server.println(dtaStr);
    //Serial.println(dtaStr); 
    
}


