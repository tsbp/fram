#ifndef Processing_H
#define Processing_H
//==============================================================================
#include "msp430.h"
//==============================================================================
#define UP_BUTTON    (BIT7)
#define DOWN_BUTTON  (BIT4)
#define OK_BUTTON    (BIT6)

#define BL_BIT       (BIT4)
#define BL_INTERVAL  (10)

#define KEY_INTERVAL       (50)
#define CLR_BUTT_INT() {P2IFG &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);\
                        P2IE  |=  (UP_BUTTON | DOWN_BUTTON | OK_BUTTON);}

#define  TA_RUN 	{TA0CTL |= MC_1;} 
#define  TA_STOP	{TA0CTL &=~MC_1;} 

#define MODES_CNT               (4)
//==============================================================================
typedef union
{
  unsigned int byte;
  struct
  {
    unsigned char keyPressed       :   1;
    unsigned char timeChanged      :   1;
    unsigned char lcdOff           :   1;
    unsigned char menuMode         :   1;   
    unsigned char subMenu          :   1;     
    unsigned char subsubMenu       :   1;     
    unsigned char rxUARTmsg        :   1;
    unsigned char modeChanged      :   1;
    unsigned char relLightON       :   1;
    unsigned char relVentON        :   1;
    unsigned char mainScreen       :   1; 
    unsigned char setTT            :   1;
    unsigned char plotRedraw       :   1;
    unsigned char set1             :   1;   
    unsigned char set2             :   1;   
    unsigned char set3             :   1;        
  };
} s_STATUS;
//==============================================================================
typedef struct
{
  unsigned char time;  
  unsigned char temp; 
} s_SET;
#define INTEVAL_CNT (6)
extern s_SET set[3][INTEVAL_CNT];
//==============================================================================
typedef struct 
{
  union
  {
    unsigned int byte[3];
    struct
    {
      unsigned int day;
      unsigned int month;
      unsigned int year;
    };
  }DATE;
  union
  {
    
    unsigned char byte[3];
    struct
    {
      unsigned char sec;
      unsigned char min;
      unsigned char hour;
    };
  }TIME;
}s_DATE_TIME;
//==============================================================================
typedef struct
{
  union
  {
    unsigned char byte[8];
    struct
    {      
      int temper;
      int hysteresis;
      struct
      {
        unsigned char timeStart;
        unsigned char timeStop;
      }light; 
      struct
      {
        unsigned char timeON;
        unsigned char timeOFF;
      }ventilation; 
    };
  };
}s_CONFIG;
//==============================================================================
extern unsigned int dayPointer;
typedef enum {none, downButt, upButt, okButt} e_keyCode;
extern e_keyCode keyCode;
//typedef enum {normal, menu,setTime} e_mode;
//extern e_mode mode;

extern s_STATUS status;
extern unsigned int backLight;
extern s_DATE_TIME date_time;
extern s_STATUS status;
extern unsigned int hysteresis;
extern unsigned  pagePointer, tInterval;
extern int  dayOfWeek;
extern unsigned char STT;
extern s_CONFIG configs[];
extern signed int rcTemper;
//==============================================================================
void mainMenu(void);
void keypressedProceed(void);
void printTime(void);
void printDate(int aX, int aY, unsigned long aCol, unsigned  long aBGCol);
void write_flash(unsigned char * value);
void lightCheck(void);
void ventilationCheck(void);
//==============================================================================
#endif