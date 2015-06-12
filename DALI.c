//==============================================================================
#include "DALI.h"
#include "Processing.h"
//==============================================================================
#define MCUCLK          (25000000)
#define T_DIVIDER       (8)
#define BITRATE         (1200)
#define HALFBIT         (MCUCLK/(BITRATE * 2 * T_DIVIDER))
#define BITOFFSET       (200)
//==============================================================================
#define DA_TIMER_START  {TA2CTL |=  MC_1;}
#define DA_TIMER_STOP   {TA2CTL &= ~MC_1;}
//==============================================================================
u_DA_LINE_STATUS lineStatus;
unsigned int bcnt = 0;
unsigned char rxByte;
unsigned char rxBuffer[15];
//==============================================================================
void DA_Init(void)
{
  OUTA(DA_EN_PORT)  &= ~DA_EN_BIT; 
  SEL(DA_EN_PORT)  &= ~DA_EN_BIT; 
  DIRA(DA_EN_PORT) |=  DA_EN_BIT; 
  
//  DIRA(DA_PORT) |=  DA_TX_BIT;
//  OUTA(DA_PORT) &= ~DA_TX_BIT;  
  
  DIRA(DA_PORT) &= ~DA_RX_BIT;
  IFG(DA_PORT) &= ~DA_RX_BIT;
  IES(DA_PORT) |=  DA_RX_BIT;
  IE (DA_PORT) |=  DA_RX_BIT;

  //== Timer RX INIT ====
  TA2CTL   = (TASSEL_2 |  TAIE | ID_3);
  TA2CCTL2 = (CM_3 | CCIS_0 | CAP | SCS); 
  TA2CCR0  = HALFBIT*5; 
  TA2CCR1  = HALFBIT;
  
  DA_EN(1);
  //P3DIR |= BIT3;
}
//==============================================================================
unsigned int timerTMP;
unsigned int lastCCR;
//==============================================================================
#pragma vector = PORT2_VECTOR
__interrupt void start_bit_ISR ( void )
{  
  if      (P2IFG & UP_BUTTON)   keyCode = upButt;
  else if (P2IFG & DOWN_BUTTON) keyCode = downButt;
  else if (P2IFG & OK_BUTTON)   keyCode = okButt;
  else                          keyCode = none;
  
  if (keyCode != none)
  {
    DA_EN(0);    
    status.keyPressed = 1; 
    P2IFG &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
    P2IE  &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
     __low_power_mode_off_on_exit();
  }
  else
  {
    P3OUT |= BIT3;
    TA2CCTL1 |= CCIE;
    TA2CCR0  = HALFBIT*5;
    lastCCR  = 0;  
    DA_TIMER_START;
    IE (DA_PORT)  &= ~DA_RX_BIT;
    SEL(DA_PORT) |=  DA_RX_BIT;
    IFG(DA_PORT) &= ~DA_RX_BIT;  
    lineStatus.busy = 1;
  }
}
//==============================================================================
void DA_Transceiver_reset(void)
{
  DA_TIMER_STOP;                                                                              
  TA2CCTL2 &= ~CCIE;                   
  SEL(DA_PORT) &= ~DA_RX_BIT;                                            
  IFG(DA_PORT) &= ~DA_RX_BIT; 
  IE (DA_PORT) |=  DA_RX_BIT;
  
  //P3OUT &= ~BIT3;
}
//==============================================================================
#pragma vector = TIMER2_A1_VECTOR
__interrupt void TA1_SYNC_LINE ( void )
{
  static unsigned int secondEdge;
  static unsigned int currentBit;
  static unsigned int nextBit;
  static unsigned int bitCounter;
  
  switch(TA2IV)
  {
       case 2: //start bit                   
                   TA2CCTL1 &= ~CCIE;
                   currentBit = 0;
                   bitCounter = 0;
                   secondEdge = 0;                                          
                   rxByte = 0;
                   //P3OUT &= ~BIT3;
                   TA2CCTL2 &=~ (CCIFG | COV); 
                   TA2CCTL2 |=  CCIE;
                   break;
       case 4:       
                      timerTMP = TA2CCR2 - lastCCR;
                      lastCCR  = TA2CCR2;
                      
                      TA2CCR0  = lastCCR + (HALFBIT*5);
                      TA2CCTL2 &=~ (CCIFG | COV);              
                      
                      if ((timerTMP > (2*HALFBIT - BITOFFSET)) &&      // doble interval
                          (timerTMP < (2*HALFBIT + BITOFFSET)) )                         
                      {
                        nextBit = currentBit ^ 1;
                        secondEdge = 0;                                          
                      }
                      else if ((timerTMP > (HALFBIT - BITOFFSET)) &&   // single interval
                               (timerTMP < (HALFBIT + BITOFFSET)))
                      {
                        if(secondEdge)
                        {
                          nextBit = currentBit;
                          secondEdge = 0;                                             
                        }
                        else secondEdge = 1;
                      }                        
                      else DA_Transceiver_reset();
                      
                      if(!secondEdge)
                      {                                                          
                        bitCounter++;
                        if (bitCounter > 8) DA_Transceiver_reset();
                        rxByte <<= 1;
                        if(nextBit) rxByte += 1;
                        currentBit = nextBit;                                                          
                      }
                      break;
       case 0x0e:                      
                      lineStatus.busy = 0;
                      DA_Transceiver_reset();                        
                      if (bitCounter >= 8) 
                      {
                          bitCounter = 0;                       
                          rxBuffer[bcnt++] = rxByte;
                          if((rxBuffer[bcnt-1] == 0x0a) &&
                             (rxBuffer[bcnt-2] == 0x0d)) 
                          {
                              DA_EN(0);
                              lineStatus.msgIn = 1;
                              __low_power_mode_off_on_exit();
                          }
                      }
                      break;
  }  
}
