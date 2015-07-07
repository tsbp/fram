//==============================================================================
#include "configs.h"
//==============================================================================
u_CONFIG_TX_BUFFER configTXBuffer = {.msgHeader = 'C', ._0a = 0x0a, ._0d = 0x0d};
//u_CONFIG configs ={.periodsCnt = '6', 
//                   .pConfig[0].hStart = "10", .pConfig[0].mStart = "22", .pConfig[0].temperature = "223",
//                   .pConfig[1].hStart = "11", .pConfig[1].mStart = "23", .pConfig[1].temperature = "234",
//                   .pConfig[2].hStart = "12", .pConfig[2].mStart = "24", .pConfig[2].temperature = "245",
//                   .pConfig[3].hStart = "13", .pConfig[3].mStart = "25", .pConfig[3].temperature = "256",
//                   .pConfig[4].hStart = "14", .pConfig[4].mStart = "26", .pConfig[4].temperature = "267",
//                   .pConfig[5].hStart = "15", .pConfig[5].mStart = "27", .pConfig[5].temperature = "278"};

u_CONFIG *cPtr  = (u_CONFIG*) 0x1800;
//==============================================================================
unsigned char* configProceed(unsigned int aTime)
{
  unsigned int curPeriod;
  for(curPeriod = 0; curPeriod < (cPtr -> periodsCnt - '0' - 1); curPeriod++)
  {
    unsigned int end = ((cPtr -> pConfig[curPeriod + 1].hStart[0] - '0') * 10 +   (cPtr -> pConfig[curPeriod + 1].hStart[1] - '0')) * 60 +
                       ((cPtr -> pConfig[curPeriod + 1].mStart[0] - '0') * 10 +   (cPtr -> pConfig[curPeriod + 1].mStart[1] - '0'));
    
    if(aTime < end)  break;     
  }
  return cPtr -> pConfig[curPeriod].temperature;
}
