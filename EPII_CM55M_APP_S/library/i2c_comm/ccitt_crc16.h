#ifndef CCITT_CRC16_H
#define CCITT_CRC16_H


typedef union
{
	struct 
	{
		uint8_t b0:1;
		uint8_t b1:1;
		uint8_t b2:1;
		uint8_t b3:1;
		uint8_t b4:1;
		uint8_t b5:1;
		uint8_t b6:1;
		uint8_t b7:1;
	};
	uint8_t byte;
} BYTE_T;

typedef union
{
	struct 
	{
		uint8_t b0:1;
		uint8_t b1:1;
		uint8_t b2:1;
		uint8_t b3:1;
		uint8_t b4:1;
		uint8_t b5:1;
		uint8_t b6:1;
		uint8_t b7:1;
		uint8_t b8:1;
		uint8_t b9:1;
		uint8_t b10:1;
		uint8_t b11:1;
		uint8_t b12:1;
		uint8_t b13:1;
		uint8_t b14:1;
		uint8_t b15:1;
	};
	uint16_t byte16;
} BYTE16_T;


unsigned short crc16_ccitt(char *buf, int len);

#endif
