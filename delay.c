//==============================================================================
#include "delay.h"
//==============================================================================
void delay_us(unsigned int auS)
{
  for(unsigned int i=0;i<auS;i++)
  {
    __no_operation(); 
    __no_operation(); 
    __no_operation(); 
    __no_operation();
    __no_operation(); 
    __no_operation(); 
    __no_operation(); 
    __no_operation();
    __no_operation(); 
    __no_operation(); 
    __no_operation(); 
    __no_operation();
    __no_operation(); 
    __no_operation();    
   __no_operation(); 
    __no_operation(); 
    __no_operation();
    __no_operation(); 
   
    
  }
}
/******************************************************************************/
void delay_ms(unsigned int amS)
{
  for(unsigned int i=0;i<amS;i++)
  {
    __no_operation();
    delay_us(999);    
  }
}
