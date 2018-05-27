// Glen Barnett
// May 16, 2018

#include <SPI.h>
//#include <Wire.h>


/*
 * Serial Commands 
 * SPI
 * SW XX...XX  <cr>  N number of XX bytes in hex, separated by space
 *            returns sW<cr>
 * SR n XX...XX <cr>   read n bytes after transmitting XX...XX,  appends 00 for each byte to read
 *            returns sR xx...xx <cr>
 *            
 * SA XX...XX <cr>   read byte after transmitting each XX, 
 *            returns sR xx...xx <cr>
 *            
 *            
 *  I2C          
 *  IW1 AA DD <cr>    write 1 byte DD to address AA
 *            returns iW1<cr>
 *  IW2 AA DDDD <cr>    write 16 bits DDDD to address AA 
 *             returns iW2<cr>
 *  IR1 AA <cr>      read 1 byte 
 *             returns iR1 xx <cr>
 *  IR2 AA <cr>      read 2 bytes
 *             returns iR2 xx xx<cr>
 *  Ir1  <cr>        raw read 1 byte
 *             returns ir1 xx<cr>
 *  
 */
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

void setup() {
  Serial.begin(57600);
  while (!Serial);

  Serial.print("Starting... ");

}

void loop() {
    char inChar;
  if(Serial.available())
  {
    inChar = (char)Serial.read();
    if( (inChar >= 32 && inChar <= 126) || inChar == '\r' || inChar == '\n' )
    {
      pushSerialData(inChar);
      if (inChar == END_CHAR) decodeSerial();
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

  if(MSD <9) MSD += '0';
  else MSD += 'A'-10;

  if(LSD <9) LSD += '0';
  else LSD += 'A'-10;

  return (String)MSD+(String)LSD;
}
//------------------------
// decode the serial commands after \n received
//------------------------
void decodeSerial()
{
  char tmp;
  char tmp2;
  int txrxbuff[20];
  char buffSize = 0; 
/*
 * Serial Commands 
 * SPI
 * SW XX...XX  <cr>  N number of XX bytes in hex, separated by space
 *            returns sW XX...XX <cr>
 * SR n XX...XX <cr>   read n bytes after transmitting XX...XX,  appends 00 for each byte to read
 *            returns sR xx...xx yy...yy<cr> where yy is returned values
 *            
 * SA XX...XX <cr>   read byte after transmitting each XX, 
 *            returns sA xx...xx yy...yy <cr>  where yy is returned values
 *            
 *            
 *  I2C          
 *  IW1 AA DD <cr>    write 1 byte DD to address AA
 *            returns iW1<cr>
 *  IW2 AA DDDD <cr>    write 16 bits DDDD to address AA 
 *             returns iW2<cr>
 *  IR1 AA <cr>      read 1 byte 
 *             returns iR1 xx <cr>
 *  IR2 AA <cr>      read 2 bytes
 *             returns iR2 xx xx<cr>
 *  Ir1  <cr>        raw read 1 byte
 *             returns ir1 xx<cr>
 *  
 */
  tmp = getNextChar();
  if(tmp == 'S')
  {
    Serial.print("found SPI comand ");
    tmp = getNextChar();
    if(tmp == 'W')
    {
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
      }
      // send SPI data
      // to do !!!!!!!!!!!!!

   
      // return response
      String resp = "sW";
      for(int i=0; i<buffSize; i++)
      {
        resp += " ";
        resp += convertToHexChar(txrxbuff[i]);
        //Serial.println(resp + ":"+(String)(i));
      }
      Serial.println(resp);
  
    }
    else if(tmp == 'R')
    {
    
    }
    else if(tmp == 'A')
    {
    
    }
    else
    {
      // error  
      drainTheBuffer();
    }
  }
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


