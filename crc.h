#ifndef _CRC_H
#define _CRC_H

#define CRC_BEGIN_VAL_32  (0xffffffff)
#define CRC_BEGIN_VAL_16  (0xffff)
#define CRC_BEGIN_VAL_8   (0xff)
#define CRC_BEGIN_VAL_16  (0xffff)
#define CRC_BEGIN_VAL_8   (0xff)

// расчет контрольной суммы CRC8 для массива байт ...
extern unsigned char crc8(unsigned char* data, unsigned int dataSize, unsigned char curCrc);
extern unsigned char crc8check(unsigned char* data, unsigned int dataSize);

// расчет контрольной суммы для одного байта 
extern unsigned char crc8_step(unsigned char data, unsigned char curCrc);

// расчет контрольной суммы CRC16 для массива байт ...
extern unsigned int crc16(unsigned char* data, unsigned int dataSize, unsigned int curCrc);

// расчет контрольной суммы для одного байта 
extern unsigned int crc16_step(unsigned char data, unsigned int curCrc);

//расчет crc16 от IARa
extern unsigned short crc16_slow(const unsigned char *p, unsigned int len, unsigned short sum);

#endif
