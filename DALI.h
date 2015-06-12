//==============================================================================
//#define MASTER
//==============================================================================
#ifndef DALI_H
#define DALI_H
//==============================================================================
#include "hal.h"
#include <intrinsics.h>
//==============================================================================
#define MAKE_IN(port)  port##IN
#define MAKE_OUT(port) port##OUT
#define MAKE_DIR(port) port##DIR
#define MAKE_SEL(port) port##SEL
#define MAKE_IE(port)  port##IE
#define MAKE_IES(port) port##IES
#define MAKE_IFG(port) port##IFG
//==============================================================================
#define OUTA(port) MAKE_OUT(port)
#define IN(port)  MAKE_IN(port)
#define DIRA(port) MAKE_DIR(port)
#define SEL(port) MAKE_SEL(port)
#define IE(port)  MAKE_IE(port)
#define IES(port) MAKE_IES(port)
#define IFG(port) MAKE_IFG(port)
//==============================================================================
#define DA_PORT         P2
#define DA_TX_BIT       BIT1
#define DA_RX_BIT       BIT5
//==============================================================================
#define DA_EN_PORT      P5
#define DA_EN_BIT       BIT4
#define DA_EN(n)        {if(n) OUTA(DA_EN_PORT) |= DA_EN_BIT; else OUTA(DA_EN_PORT) &= ~DA_EN_BIT;}
//==============================================================================
#define LEDSTT_BIT      (BIT3)
#define LEDSTT(n)       {if(n) OUTA(DA_PORT) |= LEDSTT_BIT; else OUTA(DA_PORT) &= ~LEDSTT_BIT;}
//==============================================================================
#define BACKWARD         (0)
#define FORWARD          (1)
//==============================================================================
#define DA_TX_ERROR        (0)
#define DA_TX_SUCCESS      (1)
//==============================================================================
void DA_Init(void);
unsigned char DA_Frame(unsigned char aFrame, unsigned char aAddr, unsigned char aData);
//==============================================================================
extern unsigned char rxByte, rxBuffer[15];
extern unsigned int bcnt;
//==============================================================================
typedef union
{
  unsigned char byte;
  struct
  {
    unsigned char txEn             :1;
    unsigned char busy             :1;
    unsigned char msgIn           :1;
  };
}u_DA_LINE_STATUS;
extern u_DA_LINE_STATUS lineStatus;
//==============================================================================
#endif