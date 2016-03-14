#ifndef MPU9250_H
#define MPU9250_H

void initMPU9250 (void);
void initAK8963 (void);

void updateMPU9250Data();
void updateAK8963Data();

void getDataFromMPU9250 (uint8_t *dest_array);
void getDataFromAK8963 (uint8_t *dest_array);

void whoAmI(void);

void getOmega(float *vec);
void getMag(float *vec);
void getAcc(float *vec);
void getTemp(float *temp);

void getOmegaOffset(int16_t *vec);
void getAccOffset(int16_t *vec);
void getMagOffset(int16_t *vec);

void setOmegaOffset(int16_t *offset_vec);
void setAccOffset(int16_t *offset_vec);
void setMagOffset(int16_t *offset_vec);

void setOmegaOffset_x(int16_t offset_x);
void setOmegaOffset_y(int16_t offset_y);
void setOmegaOffset_z(int16_t offset_z);

void setAccOffset_x(int16_t offset_x);
void setAccOffset_y(int16_t offset_y);
void setAccOffset_z(int16_t offset_z);

void setMagOffset_x(int16_t offset_x);
void setMagOffset_y(int16_t offset_y);
void setMagOffset_z(int16_t offset_z);

void getAsa(int16_t *vec);
void getOmegaInt(int16_t *vec);
void getACCInt(int16_t *vec);
void getMagInt(int16_t *vec);
void getTempInt(int16_t *temp);

void getRawOmegaInt(int16_t *vec);
void getRawAccInt(int16_t *vec);
void getRawMagInt(int16_t *vec);
void getRawTempInt(int16_t *temp);

void calibOmegaOffset();
void calibAccXYOffset();
void calibAccZOffset();
void calibMagOffset();

void debugMPU9250(void);

void writeEEPROMOffsetAccInt(int16_t *vec);
void writeEEPROMOffsetOmegaInt(int16_t *vec);
void writeEEPROMOffsetMagInt(int16_t *vec);

void readEEPROMOffsetAccInt(int16_t *vec);
void readEEPROMOffsetOmegaInt(int16_t *vec);
void readEEPROMOffsetMagInt(int16_t *vec);

void initOffsetByEEPROM();

#endif
