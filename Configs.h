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
  unsigned char byte[sizeof(u_CONFIG) + 3];
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
extern u_CONFIG configs;
extern u_CONFIG *cPtr;
  