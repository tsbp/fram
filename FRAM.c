//==============================================================================
#include "FRAM.h"
//==============================================================================
void framOpCode(unsigned char  aCode, unsigned char* aBuf)
{
  unsigned int aBCnt = 0;
  
  _CS(0);
  
  SPI_SEND_BYTE(aCode);
 
  switch(aCode)
  {
        case RDSR: aBCnt = 1;   break;          
        case RDID: aBCnt = 9;   break;
        case SNR:  aBCnt = 8;   break;    
        default:   aBCnt = 0;   break;
  }
  
  if(aBCnt) for(unsigned int i = 0; i < aBCnt; i++)
                  {      
                    SPI_SEND_BYTE(0);
                    aBuf[i] = UCB1RXBUF;
                  }
    
  _CS(1);
}
//==============================================================================
void framWrite(unsigned long aAddr, unsigned char *aData, unsigned int aCnt)
{
  //========== fram write enable ============
  unsigned char aSr = 0;
  while(1)
  {
    framOpCode(RDSR, &aSr);
    if(aSr & BIT1) break;
    framOpCode(WREN, 0);            
  }  
  //=========================================
  _CS(0);  
  SPI_SEND_BYTE(WRITE);
  
  SPI_SEND_BYTE(((unsigned char*)(&aAddr))[2]);
  SPI_SEND_BYTE(((unsigned char*)(&aAddr))[1]);
  SPI_SEND_BYTE(((unsigned char*)(&aAddr))[0]);
  
  for(unsigned int i = 0; i < aCnt; i++)
    SPI_SEND_BYTE(aData[i]);
  
  _CS(1);  
}
//==============================================================================
void framRead(unsigned long aAddr, unsigned char *aData, unsigned int aCnt)
{
  _CS(0);
  
  SPI_SEND_BYTE(READ);
  
  SPI_SEND_BYTE(((unsigned char*)(&aAddr))[2]);
  SPI_SEND_BYTE(((unsigned char*)(&aAddr))[1]);
  SPI_SEND_BYTE(((unsigned char*)(&aAddr))[0]);
    
  
  for(unsigned int i = 0; i < aCnt; i++)
  {
    SPI_SEND_BYTE(0);
    aData[i] = UCB1RXBUF;
  }
  
  _CS(1);
}
//==============================================================================
void framSPIinit(void)
{
  P4DIR |= (SIMO_BIT | CLK_BIT);                                          // it is not yet in SPI mode
  P4SEL |= (SOMI_BIT | SIMO_BIT | CLK_BIT);                       // P3.3,4 option select
  P4DIR |= (CS_BIT);                            // P2.7 option select
  _CS(1); 
  UCB1CTL1 |=  UCSWRST;                      // **Put state machine in reset**
  UCB1CTL1 |= (UCSSEL0 | UCSSEL1);
  UCB1CTL0 |= (UCMST | UCSYNC |  UCCKPH | UCMSB);          
  UCB1BR0 = 10;                                            
  UCB1CTL1 &= ~UCSWRST;   
}