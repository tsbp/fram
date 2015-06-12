/*************************************************************************************
*
*  ������� Atmel DataFlash
*
*  by:      BSVi
*  version: 1.00
*
*************************************************************************************/

#ifndef _DATA_FLASH_H
#define _DATA_FLASH_H

#include "msp430.h"

typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;




// � ���� ����� ���� ������� �������� �������. ��������� ����������
// � ����������� � ������������ ���������� ���������� ������� ����� �
// "�����" ���� ������� � ����� dataflash.cpp


// ��������������� ��� �������, ����� ��������� ��������������� ���� ������������
// ���������� dataflash �� ����� ���������� ���������
#define DF_AUTODETECT_FEATURES

// ��������������� ��� �������, ���� ����� df_LinearSpace �� ������������
#define DF_ENABLE_LINEAR_SPACE

// �������� ������������ ������������ ����������.
typedef struct 
{
   uint16  pages;       // ���������� �������
   uint16  page_size;   // ������ �������� � ������
   uint8   page_bit;    // ����� ������ ��������
   uint8   chip_id;     // ��� ����������
}df_Info_t;

extern df_Info_t df_Info;

// ���������� S_OK ���� ���������� ����������. ����� ������ ���� �������,
// ��������� ����������� ���������� ����� ������ �� ��������� df_Info
unsigned char df_Init();

// �������� ����������� �� ���������� ��������. ������ ������� ��� ���������
// ���������� ��������. ��� ������������� ������������ ��� �������.
uint8 df_isReady();
uint8 df_GetStatus();

// ������ � ������ � ������. ��. dataflash.cpp
void df_WriteStr( uint8 BufferNo, uint16 Addr, uint16 Count, uint8 *BufferPtr );
void df_WriteByte( uint8 BufferNo, uint16 Addr, uint8 Data );

void df_GetStr( uint8 BufferNo, uint16 Addr, uint16 Count, uint8 *BufferPtr );
uint8 df_GetChar(uint8 BufferNo, uint16 Addr );

// ������ ��������������� �� ����-������
void df_FlashRead( uint16 PageAdr, uint16 Addr, uint16 Count, uint8 *BufferPtr );

void df_PageFunc( uint8 PageCmd, uint16 PageAdr );
/*****************************************************************************
*  ���� ������� ��� ������������� � df_PageFunc
******************************************************************************/
#define DF_FLASH_TO_BUF1 		0x53	/* Main memory page to buffer 1 transfer  */
#define DF_FLASH_TO_BUF2 		0x55	/* Main memory page to buffer 2 transfer */
#define DF_BUF1_TO_FLASH_WITH_ERASE   	0x83	/* Buffer 1 to main memory page program with built-in erase */
#define DF_BUF2_TO_FLASH_WITH_ERASE   	0x86	/* Buffer 2 to main memory page program with built-in erase */
#define DF_BUF1_TO_FLASH_WITHOUT_ERASE  0x88	/* Buffer 1 to main memory page program without built-in erase */
#define DF_BUF2_TO_FLASH_WITHOUT_ERASE  0x89	/* Buffer 2 to main memory page program without built-in erase */


/*****************************************************************************
*  ���������� ��� �������� ������ � ������ ����������
******************************************************************************/
#ifdef DF_ENABLE_LINEAR_SPACE
// ����� ������������ dataflash ��� �������� ������������ � ��������� ������������ ��
// ������������� ������� �� ���������� ������, �������������� �������, �� �����������,
// ��������� � ��������� �/�� ��������. �� ������ ������� ������� �� ����� ��������
// �������� �������.
//
// ���� ������������� ��� ������ ������ - Write � UnsafeWrite. Write ������ ���������� ������
// � ������������ ������ �� � ��� �����������. UnsafeWrite �������������  ��� ����������������
// ������ ��������� ������ ������. ����� ���������� ���������������� ������� UnsafeWrite ����� �����
// ������ ��������� � dataflash ������ ���� ������ ����� Finalize, ����� ���������� �������
// ����� ���� ���������� �� ����, ��� ����� ���������� � �������� ����-������.
//
// GoToZero ���������� �� GoTo(0) ���, ��� ������ �������� ����������� �������.
//
// ��� ������� �� ���������� �������� �� ����������� ������ ������. �� ������ ���� ������� �� ����.
// ������ ���������� ���� �� ������� ����� �� ����� ������ ����� ������� SpaceToEnd
//
// ����� ��������� �������� ����������� ���������� ��. dataflash.cpp



  
   //uint16 page;
//   uint16 address;
   //uint16 page_in_buffer1;

 
   //inline df_LinearSpace() { GoToZero(); }
   void GoToZero();
   uint32 SpaceToEnd();
   void Finalize();
   void Read( uint8 *buffer, uint16 count );
   void UnsafeWrite( uint8 *buffer, uint16 count );
   //void __Write( uint8 *buffer, uint16 count ) { UnsafeWrite(buffer, count); Finalize();}
   void SeekForward(  uint32 displacement );
   void SeekBackward(  uint32 displacement );
   void GoTo( uint32 location );


#endif /* DF_ENABLE_LINEAR_SPACE */
#endif /* _DATA_FLASH_H */
