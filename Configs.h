//==============================================================================
#include "msp430.h"
//==============================================================================
#define PERIODS_CNT     (6)
//==============================================================================
typedef union
{
    unsigned char byte[5];
    struct
    {
       unsigned char hStart[2];
       unsigned char mStart[2];
       unsigned char temperature[3];
    };
}s_PCONFIG;
//==============================================================================
#define CONF_ARRAY_LENGTH (PERIODS_CNT * sizeof(s_PCONFIG))
//==============================================================================
typedef union
{
  unsigned char byte[1 + CONF_ARRAY_LENGTH];
  struct
  {
    unsigned char periodsCnt;
    s_PCONFIG pConfig[PERIODS_CNT];    
  };
}u_CONFIG;
//==============================================================================
typedef union
{
  unsigned char byte[sizeof(u_CONFIG) + 5];
  struct
  {
    unsigned char  msgHeader;
    unsigned char  msgNumber;
    unsigned char  partsCount;
    s_PCONFIG config;
    unsigned char _0a;
    unsigned char _0d;
  };
}u_CONFIG_TX_BUFFER;
extern u_CONFIG_TX_BUFFER configTXBuffer;
//==============================================================================
typedef union
{
  unsigned char byte[11];
  struct
  {
    unsigned int interval;
    unsigned int delta;
    unsigned char day[7];
  };  
}u_NASTROYKI;
extern u_NASTROYKI *nastroyki;
//==============================================================================
extern u_NASTROYKI nastr;
extern u_CONFIG configs;
extern u_CONFIG *cPtrH, *cPtrW;
//==============================================================================
unsigned char* getSetTemperature(unsigned int aTime);
unsigned char cmpTemperature (unsigned char *aT, signed int arcTemper);
  