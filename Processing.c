//==============================================================================
#include "Processing.h"
#include "n6500s.h"
#include "delay.h"
#include "plot.h"
#include "ds18b20.h"
#include "DALI.h"
//==============================================================================
unsigned char STT = 0;
s_DATE_TIME date_time = {.DATE.day   = 8,
                         .DATE.month = 2,
                         .DATE.year  = 2013};
unsigned char daysInMonth[]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char   Months[12][3] = {{"янв"},{"фев"},{"мар"},{"апр"},{"мая"},{"июн"},
                              {"июл"},{"авг"},{"сен"},{"окт"},{"ноя"},{"дек"}};
const char   Days[7][3]    = {{" Сб"},{" Вс"},{" Пн"},{" Вт"},{" Ср"},{" Чт"},{" Пт"}};
e_keyCode keyCode;
s_STATUS status = {.mainScreen = 1, .modeChanged = 1};
unsigned int menuPointer = 0, pagePointer = 0, modeMenuPointer = 0, backLight = 0, dayIntPointer = 0;
unsigned int tInterval;

#define  MENU_ITEMS_CNT  (5)
const char menuHead[MENU_ITEMS_CNT][14] = {{" Уст. времени "},
                                           {" Уст. даты    "},                                          
                                           {" Контраст     "},
                                           {" Конфигурация "},
                                           {" Выход        "}};

const char configMenuHead[MODES_CNT+1][14] = {{" Простой      "},
                                              {" Вегетация    "},
                                              {" Плодоношение "},
                                              {" Обслуживание "},
                                              {" Выход        "}};

#define MODES_MENU_ITEMS_CNT               (4)
const char modeMenuHead[MODES_MENU_ITEMS_CNT][13] = {{" Температура "},
                                                     {" Освещение   "},
                                                     {" Вентиляция  "},
                                                     {" Выход       "}};
#define PAGES_CNT               (2)

s_CONFIG configs[MODES_CNT];

//==============================================================================
void printTime(void)
{  
    picFromFlash(40, 135, 20, 30,  (date_time.TIME.hour/10)*4);
    picFromFlash(60, 135, 20, 30,  (date_time.TIME.hour%10)*4);
    picFromFlash(80, 135, 20, 30,  (11)*4);
    picFromFlash(100, 135, 20, 30, (date_time.TIME.min/10)*4);
    picFromFlash(120, 135, 20, 30, (date_time.TIME.min%10)*4);
    picFromFlash(140, 135, 20, 30, (11)*4);
    picFromFlash(160, 135, 20, 30, (date_time.TIME.sec/10)*4);
    picFromFlash(180, 135, 20, 30, (date_time.TIME.sec%10)*4);
}
//==============================================================================
void SetTime(void)
{
  status.subMenu = 1;
  picFromFlash(0, 0, 240, 135, FLASH_MENU_SCR);
  printString (50, 40, BLUE, WHITE, "Уст. времени");  
  printTime();
  unsigned int tmp = date_time.TIME.sec;  
  status.set1 = 1;
  
  printString (80, 80, BLUE, WHITE, "Секунды");
  while(status.subMenu == 1)
  {    
    if (status.keyPressed) 
    {
        status.keyPressed = 0;
        delay_ms(KEY_INTERVAL);
        switch(keyCode)
        {
              case downButt:
                tmp++;
                if ((((status.set2 == 1) || (status.set1 == 1)) && (tmp >= 60)) || ((status.set3 == 1) && (tmp >= 24))) tmp = 0;               
                break;
              case upButt:                
                if ((((status.set2 == 1) || (status.set1 == 1)) && (tmp == 0))) tmp = 60;    
                else if ((status.set3 == 1) && (tmp == 0)) tmp = 24;
                tmp--;
                break;
              case okButt:
                if       (status.set1){status.set1 = 0; status.set2 = 1;  printString (80, 80, BLUE, WHITE, " Минуты "); tmp = date_time.TIME.min;}
                else  if (status.set2){status.set2 = 0; status.set3 = 1;  printString (80, 80, BLUE, WHITE, "  Часы  "); tmp = date_time.TIME.hour;}
                else  if (status.set3){status.set3 = 0; status.subMenu = 0;}                
                break;
        }        
        if       (status.set1) {date_time.TIME.sec = tmp; }
        else  if (status.set2) {date_time.TIME.min = tmp; }
        else  if (status.set3){date_time.TIME.hour = tmp;}        
        printTime();
    }
    CLR_BUTT_INT();

    if(status.subMenu == 1)__low_power_mode_1();     
  }
  picFromFlash(0, 0, 240, 135, FLASH_MENU_SCR);
//  lcd_clear();//lcd_clear_menu();  
//  printString (51, 12, WHITE, BLUE, "Меню");
}

//==============================================================================
int  dayOfWeek;
//==============================================================================
void printDate(int aX, int aY, unsigned long aCol, unsigned  long aBGCol)
{
  int k = aX + 6;
  if(date_time.DATE.day >= 10) 
  {
    k = aX;
    char_6x8(k, aY,  aCol, aBGCol,date_time.DATE.day/10 + '0'); 
    k += 12;
  }
  else    char_6x8(aX,    aY,  aCol,aBGCol, ' ');
    
  
  char_6x8(k, aY, aCol, aBGCol, date_time.DATE.day%10 + 0x30); k += 12;
  char_6x8(k, aY,  aCol,aBGCol, ' ');    k += 12;
  for (unsigned int i = 0; i < 3; i++) {char_6x8(k, aY, aCol,aBGCol, Months[date_time.DATE.month][i]); k += 12;}
  char_6x8(k, aY,  aCol,aBGCol, ' ');k += 12;
  char_6x8(k, aY, aCol, aBGCol,  date_time.DATE.year/1000     + 0x30);k += 12;
  char_6x8(k, aY, aCol, aBGCol,  date_time.DATE.year%1000/100 + 0x30);k += 12;
  char_6x8(k, aY, aCol, aBGCol,  date_time.DATE.year%100/10   + 0x30);k += 12;
  char_6x8(k, aY, aCol, aBGCol,  date_time.DATE.year%10       + 0x30);k += 12;
  char_6x8(k, aY, aCol, aBGCol,  ',');k += 12;
  
  int m, Y = date_time.DATE.year;
  if (date_time.DATE.month < 2) { Y = Y - 1; m = date_time.DATE.month + 13;} 
  else                          {            m = date_time.DATE.month + 1;}   
  int dayOfWeek = (date_time.DATE.day + (26*(m+1)/10) + Y + Y/4 + 6*Y/100 + Y/400) % 7;
  
  for (unsigned int i = 0; i < 3; i++) {char_6x8(k, aY, aCol, aBGCol,  Days[dayOfWeek][i]);k += 12;}
  char_6x8(k, aY, aCol, aBGCol,  ' ');  
}
//==============================================================================
void SetDate(void)
{ 
  status.subMenu = 1;
  picFromFlash(0, 0, 240, 135, FLASH_MENU_SCR); 
  printString (50, 40, BLUE, WHITE, "Уст. даты");  
  printDate(28, 176, YELLOW, BLACK);
  unsigned int tmp = date_time.DATE.year;  
  status.set1 = 1; //year
  printString (80, 80, BLUE, WHITE, "Секунды"); 
  printString (80, 80, BLUE, WHITE, "  Год  ");
  while(status.subMenu == 1)
  {
    if (status.keyPressed) 
    {
      status.keyPressed = 0;
      delay_ms(KEY_INTERVAL);
      switch(keyCode)
        {
              case downButt:
                tmp++;
                if (status.set2  && (tmp >= 12)) tmp = 0;
                if (status.set3  && (tmp > daysInMonth[date_time.DATE.month])) tmp = 1;
                break;
              case upButt:
                if (status.set2  && (tmp == 0)) tmp = 12;
                if (status.set3  && (tmp == 1)) tmp = daysInMonth[date_time.DATE.month];
                tmp--;
                break;
              case okButt:
                 if       (status.set1) {status.set1 = 0; status.set2 = 1; printString (80, 80, BLUE, WHITE, " Месяц "); tmp = date_time.DATE.month;}
                 else  if (status.set2) {status.set2 = 0; status.set3 = 1; printString (80, 80, BLUE, WHITE, "  День "); tmp = date_time.DATE.day;}
                 else  if (status.set3) {status.set3 = 0; status.subMenu = 0;}                
                break;
        }        
    }    
    if       (status.set3){date_time.DATE.day   = tmp; }
    else  if (status.set2){date_time.DATE.month = tmp; }
    else  if (status.set1){date_time.DATE.year  = tmp;}    
    printDate(28, 176, YELLOW, BLACK);
    
    CLR_BUTT_INT();

    if(status.subMenu == 1) __low_power_mode_1();     
  }  
  picFromFlash(0, 0, 240, 135, FLASH_MENU_SCR); 
}

//==============================================================================
void mainMenu(void)
{
  status.menuMode = 1;  
  
  //lcd_clear();//lcd_clear_menu();                
  //printString (70, 20, BLUE, WHITE, "Меню");
  
  while(status.menuMode == 1)
  {    
    if (status.keyPressed) 
    {
      TA2CTL |= (MC_1 | TACLR);
      P2OUT |= BIT0;
      status.keyPressed = 0;
      delay_ms(KEY_INTERVAL);
      switch(keyCode)
        {
              case upButt:
                      menuPointer++;
                      if(menuPointer >= MENU_ITEMS_CNT) menuPointer = 0;
                      break;
              case downButt:
                      if(menuPointer == 0) menuPointer = MENU_ITEMS_CNT - 1;
                      else menuPointer--;
                      break;
              case okButt:
                      if      (menuPointer == 0) {TA_STOP; SetTime();TA_RUN;}
                      else if (menuPointer == 1) SetDate();
                      else if (menuPointer == 2) ;//setContrast();   
                      else if (menuPointer == 3) ;//config();
                      else if (menuPointer == 4) status.menuMode = 0;                       
                      break;
        }        
    }
    for(unsigned int k = 0; k < MENU_ITEMS_CNT; k++)
    {      
      for(unsigned int i = 0; i < 14; i++) 
      {
        if (k == menuPointer) char_6x8(35+i*12, (k+3)*16-10, WHITE, BLUE, menuHead[k][i]);
        else                  char_6x8(35+i*12, (k+3)*16-10, BLUE, WHITE, menuHead[k][i]);        
      }    
    }
    
    CLR_BUTT_INT();
    if(status.menuMode == 1) __low_power_mode_1();     
  } 
//  if (status.mainScreen) lcd_clear();//lcd_clear_main();  
//  else lcd_clear();
  //printString (70, 20, BLUE, WHITE, "Меню");
}
//==============================================================================
void keypressedProceed(void)
{
        status.keyPressed = 0;
        delay_ms(KEY_INTERVAL);
//        DA_EN(0);
        switch(keyCode)
        {
              case upButt:
                pagePointer++;
                if(pagePointer >= PAGES_CNT) pagePointer = 0;
                break;
                
              case downButt:
                if(pagePointer == 0) pagePointer = PAGES_CNT;
                pagePointer--;
                break;
                
              case okButt:
                picFromFlash(0, 0, 240, 135, FLASH_MENU_SCR);
                mainMenu();                                 
                //picFromFlash(0, 0, 240, 200, FLASH_MAIN_SCR);
                break;
        }
        switch(pagePointer)
        {
        case 0: picFromFlash(0, 0, 240, 320, 100); break;
        case 1: 
          tInterval = 0; 
          rectangle (0, 0,240, 320, BLACK);  
          status.plotRedraw = 1;
          break;
        }
        
        CLR_BUTT_INT();
        DA_EN(1);
}
//==============================================================================
void timeIncrement(void)
{
  date_time.TIME.sec++;
  if(date_time.TIME.sec >= 60) 
  {
    date_time.TIME.sec = 0;
    date_time.TIME.min++;
    if(date_time.TIME.min >= 60)
    {
      date_time.TIME.min = 0;
      date_time.TIME.hour++;
      if(date_time.TIME.hour >= 24) 
      {
        date_time.TIME.hour = 0;
        date_time.DATE.day++;
        if (date_time.DATE.day > daysInMonth[date_time.DATE.month])
        {
          date_time.DATE.day = 1;
          date_time.DATE.month ++;
          if(date_time.DATE.month >= 12)
          {
            date_time.DATE.month = 0;
            date_time.DATE.year++;
          }
        }
      }
    }
  }
}

////==============================================================================
//void write_flash(unsigned char * value)
//{
//  char *Flash_ptr;                          // Flash pointer
//  unsigned int i;
//
//  Flash_ptr = (char *)0x1000;               // Initialize Flash pointer
//  FCTL3 = FWKEY;                            // Clear Lock bit
//  FCTL1 = FWKEY + ERASE + EEI;              // Set Erase bit, allow interrupts
//  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
//
//  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
//
//  for (i = 0; i < 8*MODES_CNT; i++)
//  {
//    *Flash_ptr++ = *value++;                   // Write value to flash
//  }  
//  *Flash_ptr = (CURR_MODE);
//  FCTL1 = FWKEY;                            // Clear WRT bit
//  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
//}
//==============================================================================
signed int rcTemper;
//==============================================================================
#pragma vector = TIMER0_A0_VECTOR//TIMERA0_VECTOR
__interrupt void TA0_ISR (void)
   {    
     timeIncrement();
     status.timeChanged = 1;
     if (plotIntervalCntr) plotIntervalCntr--;
     else 
     {
       plotIntervalCntr = PLOT_INTERVAL;
       valueToBuffer(temp_buffer[0], tBuffer);
       valueToBuffer((signed int)rcTemper, tBuffer2);
       status.plotRedraw = 1;
     }
     if (!status.menuMode) __low_power_mode_off_on_exit();
   }
////==============================================================================
//#pragma vector=PORT2_VECTOR
//__interrupt void Key_ISR (void)
//{
//  if      (P2IFG & UP_BUTTON)   keyCode = upButt;
//  else if (P2IFG & DOWN_BUTTON) keyCode = downButt;
//  else if (P2IFG & OK_BUTTON)   keyCode = okButt;
//  status.keyPressed = 1; 
//  P2IFG &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
//  P2IE  &= ~(UP_BUTTON | DOWN_BUTTON | OK_BUTTON);
//   __low_power_mode_off_on_exit();
//}