//==============================================================================
#ifndef ESP8266_H
#define ESP8266_H
//==============================================================================
#include "hal.h"
#include "macro.h"
#include "plot.h"
#include <intrinsics.h>
//==============================================================================
#define ESPPORT    P3
#define TX_BIT    (BIT3)
#define RX_BIT    (BIT4)
//==============================================================================
#define CHPD_BIT  (BIT1)
#define ESP_CHPD(n) {if(n) OUT(ESPPORT) |= CHPD_BIT; else OUT(ESPPORT) &= ~CHPD_BIT;}
#define RST_BIT   (BIT2)
#define ESP_RST(n) {if(n) OUT(ESPPORT) |= RST_BIT; else OUT(ESPPORT) &= ~RST_BIT;}
//==============================================================================
#define PARTS_COUNT       (3)
#define VALS_TX_CNT       (POINTS_CNT / PARTS_COUNT)
#define VALS_TX_CNT_LAST  (POINTS_CNT % PARTS_COUNT) 
//==============================================================================
typedef struct
{
  unsigned char byte[3]; 
}s_CTEMPERATURE;
//==============================================================================
typedef union
{
  unsigned char byte[1 + 3 + /*6 + */VALS_TX_CNT*3 + 2];
  struct
  {
    unsigned char  msgHeader;
    unsigned char  msgNumber;
    unsigned char  partsCount;
    unsigned char  valsCount;
//    unsigned char inT[3];
//    unsigned char outT[3];
    s_CTEMPERATURE cTemperature[VALS_TX_CNT];   
    unsigned char __0a;
    unsigned char __0d;
  }; 
}u_TDATA;
extern u_TDATA tData;
//==============================================================================
void ESP8266Init(void);
void espTxMessage(unsigned char *aBuf, unsigned int aCnt);
void formTXBuffer(signed int * aBuf, unsigned int aMsgNumb);
//==============================================================================
extern unsigned int rxCntr;
extern unsigned char espRXbuffer[];
//==============================================================================
#endif