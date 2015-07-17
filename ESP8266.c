//==============================================================================
#include "ESP8266.h"
#include "Processing.h"
//==============================================================================
unsigned char espRXbuffer[100];
unsigned int rxCntr = 0;
unsigned char timeout = 0;
//==============================================================================
#define TIMEOUT_START   {TBCTL |= (MC_1 | TBCLR);  timeout = 1;}
#define TIMEOUT_STOP    {TBCTL &= ~MC_1; timeout = 0;}
//==============================================================================
void ESP8266Init(void)
{
   DIRA(ESPPORT) |= (CHPD_BIT | RST_BIT);
   OUTA(ESPPORT) |= (CHPD_BIT);
   OUTA(ESPPORT) |= (RST_BIT);
  
   SEL(ESPPORT) |= (TX_BIT | RX_BIT);                        // P3.4,5 = USCI_A0 TXD/RXD
   UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
   UCA0CTL1 |= UCSSEL_2;                     // SMCLK
   UCA0BR0 = 162;                              // 25MHz 9600 (see User's Guide)
   UCA0BR1 = 0;                              // 25MHz 9600
   UCA0MCTL = UCOS16;   
   // over sampling
   UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
   UCA0IE |= UCRXIE | UCTXIE;                         // Enable USCI_A0 RX interrupt
   
   TBCCR0 = 0xffff;
   TBCTL = TASSEL_2/* + MC_1*/ + ID_3;                // ACLK, upmode
   TBCCTL0 |= CCIE; 
   
}
//==============================================================================
unsigned int txCnt, txAmnt;
unsigned char *txByte;
u_TDATA tData = {.msgHeader = 'S', .__0a = 0x0a, .__0d = 0x0d};
//==============================================================================
void espTxMessage(unsigned char *aBuf, unsigned int aCnt)
{
   txCnt = 0;
   txByte =  aBuf;
   UCA0TXBUF = aBuf[0];
   txAmnt = aCnt;
   txCnt++;
}
//==============================================================================
void convertInoToCharArray(signed int aVal, unsigned char* aArr)
{
      if(aVal < 0){ aArr[0] = '-'; aVal *= (-1);} 
      else aArr[0] = '+';
      aArr[1] = (aVal /100)+ '0'; aVal %= 100;
      aArr[2] = (aVal /10)+ '0';      
      aArr[3] = (aVal %10)+ '0';
}
//==============================================================================
void formTXBuffer(signed int * aBuf, unsigned int aMsgNumb)
{
  tData.msgNumber = aMsgNumb + '0';
 
  tData.partsCount = PARTS_COUNT +'0';
  if (VALS_TX_CNT_LAST) tData.partsCount = tData.partsCount + 1;
  
  if(aMsgNumb <= PARTS_COUNT) tData.valsCount = VALS_TX_CNT + '0';
  else                        tData.valsCount = VALS_TX_CNT_LAST + '0';
 
  for(int j = 0; j < VALS_TX_CNT; j++)
        convertInoToCharArray(aBuf[j+(aMsgNumb - 1)*VALS_TX_CNT], tData.cTemperature[j].byte);
}
//==============================================================================
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
      case 0:   break;                             // Vector 0 - no interrupt
      case 2:                                   // Vector 2 - RXIFG

                if(!timeout) TIMEOUT_START;               
                espRXbuffer[rxCntr] = UCA0RXBUF;                  // TX -> RXed character
                if(rxCntr < sizeof(espRXbuffer)) rxCntr++;
                break;
                
      case 4:   // Vector 4 - TXIFG
                if(txCnt < txAmnt) 
                {
                  txByte++;
                  UCA0TXBUF = *txByte;
                  txCnt++;
                }
                break; 
                
      default:  break;
  }
}
//==============================================================================
#define CNT (10)
unsigned int cnt = CNT;
//==============================================================================
#pragma vector = TIMER0_B0_VECTOR
__interrupt void esp_timeout_isr ( void )
{
  if(cnt) cnt--;
  else 
  {
    cnt = CNT;
    TIMEOUT_STOP;    
    status.espMsgIn = 1;
    __low_power_mode_off_on_exit();
  }
}