//==============================================================================
#include <msp430.h>
//==============================================================================
#define SIMO_BIT    BIT1
#define SOMI_BIT    BIT2
#define CLK_BIT     BIT3
#define CS_BIT      BIT0
#define _CS(n){if(n) P4OUT |= CS_BIT; else P4OUT &= ~CS_BIT;}
//============== Op-code Commands ==============================================
#define WREN  (0x06) // Set Write Enable Latch 0000 0110b
#define WRDI  (0x04) // Write Disable 0000 0100b
#define RDSR  (0x05) // Read Status Register 0000 0101b
#define WRSR  (0x01) // Write Status Register 0000 0001b
#define READ  (0x03) // Read Memory Data 0000 0011b
#define FSTRD (0x0b) // Fast Read Memory Data 0000 1011b
#define WRITE (0x02) // Write Memory Data 0000 0010b
#define SLEEP (0xb9) // Enter Sleep Mode 1011 1001b
#define RDID  (0x9f) // Read Device ID 1001 1111b
#define SNR   (0xc3) // Read S/N 1100 0011b
//==============================================================================
#define SPI_SEND_BYTE(n) { UCB1TXBUF = n; while ((UCB1STAT&UCBUSY));   } 
//==============================================================================
extern void framOpCode(unsigned char  aCode, unsigned char* aBuf);
extern void framWrite(unsigned long aAddr, unsigned char *aData, unsigned int aCnt);
extern void framRead(unsigned long aAddr, unsigned char *aData, unsigned int aCnt);
extern void framSPIinit(void);
//==============================================================================
