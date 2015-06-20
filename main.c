//==============================================================================
#include "hal.h"
#include "processing.h"
#include "n6500s.h"
#include "ds18b20.h"
#include "dataflash.h"
#include "plot.h"
#include "dali.h"
#include "crc.h"
#include "ESP8266.h"
//==============================================================================
unsigned int  min, hour, measint;
//==============================================================================
//======= quack ===
#define STEPS_COUNT_Q     (50)
#define STEP_Q            (8)
//======= piu =====
#define STEPS_COUNT_P     (80)
#define STEP_P            (3)
unsigned int step = STEP_Q, steps_count = STEPS_COUNT_Q;
unsigned int curStep = 1;unsigned int goUP = 1;


//==============================================================================
unsigned char test[24] = {1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3};
//unsigned char rxBuffer[15];
//unsigned int bcnt = 0;
//long t;
int rcEnCntr;
//==============================================================================
void printTemp(void)
{
  int a = temp_buffer[0];    
  picFromFlash(121, 74, 20, 30, (a / 100) *4); a %=100;
  picFromFlash(141, 74, 20, 30, (a / 10)  *4);
  picFromFlash(161, 74, 20, 30, 12 *4);
  picFromFlash(171, 74, 20, 30, (a % 10 ) *4);
  picFromFlash(191, 74, 20, 30, 10 *4);  
  
  //temp_buffer[0] = 0;

  a = (int)rcTemper;
  picFromFlash(28, 217, 20, 30, (a / 100) *4); a %=100;
  picFromFlash(48, 217, 20, 30, (a / 10)  *4);
  picFromFlash(68, 217, 20, 30, 12 *4);
  picFromFlash(78, 217, 20, 30, (a % 10 ) *4);
  picFromFlash(98, 217, 20, 30, 10 *4);  
}

//==============================================================================
void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  for(unsigned int i = 0; i < 0xffff; i++);   
  PMM_setVCore(PMM_CORE_LEVEL_2);
  initClocks(25000000);   
  
  DA_Init();
  
  
  LCD_Init();
  LCD_wakeup();
  
  ESP8266Init();

  tData.__0a = 0x0a;
  tData.__0d = 0x0d;
  
  __enable_interrupt();    
  
  df_Init();
  
//  rectangle (0, 0,240, 320, BLACK);
    picFromFlash(0, 0, 240, 320, 100);
  
   //======== buttons ========
    P2DIR &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
    P2REN |=  (UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
    P2OUT |=  (UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
    P2IES |=  (UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
    P2IFG &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
    P2IE  |=  (UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
  
  //======== DS18B20 =======================
#define DS_PWR_BIT      (BIT6)
    P4DIR |= (DS_PWR_BIT);
    P4OUT |= (DS_PWR_BIT);
    P4OUT &= ~BIT7;
    OWFind();
    OWConfig(); 
    while(Init_18B20());
    OWWriteByte(SKIP_ROM);
    OWWriteByte(CONVERT);
    
   //======== Sound ========
    P2DIR |= (BIT0 | BIT1);
    P2DS  |= (BIT0 | BIT1);
    P2OUT |= (BIT0);
    TA1CCR0 = 100;
    TA1CTL = TASSEL_2/* + MC_1*/ + ID_3;                // ACLK, upmode
    TA1CCTL0 |= CCIE; 
    //=====================
  
  TA0CCR0 = 32768;
  TA0CTL = TASSEL_1 + MC_1 + TACLR; // ACLK, count to CCR0 then roll, clear TAR
  TA0CCTL0 = CCIE;              // Gen int at rollover (TIMER0_A0 vector)
  
  for(unsigned int i = 0; i < POINTS_CNT; i++) tBuffer[i] = 220; 
  
  while(1)
  { 
    if (status.keyPressed) 
     {
       TA1CTL |= (MC_1 | TACLR);
       P2OUT |= BIT0;
       keypressedProceed(); 
     }
    
    if(lineStatus.msgIn && (crc8(rxBuffer+(bcnt-8), 5, 0) == rxBuffer[bcnt-3]))
    {
        lineStatus.msgIn = 0;
        ((unsigned char *)&rcTemper)[0] = rxBuffer[bcnt-5];
        ((unsigned char *)&rcTemper)[1] = rxBuffer[bcnt-4];
        bcnt = 0;
        rcEnCntr = 10;
        //==== ds =========
        for(int i = 0; i < DevicesCount; i++) readOWLine(i);         
        // запуск преобразования    
        while(Init_18B20());
        OWWriteByte(SKIP_ROM);
        OWWriteByte(CONVERT);    
        if(pagePointer == 0) printTemp();      
    }
    //===========================================    
    if(rcEnCntr) rcEnCntr--;
    else DA_EN(1);
    //===========================================
    if(status.espMsgIn)
    {
      if(espRXbuffer[0] == 'I')
      {
        tBuffer[POINTS_CNT - 1] = temp_buffer[0];
        tData.msgHeader = 'I';
        formTXBuffer(tBuffer,  espRXbuffer[1] - '0');
      }
      else
      {
        tBuffer2[POINTS_CNT - 1] = rcTemper;
        tData.msgHeader = 'O';
        formTXBuffer(tBuffer2, espRXbuffer[1] - '0');
      }
      
      espTxMessage(tData.byte, sizeof(tData.byte));
     
      status.espMsgIn = 0;
      rxCntr = 0;
    }
    //===========================================
    if(status.timeChanged)
    {
     status.timeChanged = 0;
     switch(pagePointer)
     {
             case 0:
               printTime();
               printDate(28, 176, YELLOW, BLACK);               
               break;
               
             case 1:
               if(status.plotRedraw)
                  {
                    showGraphic(tBuffer, 0);//(temp_buffer[0]);  
                    showGraphic(tBuffer2, 160);//(temp_buffer[0]);  
                    status.plotRedraw = 0;
                  }
               break;         
     }       
    }

    __low_power_mode_0();
  }

}
//==============================================================================
#define MAX_BCOUNT       (3)
unsigned char bCount = MAX_BCOUNT;
//==============================================================================
#pragma vector = TIMER1_A0_VECTOR
__interrupt void siren_isr (void)
{
  
  TA1CCR0 = curStep*step;                   
  
//  if(curStep & BIT0) TA2CCR0 = curStep*step;
//                    else            TA2CCR0 = 20;
  
  if(P2OUT & BIT0) {P2OUT &= ~BIT0; P2OUT |= BIT1;}
  else
    if(P2OUT & BIT1) {P2OUT &= ~BIT1; P2OUT |= BIT0;}
  
  if(goUP) curStep++;
  else     curStep--;
  if( goUP && (curStep >  steps_count)) goUP = 0;
  if(!goUP && (curStep <= 1))           
  {
    if(bCount) bCount--;
    else {TA1CTL &= ~MC_1; bCount = MAX_BCOUNT; P2OUT &= ~(BIT1 |BIT0);}
    goUP = 1;  
  }
}