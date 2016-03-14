#ifndef EEPROM_24LC16BT_I_H
#define EEPROM_24LC16BT_I_H

void write1byteEEPROM(uint8_t block, uint8_t address, uint8_t write_data);
uint8_t read1byteEEPROM(uint8_t block, uint8_t adress);

void writeNbyteEEPROM(uint8_t block, uint8_t address, uint8_t *read_data_array, uint16_t length);
void readNbyteEEPROM(uint8_t block, uint8_t address, uint8_t *read_data_array, uint16_t length);

void printBlockData(uint8_t block);
void printAllData(void);

#endif
