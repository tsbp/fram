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
#include "configs.h"
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
int rcEnCntr;
unsigned char tmpRxBuf[1 + 70];
unsigned char okAnswer[] = {"OKA"};
//==============================================================================
#define HEAT_BIT        (BIT0)
#define HEAT_OUT        (P3OUT)
#define HEAT_DIR        (P3DIR)
#define HEAT(n) {if (n) HEAT_OUT |= HEAT_BIT; else HEAT_OUT &= ~HEAT_BIT;}
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
  
  HEAT_OUT &= ~HEAT_BIT;
  HEAT_DIR |=  HEAT_BIT;
  
  __enable_interrupt();   
  
//  write_flash(configs.byte, sizeof(u_CONFIG), 0x1800);
//  write_flash(configs.byte, sizeof(u_CONFIG), 0x1880);
//  write_flash(nastr.byte, sizeof(u_NASTROYKI), 0x1900);
  
    df_Init();  
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
    TA1CTL = TASSEL_2 + ID_3;                // ACLK, upmode
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
    //===========================================  
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
        
        //======== heater manage ======================            
        if(cmpTemperature(getSetTemperature(date_time.TIME.hour * 60 + date_time.TIME.min),
                          rcTemper))        
          HEAT(1)// heating
        else
          HEAT(0);// heating off       
    }
    else 
    {
      lineStatus.msgIn = 0;
      bcnt = 0;
    } 
    //===========================================    
    if(rcEnCntr) rcEnCntr--;
    else DA_EN(1);
    //===========================================
    if(status.espMsgIn)
    {       
      //========= save day configs ===========================    
      if     (espRXbuffer[0] == 'C' && espRXbuffer[1] == 'S' && espRXbuffer[2] == 'A' && espRXbuffer[3] == 'V')
      {
            u_CONFIG *ptr;
            if(espRXbuffer[4]== 'H') ptr = cPtrH;
            else                     ptr = cPtrW;
          
          for(unsigned int j = 0; j < 7; j++)        
            tmpRxBuf[7*(espRXbuffer[5] - '0'-1) + j + 1] = espRXbuffer[7+j];
          
          if(espRXbuffer[5] == espRXbuffer[6]) 
          {
              tmpRxBuf[0] = espRXbuffer[6];
              write_flash(tmpRxBuf, (espRXbuffer[6] - '0')*7 + 1, (unsigned int) ptr);
          }          
          
          okAnswer[2] = espRXbuffer[5];
          espTxMessage(okAnswer, sizeof(okAnswer));       
      }
      //========= save week configs ===========================
      else if(espRXbuffer[0] == 'C' && espRXbuffer[1] == 'S' && espRXbuffer[2] == 'A' && espRXbuffer[3] == 'W')
      {
         //== write flash ================
          for(unsigned int m = 0; m < (sizeof(u_NASTROYKI)); m++) tmp[m] = nastroyki -> byte[m];
          for(unsigned int m = 4; m < (sizeof(u_NASTROYKI)); m++) tmp[m] = espRXbuffer[m];
          write_flash(tmp, sizeof(u_NASTROYKI), 0x1900);
          
          okAnswer[2] = 'W';
          espTxMessage(okAnswer, sizeof(okAnswer));          
      }
      //========= read week configs ===========================
      else if(espRXbuffer[0] == 'W' && espRXbuffer[1] == 'E' && espRXbuffer[2] == 'E' && espRXbuffer[3] == 'K')
      {
        unsigned char weekTxBuf[11];
        weekTxBuf[0] = 'W';
        weekTxBuf[1] = 'C';
        weekTxBuf[9] = 0x0a;
        weekTxBuf[10] = 0x0d;
        for(unsigned int i = 0; i < 7; i++) weekTxBuf[i+2] = nastroyki -> byte[i+4];
        espTxMessage(weekTxBuf, sizeof(weekTxBuf));
      }
      //========= read day configs ===========================
      else if(espRXbuffer[0] == 'C' && espRXbuffer[1] == 'O' && espRXbuffer[2] == 'N' && espRXbuffer[3] == 'F')
      {
            u_CONFIG *ptr;
            if(espRXbuffer[4]== 'H') ptr = cPtrH;
            else                     ptr = cPtrW;
            configTXBuffer.msgNumber  = espRXbuffer[5];
            configTXBuffer.partsCount = ptr -> periodsCnt;
            
            for(unsigned int i = 0; i < sizeof(s_PCONFIG); i++)
              configTXBuffer.config.byte[i] = ptr->pConfig[espRXbuffer[5]-'0'-1].byte[i];
            
            espTxMessage(configTXBuffer.byte, sizeof(configTXBuffer.byte));
      }
      //========= read plot data ===========================
      else 
      {
        if(espRXbuffer[0] == 'O')
          {
            tBuffer[POINTS_CNT - 1] = temp_buffer[0];
            tData.msgHeader = 'O';
            formTXBuffer(tBuffer,  espRXbuffer[1] - '0');            
          }
          else
          {
            tBuffer2[POINTS_CNT - 1] = rcTemper;
            tData.msgHeader = 'I';
            formTXBuffer(tBuffer2, espRXbuffer[1] - '0');
            //============ time syncronization ===================
            if(espRXbuffer[1] == '1')
            {
                  date_time.DATE.year  = (espRXbuffer[2]  - '0')*1000 +
                                         (espRXbuffer[3]  - '0')*100  +
                                         (espRXbuffer[4]  - '0')*10   +
                                         (espRXbuffer[5]  - '0');       
                  date_time.DATE.month = (espRXbuffer[7]  - '0')*10 +                               
                                         (espRXbuffer[8]  - '0' - 1);
                  date_time.DATE.day   = (espRXbuffer[10] - '0')*10 +                               
                                         (espRXbuffer[11] - '0');
                  
                  date_time.TIME.hour =  (espRXbuffer[13] - '0')*10 +                               
                                         (espRXbuffer[14] - '0');       
                  date_time.TIME.min   = (espRXbuffer[16] - '0')*10 +                               
                                         (espRXbuffer[17] - '0');
                  date_time.TIME.sec   = (espRXbuffer[19] - '0')*10 +                               
                                         (espRXbuffer[20] - '0');
            }        
            //=========================================
          }          
          espTxMessage(tData.byte, sizeof(tData.byte));
      }     
      status.espMsgIn = 0;
      rxCntr = 0;
      for(unsigned int i = 0; i < 100; i++) espRXbuffer[i] = 0;
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
                    showGraphic(tBuffer, 0);
                    showGraphic(tBuffer2, 160);
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