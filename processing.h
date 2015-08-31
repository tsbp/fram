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
    unsigned char timeSet          :   1;
    unsigned char menuMode         :   1;   
    unsigned char subMenu          :   1;     
    unsigned char subsubMenu       :   1;     
    unsigned char espMsgIn         :   1;
    unsigned char modeChanged      :   1;    
    unsigned char mainScreen       :   1;     
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
#define M_TIMEOUT       (10)
extern unsigned char menuTimeout;
//==============================================================================
extern unsigned int dayPointer;
typedef enum {none, downButt, upButt, okButt} e_keyCode;
extern e_keyCode keyCode, keyCode2;
extern s_STATUS status;
extern unsigned int backLight;
extern s_DATE_TIME date_time;
extern s_STATUS status;
extern unsigned int hysteresis;
extern unsigned  pagePointer, tInterval;
extern int  dayOfWeek;
extern unsigned char STT;
extern signed int rcTemper;
extern unsigned char tmp[];
//==============================================================================
void mainMenu(void);
void keypressedProceed(void);
void printTime(void);
void printDate(int aX, int aY, unsigned long aCol, unsigned  long aBGCol);
void write_flash(unsigned char * value, unsigned int aCnt , unsigned int aAddr);
void lightCheck(void);
void ventilationCheck(void);
int getDayOfWeek(void);
void printTemp(void);
//==============================================================================
#endif