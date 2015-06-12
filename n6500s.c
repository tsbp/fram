//==============================================================================
#include "n6500s.h"
#include "fonts.h"
//#include "img.h"
//#include "FRAM.h"
#include "dataflash.h"
//==============================================================================
void lcdWrite(unsigned char aType, unsigned char aByte)
{
  
  LCD_DCX(aType);
  //for(unsigned int i = 0; i < 0xff; i++);
  LCD_DATA = aByte ;
  //for(unsigned int i = 0; i < 0xff; i++);
  LCD_WRX(0);
  //for(unsigned int i = 0; i < 0xff; i++);
  LCD_WRX(1);
  //for(unsigned int i = 0; i < 0xff; i++);
}
//==============================================================================
void lcdRead(unsigned char aType, unsigned char aByte, unsigned char *aDst)
{
  LCD_DCX(aType);
  //for(unsigned int i = 0; i < 0xff; i++);
  LCD_DATA = aByte;
  //for(unsigned int i = 0; i < 0xff; i++);
  LCD_WRX(0);
  //for(unsigned int i = 0; i < 0xff; i++);
  LCD_WRX(1);
  //for(unsigned int i = 0; i < 0xff; i++);
  
  P6DIR = 0x00;
  LCD_REG_DATA_DIR(_IN);
  
  for( int i = 0; i < 2; i++)
  {
    aDst[i] = LCD_DATA_IN;
    LCD_RDX(0);
    //for(unsigned int i = 0; i < 0xff; i++);
    LCD_RDX(1);
    //for(unsigned int i = 0; i < 0xff; i++);
  }  
  LCD_REG_DATA_DIR(_OUT);  
  P6DIR = 0xff;
  
}
//==============================================================================
void LCD_Init(void)
{
  P6OUT = 0;
  P6DIR = 0xff;
  
  P1OUT = 0;
  P1DIR = 0xff;
  
  P5OUT |= (LCD_REG_DATA_OE_BIT | LCD_REG_DATA_DIR_BIT);
  P5DIR |= (LCD_REG_DATA_OE_BIT | LCD_REG_DATA_DIR_BIT);
  
  LCD_REG_CTRL_OE(0);
  LCD_REG_DATA_OE(0);
  LCD_REG_CTRL_DIR(_OUT);  
  LCD_REG_DATA_DIR(_OUT);    
  //=====================================
}
//==============================================================================
void LCD_wakeup(void)
{
  LCD_RDX(1);
  LCD_CSX(1);             //RES = 0;
  for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  LCD_CSX(0);             //RES = 1;
  LCD_WRX(1);              //WR = 1;
  LCD_RESX(0);             //RES = 0;
  for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  LCD_RESX(1);             //RES = 1;
  lcdWrite(COMMAND, SWRESET); //SendCom(0x01);       // Выход дисплея из спячки
  for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(5);
  lcdWrite(COMMAND, SLPOUT); //SendCom(0x11);       // Выход дисплея из спячки
  lcdWrite(COMMAND, DISPON); //SendCom(0x29);       // Включение дисплея
  for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  lcdWrite(COMMAND, MADCTR); //SendCom(0x36);
  lcdWrite(DATA,    0xC0); //SendDat(0xC0);       // Книжная ориентация дисплея
  lcdWrite(COMMAND, COLMOD); //SendCom(0x3A);
  lcdWrite(DATA,    0x07); //SendDat(0b00000111); // 24 битныйрежим
}
//==============================================================================
void setPixel(unsigned long aCol)
{
  lcdWrite(DATA, (unsigned char)(aCol >> 16));   
  lcdWrite(DATA, (unsigned char)(aCol >> 8));  
  lcdWrite(DATA, (unsigned char)aCol); 
}
////==============================================================================
//void char_6x8 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
//{
//   lcdWrite(COMMAND, CASET);
//   lcdWrite(DATA, (unsigned char)(aX >> 8));
//   lcdWrite(DATA, (unsigned char)(aX));
//   lcdWrite(DATA, (unsigned char)((aX+5) >> 8));
//   lcdWrite(DATA, (unsigned char)(aX+5));
//   
//   lcdWrite(COMMAND, RASET); 
//   lcdWrite(DATA, (unsigned char)(aY >> 8));
//   lcdWrite(DATA, (unsigned char)(aY)); 
//   lcdWrite(DATA, (unsigned char)((aY+8) >> 8)); 
//   lcdWrite(DATA, (unsigned char)(aY+8));
//   
//   lcdWrite(COMMAND, RAMWR);
//   
//   for(int i = 0; i < 8; i++)
//   {
//      for(int j = 0; j < 6; j++)
//      {
//        if (Font8x6[aChar*6+j] & (1 << i)) setPixel(aCOL);
//        else                               setPixel(aBGCOL);
//      }
//   }   
//}
//==============================================================================
void lcd_clear(void)
{
  for(unsigned int i = 0; i < 240; i++)   
      for(unsigned int j = 0; j < 32; j++) setPixel(BLACK);
      
}
//==============================================================================
void char_6x8 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);
   
   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+11) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+11));
      
   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY)); 
   lcdWrite(DATA, (unsigned char)((aY+16) >> 8)); 
   lcdWrite(DATA, (unsigned char)(aY+16));
   
   lcdWrite(COMMAND, RAMWR);
   
   for(int i = 0; i < 8; i++)
   {
      for(int j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL); setPixel(aCOL);}
        else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
      }
      for(int j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL); setPixel(aCOL);}
        else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
      }
   }   
}
//==============================================================================
void char_6x8_s (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);
   
   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+5) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+5));
      
   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY)); 
   lcdWrite(DATA, (unsigned char)((aY+7) >> 8)); 
   lcdWrite(DATA, (unsigned char)(aY+7));
   
   lcdWrite(COMMAND, RAMWR);
   
   for(int i = 0; i < 8; i++)
   {
      for(int j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL);}
        else                               { setPixel(aBGCOL);}
      }
      
   }   
}
//==============================================================================
void line (int aXs, int aYs,  int aXe, int aYe, unsigned long aCOL)
{
   int yn;
   int yo;   
   yo = aYs;
   for(unsigned int i = aXs+1; i <= aXe ; i++)
   {
     int a = (i- aXs);
     int b = (aYe-aYs);
     int c = (aXe-aXs);
     yn = ((a*b)/c) + aYs;
     if(yn >= yo)lineV(i, yo, yn-yo+1, aCOL);
     else        lineV(i, yn, yo-yn+1, aCOL);
     yo = yn;
   }
}
//==============================================================================
void lineV (unsigned int aXs, unsigned int aYs, unsigned int aLng, unsigned long aCOL)
{
   lcdWrite(COMMAND, CASET);
   
   lcdWrite(DATA, (unsigned char)((aXs) >> 8));
   lcdWrite(DATA, (unsigned char) (aXs));
   lcdWrite(DATA, (unsigned char)((aXs) >> 8));
   lcdWrite(DATA, (unsigned char) (aXs));
      
   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)((aYs) >> 8));
   lcdWrite(DATA, (unsigned char) (aYs)); 
   lcdWrite(DATA, (unsigned char)((aYs+aLng-1) >> 8)); 
   lcdWrite(DATA, (unsigned char) (aYs+aLng-1));
   
   lcdWrite(COMMAND, RAMWR);
   
   for(int i = 0; i < aLng; i++)
   {      
       setPixel(aCOL);   
   }   
}
//==============================================================================
void lineH (unsigned int aXs, unsigned int aYs, unsigned int aLng, unsigned long aCOL)
{
   lcdWrite(COMMAND, CASET);
   
   lcdWrite(DATA, (unsigned char)((aXs) >> 8));
   lcdWrite(DATA, (unsigned char) (aXs));
   lcdWrite(DATA, (unsigned char)((aXs+aLng-1) >> 8));
   lcdWrite(DATA, (unsigned char) (aXs+aLng-1));
      
   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)((aYs) >> 8));
   lcdWrite(DATA, (unsigned char) (aYs)); 
   lcdWrite(DATA, (unsigned char)((aYs) >> 8)); 
   lcdWrite(DATA, (unsigned char) (aYs));
   
   lcdWrite(COMMAND, RAMWR);
   
   for(int i = 0; i < aLng; i++)
   {      
       setPixel(aCOL);   
   }   
}
//==============================================================================
void rectangle (unsigned int aXs, unsigned int aYs, unsigned int aXe, unsigned int aYe, unsigned long aCOL)
{
   lcdWrite(COMMAND, CASET);
   
   lcdWrite(DATA, (unsigned char)((aXs) >> 8));
   lcdWrite(DATA, (unsigned char) (aXs));
   lcdWrite(DATA, (unsigned char)((aXe-1) >> 8));
   lcdWrite(DATA, (unsigned char) (aXe-1));
      
   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)((aYs) >> 8));
   lcdWrite(DATA, (unsigned char) (aYs)); 
   lcdWrite(DATA, (unsigned char)((aYe - 1) >> 8)); 
   lcdWrite(DATA, (unsigned char) (aYe - 1));
   
   lcdWrite(COMMAND, RAMWR);
   
   for(int i = 0; i < (aXe - aXs); i++)         
     for(int i = 0; i < (aYe - aYs); i++)       setPixel(aCOL);   
      
}
//==============================================================================
void printString (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char *aStr)
{
  while (*aStr)
  {
    char_6x8(aX, aY, aCOL, aBGCOL, *aStr++);
    aX += 12;
  }
}
//==============================================================================
void printDigit_16x24(unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aDig)
{
   lcdWrite(COMMAND, CASET);
   lcdWrite(DATA, (unsigned char)(aX >> 8));
   lcdWrite(DATA, (unsigned char)(aX)); 
   lcdWrite(DATA, (unsigned char)((aX+15) >> 8)); 
   lcdWrite(DATA, (unsigned char)(aX+15));   

   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+24) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+24));
   
   lcdWrite(COMMAND, RAMWR);
   
   for(int k = 0; k < 3; k++)
   {
      for(int i = 0; i < 8; i++)
           {
              for(int j = 0; j < 16; j++)
              {
                if (font_16x24[aDig*16*3 + 16*k+j] & (1 << i)) setPixel(aCOL);
                else                                           setPixel(aBGCOL); 
              }
           }        
   }  
}
////==============================================================================
//void pictute(unsigned int aX, unsigned int aY)
//{
//   lcdWrite(COMMAND, CASET);
//   lcdWrite(DATA, (unsigned char)(aX >> 8));
//   lcdWrite(DATA, (unsigned char)(aX)); 
//   lcdWrite(DATA, (unsigned char)((aX+79) >> 8));
//   lcdWrite(DATA, (unsigned char)(aX+79));   
//
//   lcdWrite(COMMAND, RASET); 
//   lcdWrite(DATA, (unsigned char)(aY >> 8));
//   lcdWrite(DATA, (unsigned char)(aY)); 
//   lcdWrite(DATA, (unsigned char)((aY+79) >> 8));
//   lcdWrite(DATA, (unsigned char)(aY+79));
//   
//   lcdWrite(COMMAND, RAMWR);
//   
//   for(int k = 0; k < sizeof(img); k++) lcdWrite(DATA, img[k]);    
//}
////==============================================================================
//void digitFromDram(unsigned int aX, unsigned int aY, unsigned int aDig)
//{
//   lcdWrite(COMMAND, CASET);
//   lcdWrite(DATA, (unsigned char)(aX >> 8));
//   lcdWrite(DATA, (unsigned char)(aX)); 
//   lcdWrite(DATA, (unsigned char)((aX+19) >> 8));
//   lcdWrite(DATA, (unsigned char)(aX+19));   
//
//   lcdWrite(COMMAND, RASET); 
//   lcdWrite(DATA, (unsigned char)(aY >> 8));
//   lcdWrite(DATA, (unsigned char)(aY)); 
//   lcdWrite(DATA, (unsigned char)((aY+29) >> 8));
//   lcdWrite(DATA, (unsigned char)(aY+29));
//   
//   lcdWrite(COMMAND, RAMWR);  
//   unsigned char tmp[30];
//   for(int i = 0; i < 60; i++)
//   {
//     framRead(i*30 + aDig*1800, tmp, sizeof(tmp));
//     for(int k = 0; k < sizeof(tmp); k++) lcdWrite(DATA, tmp[k]); 
//   }    
//      
//}
////==============================================================================
//void picFromDram(unsigned int aX, unsigned int aY, unsigned int aH, unsigned int aW, unsigned long aAddr)
//{   
//   lcdWrite(COMMAND, RASET);
//   lcdWrite(DATA, (unsigned char)(aX >> 8));
//   lcdWrite(DATA, (unsigned char)(aX)); 
//   lcdWrite(DATA, (unsigned char)((aX+(aW-1)) >> 8));
//   lcdWrite(DATA, (unsigned char)(aX +(aW-1)));   
//
//   lcdWrite(COMMAND, CASET); 
//   lcdWrite(DATA, (unsigned char)(aY >> 8));
//   lcdWrite(DATA, (unsigned char)(aY)); 
//   lcdWrite(DATA, (unsigned char)((aY+(aH-1)) >> 8));
//   lcdWrite(DATA, (unsigned char)(aY +(aH-1)));
//   
//   lcdWrite(COMMAND, RAMWR); 
//#define TMP_CNT (60) 
//   unsigned char tmp[60];
//   unsigned long freaks = aH*aW*3/TMP_CNT;
//   
//   for(unsigned long i = 0; i < freaks; i++)
//   {
//     
//     framRead((i*60)+ aAddr, tmp, sizeof(tmp));
//     for(int k = 0; k < sizeof(tmp); k++) lcdWrite(DATA, tmp[k]); 
//   }    
//      
//}
////==============================================================================

//==============================================================================
void picFromFlash(unsigned int aX, unsigned long aY, unsigned long aH, unsigned int aW, unsigned int dOfset)
{   
  lcdWrite(COMMAND, CASET);
   lcdWrite(DATA, (unsigned char)(aX >> 8));
   lcdWrite(DATA, (unsigned char)(aX)); 
   lcdWrite(DATA, (unsigned char)((aX+(aH-1)) >> 8));
   lcdWrite(DATA, (unsigned char)(aX +(aH-1)));   

   lcdWrite(COMMAND, RASET); 
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY)); 
   lcdWrite(DATA, (unsigned char)((aY+(aW-1)) >> 8));
   lcdWrite(DATA, (unsigned char)(aY +(aW-1)));
   
   lcdWrite(COMMAND, RAMWR); 
#define _TMP_CNT (528) 
   unsigned char tmp[_TMP_CNT];
   unsigned long freaks = aH*aW*3/_TMP_CNT;
   unsigned long tale = aH*aW*3%_TMP_CNT;   
    
   for(unsigned long i = 0; i < freaks; i++)
   {     
     //framRead((i*60)+ aAddr, tmp, sizeof(tmp));
     df_FlashRead(dOfset+i, 0, _TMP_CNT, tmp);
     for(unsigned long k = 0; k < sizeof(tmp); k++) lcdWrite(DATA, tmp[k]); 
   }    
   if(tale)
   {
     //i++;
     df_FlashRead(dOfset+freaks, 0, tale, tmp);
     for(unsigned long k = 0; k < tale; k++) lcdWrite(DATA, tmp[k]); 
   }
}