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

u_CONFIG *cPtrW  = (u_CONFIG*) 0x1800;
u_CONFIG *cPtrH  = (u_CONFIG*) 0x1880;

//u_NASTROYKI nastr = {.interval = 600, .delta = 5,
//                             .day[0] = 'W',
//                             .day[1] = 'H',
//                             .day[2] = 'W',
//                             .day[3] = 'H',
//                             .day[4] = 'W',
//                             .day[5] = 'H',
//                             .day[6] = 'H'};
u_NASTROYKI *nastroyki = (u_NASTROYKI*) 0x1900;
//==============================================================================
unsigned char* configProceed(unsigned int aTime, u_CONFIG *aPtr)
{
  unsigned int curPeriod;
  for(curPeriod = 0; curPeriod < (aPtr -> periodsCnt - '0' - 1); curPeriod++)
  {
    unsigned int end = ((aPtr -> pConfig[curPeriod + 1].hStart[0] - '0') * 10 +   (aPtr -> pConfig[curPeriod + 1].hStart[1] - '0')) * 60 +
                       ((aPtr -> pConfig[curPeriod + 1].mStart[0] - '0') * 10 +   (aPtr -> pConfig[curPeriod + 1].mStart[1] - '0'));
    
    if(aTime < end)  break;     
  }
  return aPtr -> pConfig[curPeriod].temperature;
}
