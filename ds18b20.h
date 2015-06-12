#ifndef DS18B20_H
#define DS18B20_H
//==============================================================================
#include "delay.h"
//==============================================================================
#define MAX_DS1820_NUMB  (2)
#define CONF_REG        (0x7f)
//==============================================================================
typedef union
{
  unsigned char data[8];
  struct
  {
    unsigned char famCode;
    unsigned char serNumb[6];
    unsigned char crc;
  };
}s_DEV_ID;
//==============================================================================
typedef unsigned char uchar; 
typedef unsigned int  uint;
//==============================================================================
extern void TA_Init(void);
extern void DelayNus(uint n);
extern uchar Init_18B20(void); 
//extern void Write_18B20(uchar wdata); 
extern uchar Read_18B20(void);
extern void Skip(void);
extern void Convert(void); 
extern void Read_SP(void);

void OWFind(void);
void OWConfig(void);
//==============================================================================
int  OWFirst();
int  OWNext();
int  OWVerify();
void OWTargetSetup(unsigned char family_code);
void OWFamilySkipSetup();
int  OWReset();
void OWWriteByte(unsigned char byte_value);
void OWWriteBit(unsigned char bit_value);
unsigned char OWReadBit();
int  OWSearch();
unsigned char docrc8(unsigned char value);
//void readOWLine(void);
void readOWLine(unsigned int aDev);
// global search state
extern unsigned char ROM_NO[8];
extern int LastDiscrepancy;
extern int LastFamilyDiscrepancy;
extern int LastDeviceFlag;
extern unsigned char _crc8;
extern unsigned int DevicesCount;
//==============================================================================
extern s_DEV_ID device[MAX_DS1820_NUMB];
extern unsigned char tmpBuf[9];
extern signed int temp_buffer[MAX_DS1820_NUMB];
//==============================================================================
#define MATCH_ROM        (0x55)
#define SKIP_ROM         (0xCC)
#define CONVERT          (0x44)
#define READ_SCRATCPAD   (0xBE)
#define WRITE_SCRATCPAD  (0x4E)
//==============================================================================
#endif