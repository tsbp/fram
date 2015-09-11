//==============================================================================
#include "DS18B20.h"
#include "crc.h"
//==============================================================================
#define DQ_BIT  (BIT7)
#define DS_PORT_DIR  (P4DIR)
#define DS_PORT_IN   (P4IN)
#define DS_PORT_OUT  (P4OUT)

#define DQ1 (DS_PORT_DIR &= ~DQ_BIT)
#define DQ0 (DS_PORT_DIR |=  DQ_BIT) 
//==============================================================================
unsigned char Init_18B20(void) 
{ 
    unsigned char Error; 
    _DINT(); 
    DQ0; 
    delay_us(500); 
    DQ1; 
    delay_us(55); 
    if(DS_PORT_IN & DQ_BIT)   Error = 1; 
    else 
    { 
        Error = 0;
        DQ1; 
    } 
    _EINT(); 
     
    delay_us(400); 
    return Error; 
} 

//================= Read 1 bit of data from the 1-Wire bus =====================
unsigned char OWReadBit(void)
{   
  unsigned char temp = 0;
  DQ0; 
  delay_us(1);           
  DQ1; 
  delay_us(15);                     
  if(DS_PORT_IN & DQ_BIT)   temp = 0x01; 
  delay_us(55);                 
  //delay_us(10);           //—” ±10us 
  return temp;
}
//================= Send 1 bit of data to teh 1-Wire bus =======================
void OWWriteBit(unsigned char aBit)
{   
  DQ0; 
  delay_us(3);            
  if(aBit)    DQ1; 
  else        DQ0;         
  delay_us(50);           
  DQ1; 
  delay_us(10);   
}
//==============================================================================
void OWWriteByte(unsigned char wdata) 
{ 
    unsigned char i; 
     
    _DINT(); 
    for(i = 0; i < 8;i++) 
    { 
      if (wdata & BIT0) OWWriteBit(1);
      else OWWriteBit(0);
      wdata >>= 1;
    } 
    _EINT(); 
} 
//============================================================================== 
unsigned char OWReadByte(void) 
{    
    unsigned char temp = 0;      
    _DINT(); 
    for(unsigned char i = 0; i < 8; i++) 
    { 
      temp >>= 1; 
      if (OWReadBit()) temp |= 0x80;
    } 
    _EINT();      
    return  temp; 
}
//==============================================================================
#define FALSE 0
#define TRUE  1
//==============================================================================
void OWWriteByte(unsigned char byte_value);
void OWWriteBit(unsigned char bit_value);
unsigned char OWReadBit();
int  OWSearch();
//==============================================================================
unsigned char ROM_NO[8];
int LastDiscrepancy;
int LastFamilyDiscrepancy;
int LastDeviceFlag;
unsigned int DevicesCount;
unsigned char crc8_;


//==============================================================================
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//==============================================================================
int OWSearch()
{
   int id_bit_number;
   int last_zero, rom_byte_number, search_result;
   int id_bit, cmp_id_bit;
   unsigned char rom_byte_mask, search_direction;

   // initialize for search
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;
   crc8_ = 0;

   // if the last call was not the last one
   if (!LastDeviceFlag)
   {
      // 1-Wire reset
      if (!OWReset())
      {
         // reset the search
         LastDiscrepancy = 0;
         LastDeviceFlag = FALSE;
         LastFamilyDiscrepancy = 0;
         return FALSE;
      }

      // issue the search command 
      OWWriteByte(0xF0);  

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = OWReadBit();
         cmp_id_bit = OWReadBit();

         // check for no devices on 1-wire
         if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
               search_direction = id_bit;  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
              ROM_NO[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            OWWriteBit(search_direction);

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                crc8_ = crc8_step(ROM_NO[rom_byte_number], crc8_);  // accumulate the CRC
                //docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!((id_bit_number < 65) || (crc8_ != 0)))
      {
         // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
         LastDiscrepancy = last_zero;
         DevicesCount++;

         // check for last device
         if (LastDiscrepancy == 0)
            LastDeviceFlag = TRUE;
         
         search_result = TRUE;
      }
   }
   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !ROM_NO[0])
   {
      LastDiscrepancy = 0;
      LastDeviceFlag = FALSE;
      LastFamilyDiscrepancy = 0;
      search_result = FALSE;
   }

   return search_result;
}
//==============================================================================
// 1-Wire Functions to be implemented for a particular platform
//==============================================================================
//========= Reset the 1-Wire bus and return the presence of any device =========
int OWReset()
{ 
   return !Init_18B20();
}

//==============================================================================
s_DEV_ID device[MAX_DS1820_NUMB];
const unsigned char config[3] = {0x00, 0x00, CONF_REG};
unsigned char tmpBuf[9];
//==============================================================================
void OWConfig(void)
{
  for(unsigned int i = 0; i < DevicesCount; i++)
    {
      unsigned char  ok = 0;       
      do
      {
                while(Init_18B20());
                OWWriteByte(MATCH_ROM);
                for(unsigned int k = 0; k < 8; k++) OWWriteByte(device[i].data[k]);
                OWWriteByte(WRITE_SCRATCPAD);       
                for(unsigned int k = 0; k < 3; k++) OWWriteByte(config[k]);
                
                while(Init_18B20());
                OWWriteByte(MATCH_ROM);
                for(unsigned int k = 0; k < 8; k++) OWWriteByte(device[i].data[k]);
                OWWriteByte(READ_SCRATCPAD);       
                for(unsigned int k = 0; k < 9; k++) tmpBuf[k] = OWReadByte();
                if (tmpBuf[8] == crc8(tmpBuf, sizeof(tmpBuf) - 1, 0) && (tmpBuf[4] == CONF_REG)) ok = 1;
      }
      while(!ok);
    }
}
//==============================================================================
void OWFind(void)
{
  DevicesCount = 0;
  LastDiscrepancy = 0;
  LastDeviceFlag = FALSE;
  LastFamilyDiscrepancy = 0;
  do
  {
            OWSearch();//OWNext();
            for(unsigned int i = 0; i < 8; i++)
              device[DevicesCount - 1].data[i] = ROM_NO[i];    
  }
  while (!LastDeviceFlag);
}
//==============================================================================
signed int temp_buffer[MAX_DS1820_NUMB];
//==============================================================================
void readOWLine(unsigned int aDev)
{
          
        while(Init_18B20());
        OWWriteByte(MATCH_ROM);      
        for(unsigned int k = 0; k < 8; k++) OWWriteByte(device[aDev].data[k]);
        
        OWWriteByte(READ_SCRATCPAD); 
        
        signed int tmp;
        for(unsigned int n = 0; n < sizeof(tmpBuf); n++) tmpBuf[n] = OWReadByte();
        if (tmpBuf[8] == crc8(tmpBuf, sizeof(tmpBuf) - 1, 0))
        {
          tmp = (tmpBuf[0] | (tmpBuf[1] << 8));
          temp_buffer[aDev] =  (tmp >> 4) *10;
          temp_buffer[aDev] += (signed int)(tmp & 0x000f) * 0.0625 * 10;
        }        
          
}