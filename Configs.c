//==============================================================================
#include "configs.h"
#include "processing.h"
#include "n6500s.h"
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
unsigned char* getSetTemperature(unsigned int aTime)  // return ptr to set temper to INFO
{
  int aDayNumber = getDayOfWeek();
  unsigned char aDay;
  u_CONFIG *aPtr;
  unsigned long col;
  
  if      (aDayNumber == 0)  aDay = nastroyki -> day[5];
  else if (aDayNumber == 1)  aDay = nastroyki -> day[6];
  else                       aDay = nastroyki -> day[aDayNumber - 2]; 
  
  aPtr = (aDay == 'H') ? cPtrH : cPtrW;
  
  if(pagePointer == 0)
  {
    if(aDay == 'W') { aDay = 'P'; col = BLUE;}
    else            { aDay = 'B'; col = RED;}
    char_6x8(160, 284, col, 0xffff64, aDay);  
  }
      
  unsigned int curPeriod;
  for(curPeriod = 0; curPeriod < (aPtr -> periodsCnt - '0' - 1); curPeriod++)
  {
    unsigned int end = ((aPtr -> pConfig[curPeriod + 1].hStart[0] - '0') * 10 +   (aPtr -> pConfig[curPeriod + 1].hStart[1] - '0')) * 60 +
                       ((aPtr -> pConfig[curPeriod + 1].mStart[0] - '0') * 10 +   (aPtr -> pConfig[curPeriod + 1].mStart[1] - '0'));    
    if(aTime < end)  break;     
  }
  return aPtr -> pConfig[curPeriod].temperature;
}
//==============================================================================
unsigned char cmpTemperature (unsigned char *aT, signed int arcTemper)
{  
  static unsigned char out = 0;
  static unsigned long col = 0x44916c;
  int tmp = (aT[0] - '0') * 100 + (aT[1] - '0') * 10 + (aT[2] - '0');
  if      (arcTemper > tmp + (nastroyki -> delta))  
  {
    col = 0x44916c; 
    out = 0; 
  }
  else if (arcTemper < tmp - (nastroyki -> delta)) 
  {
    col = 0xdb214c;
    out = 1;
  }
  
  if(pagePointer == 0)
  {
      char_6x8(174, 284, col, 0xffff64, aT[0]);
      char_6x8(186, 284, col, 0xffff64, aT[1]);
      char_6x8(198, 284, col, 0xffff64, ',');
      char_6x8(206, 284, col, 0xffff64, aT[2]);
  } 
 return out; 
}