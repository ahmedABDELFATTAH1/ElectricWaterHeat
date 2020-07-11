#ifndef EXT_EEPROM_H
#define	EXT_EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

//-----------[ Functions' Prototypes ]--------------
//---[ I2C Routines ]---
void I2C_Master_Init(const unsigned long baud);
void I2C_Master_Wait();
void I2C_Master_Start();
void I2C_Master_RepeatedStart();
void I2C_Master_Stop();
void I2C_ACK();
void I2C_NACK();
unsigned char I2C_Master_Write(unsigned char);
unsigned char I2C_Read_Byte(void);
//---[ EEPROM Routines ]---
void EEPROM_Write(unsigned char add, unsigned char data);
void EEPROM_Write_Page(unsigned int add, unsigned char* data, unsigned char len);
unsigned char EEPROM_Read(unsigned int add);
void EEPROM_Read_Page(unsigned int add, unsigned char* data, unsigned int len);


#ifdef	__cplusplus
}
#endif

#endif	/* EXT_EEPROM_H */

